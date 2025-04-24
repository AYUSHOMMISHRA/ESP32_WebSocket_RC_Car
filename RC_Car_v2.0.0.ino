/**
 * @file ESP32_RC_Car_WiFi_WebSocket_RFID.ino
 * @brief Firmware for an ESP32 based RC car controlled over WiFi via WebSockets,
 * featuring RFID authorization, ultrasonic obstacle avoidance, and telemetry.
 * @version 2.0.0
 * @date 2025-04-17
 *
 * @details This code runs on an ESP32 microcontroller and performs the following functions:
 * - Connects to a specified WiFi network.
 * - Starts an HTTP server (port 80) to serve a web control interface.
 * - Starts a WebSocket server (port 81) for real-time communication (commands, telemetry).
 * - Controls two DC motors via an L298N motor driver for movement (forward, backward, left, right, stop).
 * - Implements basic obstacle avoidance using an HC-SR04 ultrasonic sensor.
 * - Uses an MFRC522 RFID reader to scan RFID tags/cards for user authorization.
 * - Requires authorization via RFID before accepting movement commands.
 * - Implements an authorization timeout.
 * - Sends telemetry data (RSSI, authorization status, distance, etc.) back to the client via WebSocket.
 * - Handles WebSocket PING/PONG for connection keep-alive.
 * - Includes logic for checking and re-establishing WiFi connection.
 *
 * Hardware Connections:
 * - L298N Motor Driver: ENA -> GPIO 13, IN1 -> GPIO 15, IN2 -> GPIO 14, ENB -> GPIO 27, IN3 -> GPIO 26, IN4 -> GPIO 25
 * - HC-SR04 Ultrasonic Sensor: TRIG -> GPIO 33, ECHO -> GPIO 32
 * - MFRC522 RFID Reader: SDA (SS) -> GPIO 5, SCK -> GPIO 18 (Default SPI), MOSI -> GPIO 23 (Default SPI), MISO -> GPIO 19 (Default SPI), RST -> GPIO 4
 *
 * Dependencies:
 * - WiFi.h (ESP32 Core)
 * - WebSocketServer.h (Requires a WebSocket library, e.g., arduinoWebSockets or similar adapted)
 * - index.h (Contains the HTML/JS for the web interface, stored as a C++ string literal)
 * - SPI.h (ESP32 Core)
 * - MFRC522.h (Requires MFRC522 library by miguelbalboa)
 * - ArduinoJson.h (Requires ArduinoJson library by bblanchon)
 * - Arduino.h (ESP32 Core)
 */

// =============================================================================
// Includes
// =============================================================================
#include <WiFi.h>             // For WiFi connectivity
#include "WebSocketServer.h"  // For WebSocket communication (replace with your actual library header if different)
#include "index.h"            // Contains the HTML/JS content for the web UI
#include <SPI.h>              // For SPI communication (used by RFID)
#include <MFRC522.h>          // For RFID reader interaction
#include <ArduinoJson.h>      // For easy JSON creation and parsing
#include <Arduino.h>          // Core Arduino framework functions

// =============================================================================
// Command Definitions
// =============================================================================
#define CMD_STOP 0      ///< Command code to stop the car.
#define CMD_FORWARD 1   ///< Command code to move the car forward.
#define CMD_BACKWARD 2  ///< Command code to move the car backward.
#define CMD_LEFT 4      ///< Command code to turn the car left.
#define CMD_RIGHT 8     ///< Command code to turn the car right.

// =============================================================================
// Motor Control Pin Definitions & Configuration
// =============================================================================
// Motor Driver (L298N) pin Connections to ESP32
const int ENA = 13; ///< Enable pin for Motor A (controls speed via PWM).
const int IN1 = 15; ///< Input 1 for Motor A (controls direction). Changed from 12 to avoid boot issues.
const int IN2 = 14; ///< Input 2 for Motor A (controls direction).
const int ENB = 27; ///< Enable pin for Motor B (controls speed via PWM).
const int IN3 = 26; ///< Input 1 for Motor B (controls direction).
const int IN4 = 25; ///< Input 2 for Motor B (controls direction).

// Define PWM properties for motor control (Note: `analogWrite` on ESP32 handles setup implicitly)
int motorSpeed = 200;         ///< Default motor speed PWM value (0-255 range).

// =============================================================================
// Ultrasonic Sensor Pin Definitions & Configuration
// =============================================================================
const int trigPin = 33; ///< GPIO pin connected to the TRIG pin of the HC-SR04 sensor.
const int echoPin = 32; ///< GPIO pin connected to the ECHO pin of the HC-SR04 sensor.

// =============================================================================
// Obstacle Avoidance State Variables
// =============================================================================
const int STOP_DISTANCE = 100; ///< Minimum distance (cm) before obstacle avoidance triggers or forward motion is blocked.

/**
 * @enum ObstacleAvoidanceState
 * @brief Defines the states for the obstacle avoidance state machine.
 */
enum ObstacleAvoidanceState {
  IDLE,           ///< Obstacle avoidance is not active.
  BACKING_UP,     ///< The car is currently moving backward as part of the avoidance maneuver.
  COMPLETING      ///< The car is finishing the avoidance maneuver (e.g., pausing briefly).
};
ObstacleAvoidanceState avoidanceState = IDLE; ///< Current state of the obstacle avoidance state machine.
unsigned long avoidanceStateStartTime = 0; ///< Timestamp (millis) when the current avoidance state began.
bool avoidingObstacle = false; ///< Flag indicating if the obstacle avoidance routine is currently active.
int lastDistance = 100; ///< Last measured distance from the ultrasonic sensor (cm). Initialized to a safe value.
unsigned long lastAvoidanceTime = 0; ///< Timestamp (millis) of the last time avoidance was active (can be used for debouncing or timing).
int lastSentCommand = CMD_STOP; ///< Stores the last valid command received via WebSocket, used for state management.

// =============================================================================
// WiFi Configuration
// =============================================================================
const char *ssid = " ";   ///< Your WiFi network SSID (Network Name).
const char *password = " ";  ///< Your WiFi network password.
// Note: Hardcoding credentials is not recommended for production. Consider using WiFiManager or other secure methods.

// =============================================================================
// RFID Configuration
// =============================================================================
#define RFID_SS_PIN  5  ///< SPI Slave Select (SS/SDA) pin for the MFRC522 RFID reader.
#define RFID_RST_PIN 4  ///< Reset pin for the MFRC522 RFID reader. Changed from default to avoid boot issues.

/**
 * @brief MFRC522 RFID reader instance.
 * @param ssPin The SPI Slave Select pin.
 * @param rstPin The Reset pin.
 */
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);

// =============================================================================
// Authorization State Variables
// =============================================================================
bool isAuthorized = false; ///< Flag indicating if a valid RFID tag has been scanned and the session is active.
String authorizedUser = ""; ///< Stores the name of the currently authorized user.
unsigned long lastAuthorizedActivity = 0; ///< Timestamp (millis) of the last authorized command or RFID scan.
const unsigned long AUTH_TIMEOUT = 300000; ///< Authorization timeout duration in milliseconds (5 minutes).

// =============================================================================
// Network Server Objects
// =============================================================================
WiFiServer httpServer(80); ///< HTTP server instance listening on port 80.
net::WebSocketServer webSocket(81); ///< WebSocket server instance listening on port 81. (Note: 'net::' prefix depends on the specific library used)

// =============================================================================
// Authorized RFID Users Definition
// =============================================================================
/**
 * @struct AuthorizedUser
 * @brief Structure to hold the UID and name of an authorized RFID user.
 */
struct AuthorizedUser {
  byte uid[4];  ///< 4-byte Unique Identifier (UID) of the RFID tag/card.
  String name;  ///< Name associated with the UID.
};

/**
 * @brief Array defining the list of authorized users.
 * Add or remove users by modifying this array. Ensure UIDs match your specific RFID tags/cards.
 */
const AuthorizedUser authorizedUsers[] = {
  {{0x4B, 0x17, 0xE2, 0x00}, "User 1"}, 
  {{0x63, 0xED, 0x38, 0x2D}, "User 2"}
  // Add authorized users here in the format: {{0xXX, 0xXX, 0xXX, 0xXX}, "User Name"}
};

// =============================================================================
// Concurrency Control
// =============================================================================
/**
 * @brief Simple mutex flag to prevent race conditions when updating shared state variables,
 * particularly between the WebSocket handler and the obstacle avoidance logic.
 */
bool stateUpdateInProgress = false;

// =============================================================================
// Ultrasonic Sensor Timing
// =============================================================================
unsigned long lastUltrasonicTrigger = 0; ///< Timestamp (millis) of the last ultrasonic sensor trigger.
const unsigned long ULTRASONIC_INTERVAL = 200; ///< Minimum interval (ms) between ultrasonic measurements to avoid echo interference and reduce load.

// =============================================================================
// WebSocket Message Handling Function
// =============================================================================
/**
 * @brief Callback function executed when a WebSocket message is received.
 * @param client The WebSocket client instance that sent the message.
 * @param dataType The type of WebSocket message received (e.g., TEXT, BINARY).
 * @param message Pointer to the message payload.
 * @param length Length of the message payload.
 *
 * @details Parses incoming text messages. Handles PING requests. Validates commands.
 * Checks authorization status before executing movement commands.
 * Interacts with the obstacle avoidance system to prevent forward motion if blocked.
 * Sends feedback messages (errors, RFID requests, obstacle notifications) to the client.
 */
void handleWebSocketMessage(net::WebSocket &client, net::WebSocket::DataType dataType, const char *message, uint16_t length) {
    // Only process text messages
    if (dataType == net::WebSocket::DataType::TEXT) {
        // Convert the received message payload to an Arduino String
        String cmd_str = "";
        for (uint16_t i = 0; i < length; i++) {
            cmd_str += (char)message[i];
        }

        // Handle PING messages for keep-alive
        if (cmd_str.startsWith("PING")) {
            String pongMessage = "PONG:" + String(millis());
            client.send(net::WebSocket::DataType::TEXT, pongMessage.c_str(), pongMessage.length());
            return; // Exit after handling PING
        }

        // Attempt to convert the command string to an integer
        int command = cmd_str.toInt();

        // Validate if the command is one of the recognized movement/stop commands
        bool validCommand = (command == CMD_STOP || command == CMD_FORWARD ||
                           command == CMD_BACKWARD || command == CMD_LEFT || command == CMD_RIGHT);

        // If the command is invalid, send an error message and exit
        if (!validCommand) {
            client.send(net::WebSocket::DataType::TEXT, "ERROR:Invalid command", 23); // 23 is length of "ERROR:Invalid command"
            return;
        }

        Serial.print("Received command: ");
        Serial.println(command);

        // If the user is authorized, update their last activity timestamp to prevent timeout
        if (isAuthorized) {
            lastAuthorizedActivity = millis();
        }

        // If the command is a movement command but the user is not authorized
        if (!isAuthorized && command != CMD_STOP) {
            Serial.println("Command rejected: Not authorized");

            // Prepare a JSON response indicating authorization is required
            StaticJsonDocument<128> doc; // Use StaticJsonDocument for fixed buffer size on ESP32
            doc["authorized"] = false;
            doc["message"] = "Authentication required";

            String authJson;
            serializeJson(doc, authJson); // Convert JSON object to string

            // Prefix the message for client-side parsing
            String authMessage = "RFID:" + authJson;

            // Send the authorization request message
            client.send(net::WebSocket::DataType::TEXT, authMessage.c_str(), authMessage.length());
            return; // Exit, do not process the movement command
        }

        // --- Critical Section Check ---
        // Wait if the obstacle avoidance routine is currently modifying state
        while (stateUpdateInProgress) {
            yield(); // Allow other tasks (like background WiFi) to run
        }
        // --- End Critical Section Check ---

        // Process the command if obstacle avoidance is NOT active OR if the command is STOP
        if (!avoidingObstacle || command == CMD_STOP) {
            lastSentCommand = command; // Store the latest valid command

            switch (command) {
                case CMD_STOP:
                    Serial.println("Stop");
                    CAR_stop(); // Execute stop motor function
                    break;
                case CMD_FORWARD:
                    // Only move forward if the path is clear
                    if (lastDistance > STOP_DISTANCE) {
                        Serial.println("Move Forward");
                        CAR_moveForward(); // Execute forward motor function
                    } else {
                        // Path is blocked, notify the client
                        StaticJsonDocument<128> doc;
                        doc["active"] = true;       // Indicate obstacle detection is active
                        doc["distance"] = lastDistance; // Send the current distance

                        String obstacleJson;
                        serializeJson(doc, obstacleJson);

                        // Prefix the message
                        String obstacleMessage = "OBSTACLE:" + obstacleJson;
                        client.send(net::WebSocket::DataType::TEXT, obstacleMessage.c_str(), obstacleMessage.length());

                        // Override command to STOP and log the blockage
                        lastSentCommand = CMD_STOP;
                        Serial.println("Forward blocked by obstacle");
                        CAR_stop();

                        // Explicitly trigger the obstacle avoidance state machine
                        // This ensures avoidance starts even if the command was initially blocked here
                        avoidingObstacle = true;
                        avoidanceState = BACKING_UP; // Start by backing up
                        avoidanceStateStartTime = millis();
                    }
                    break;
                case CMD_BACKWARD:
                    Serial.println("Move Backward");
                    CAR_moveBackward(); // Execute backward motor function
                    break;
                case CMD_LEFT:
                    Serial.println("Turn Left");
                    CAR_turnLeft(); // Execute left turn motor function
                    break;
                case CMD_RIGHT:
                    Serial.println("Turn Right");
                    CAR_turnRight(); // Execute right turn motor function
                    break;
            }
        } else {
            // If trying to move while obstacle avoidance is active, notify the client
            StaticJsonDocument<128> doc;
            doc["active"] = true;
            doc["distance"] = lastDistance;
            doc["message"] = "Obstacle avoidance in progress";

            String avoidanceJson;
            serializeJson(doc, avoidanceJson);

            // Prefix the message
            String avoidanceMessage = "OBSTACLE:" + avoidanceJson;
            client.send(net::WebSocket::DataType::TEXT, avoidanceMessage.c_str(), avoidanceMessage.length());
        }
    }
}

// =============================================================================
// Ultrasonic Sensor Reading Function
// =============================================================================
/**
 * @brief Measures the distance using the HC-SR04 ultrasonic sensor.
 * @details Triggers the sensor, reads the echo pulse duration, and calculates
 * the distance in centimeters. Includes a non-blocking delay (`ULTRASONIC_INTERVAL`)
 * between measurements and basic filtering for unreasonable values. Updates the
 * global `lastDistance` variable. Uses a timeout for `pulseIn` for robustness.
 */
void updateUltrasonicSensor() {
  unsigned long currentMillis = millis();

  // Throttle sensor readings to avoid interference and reduce processing load
  if (currentMillis - lastUltrasonicTrigger < ULTRASONIC_INTERVAL) {
    return; // Not time to measure yet
  }

  lastUltrasonicTrigger = currentMillis; // Record the time of this trigger

  // Generate the ultrasonic trigger pulse (10 microseconds HIGH)
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2); // Short low pulse to ensure clean rising edge
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the duration of the echo pulse (HIGH state)
  // pulseIn returns the duration in microseconds or 0 if timeout occurs.
  // Timeout (30000 us = 30 ms) prevents blocking if no echo is received.
  unsigned long duration = pulseIn(echoPin, HIGH, 30000);

  // Calculate distance if a valid pulse was received
  if (duration > 0) {
    // Distance (cm) = (Duration (us) * Speed of Sound (cm/us)) / 2
    // Speed of sound ~ 0.0343 cm/us
    int distance = (duration * 0.0343) / 2;

    // Basic filter: Only update if the distance is within a plausible range (e.g., > 0 and < 400 cm)
    if (distance > 0 && distance < 400) {
      lastDistance = distance;
    }
    // Optionally, add more sophisticated filtering (e.g., moving average) here
  }
  // If duration is 0 (timeout), lastDistance retains its previous value.
}


// =============================================================================
// RFID Checking Function
// =============================================================================
/**
 * @brief Checks for nearby RFID tags/cards using the MFRC522 reader.
 * @details If a new card is detected and read successfully, its UID is compared
 * against the `authorizedUsers` list. Updates the `isAuthorized` flag and
 * `authorizedUser` string accordingly. Records the `lastAuthorizedActivity` time
 * if authorization is successful. Broadcasts the authorization status (success/failure
 * and username) to all connected WebSocket clients in JSON format, prefixed with "RFID:".
 */
void checkRFID() {
    // Check if a new RFID tag/card is present in the reader's field
    if (!rfid.PICC_IsNewCardPresent()) {
        return; // No new card, exit function
    }

    // Try to read the serial number (UID) of the card
    if (!rfid.PICC_ReadCardSerial()) {
        return; // Failed to read card, exit function
    }

    // Card detected and read successfully

    // Store the UID (typically 4 bytes for Mifare Classic)
    byte cardUID[4];
    for (byte i = 0; i < sizeof(cardUID); i++) {
        cardUID[i] = rfid.uid.uidByte[i];
    }

    // Print the scanned UID to the Serial monitor for debugging
    Serial.print("Scanned UID: ");
    for (byte i = 0; i < sizeof(cardUID); i++) {
        Serial.print(cardUID[i], HEX); // Print in hexadecimal format
        Serial.print(" ");
    }
    Serial.println();

    bool foundMatch = false;        // Flag to indicate if the scanned UID matches an authorized user
    String userName = "Unauthorized User"; // Default username if no match is found

    // Iterate through the list of authorized users
    for (byte i = 0; i < sizeof(authorizedUsers) / sizeof(authorizedUsers[0]); i++) {
        bool match = true; // Assume match until proven otherwise
        // Compare the scanned UID byte-by-byte with the current authorized user's UID
        for (byte j = 0; j < sizeof(cardUID); j++) {
            if (cardUID[j] != authorizedUsers[i].uid[j]) {
                match = false; // Mismatch found
                break;         // No need to compare further bytes for this user
            }
        }
        // If all bytes matched
        if (match) {
            foundMatch = true;
            userName = authorizedUsers[i].name; // Get the name of the authorized user
            break; // Exit the loop, user found
        }
    }

    // Update the global authorization state based on whether a match was found
    isAuthorized = foundMatch;
    authorizedUser = userName; // Update the global authorized user name

    // If authorization was successful, update the last activity timestamp
    if (isAuthorized) {
        lastAuthorizedActivity = millis();
    }

    // Prepare JSON response to send via WebSocket
    StaticJsonDocument<128> doc; // Adjust size as needed
    doc["authorized"] = foundMatch;
    doc["user"] = userName;

    String authJson;
    serializeJson(doc, authJson); // Serialize the JSON object to a string

    // Prefix the JSON string for client-side identification
    String rfidMessage = "RFID:" + authJson;

    // Broadcast the authorization status to all connected WebSocket clients
    webSocket.broadcast(net::WebSocket::DataType::TEXT, rfidMessage.c_str(), rfidMessage.length());

    // Halt communication with the current PICC (RFID tag/card)
    rfid.PICC_HaltA();
    // Stop encryption (relevant for Mifare Classic)
    rfid.PCD_StopCrypto1();

    // Log the authorization result to the Serial monitor
    if (foundMatch) {
        Serial.print("Authorization granted to: ");
        Serial.println(userName);
    } else {
        Serial.println("Authorization denied");
    }
}

// =============================================================================
// Telemetry Sending Function
// =============================================================================
/**
 * @brief Periodically sends telemetry data to all connected WebSocket clients.
 * @details Gathers data like WiFi RSSI, authorization status, ultrasonic distance,
 * obstacle avoidance status, last command, and avoidance state. Formats the data
 * into a JSON string, prefixed with "TELEMETRY:", and broadcasts it.
 * Runs approximately every 100ms (controlled by `lastUpdate` check).
 */
void sendTelemetryData() {
    static unsigned long lastUpdate = 0; // Stores the time of the last telemetry update
    unsigned long currentMillis = millis();

    // Limit the frequency of telemetry updates (e.g., 10 times per second)
    if (currentMillis - lastUpdate < 100) { // 100ms interval
        return; // Not time to send yet
    }

    // Prepare JSON document
    StaticJsonDocument<256> doc; // Allocate enough space for telemetry data

    // Add telemetry data points to the JSON document
    doc["rssi"] = WiFi.RSSI();                // WiFi signal strength
    doc["authorized"] = isAuthorized;         // Current authorization status
    doc["distance"] = lastDistance;           // Last measured ultrasonic distance
    doc["obstacleAvoidance"] = avoidingObstacle; // Is obstacle avoidance currently active?
    doc["currentCommand"] = lastSentCommand;  // Last command received/being executed
    doc["avoidanceState"] = (int)avoidanceState; // Current state of the avoidance FSM

    // Serialize JSON object to a string
    String telemetryJson;
    serializeJson(doc, telemetryJson);

    // Prefix the JSON string for client-side identification
    String telemetryMessage = "TELEMETRY:" + telemetryJson;

    // Broadcast the telemetry message to all connected WebSocket clients
    webSocket.broadcast(net::WebSocket::DataType::TEXT,
                       telemetryMessage.c_str(),
                       telemetryMessage.length());

    lastUpdate = currentMillis; // Record the time of this update
}


// =============================================================================
// Obstacle Avoidance State Machine Handler
// =============================================================================
/**
 * @brief Manages the obstacle avoidance state machine.
 * @details This function is called repeatedly in the main loop.
 * 1. Sets a mutex (`stateUpdateInProgress`) to prevent conflicts with WebSocket commands.
 * 2. Updates the ultrasonic sensor reading (`updateUltrasonicSensor`).
 * 3. Checks if avoidance should be triggered (obstacle detected while moving forward).
 * 4. If triggered, sets `avoidingObstacle` flag, changes state to `BACKING_UP`,
 * notifies clients, and initiates backward movement.
 * 5. If avoidance is already active, progresses through the states (`BACKING_UP`, `COMPLETING`)
 * based on elapsed time (`avoidanceStateStartTime`).
 * 6. Controls motor actions (stop, back up) according to the current avoidance state.
 * 7. When the maneuver is complete, resets state to `IDLE`, clears the `avoidingObstacle` flag,
 * stops the car, and notifies clients that avoidance is finished.
 * 8. Releases the mutex.
 */
void handleObstacleAvoidance() {
  // --- Enter Critical Section ---
  stateUpdateInProgress = true; // Prevent WebSocket handler from changing motor state concurrently

  // Always get the latest distance reading at the start of the handler
  updateUltrasonicSensor();

  // --- Condition to START Obstacle Avoidance ---
  // Trigger avoidance ONLY if:
  // - Avoidance is not already active.
  // - An obstacle is within the STOP_DISTANCE.
  // - The car was last commanded to move FORWARD.
  if (!avoidingObstacle && lastDistance <= STOP_DISTANCE && lastSentCommand == CMD_FORWARD) {
    Serial.println("DETECTED Obstacle while moving forward! Starting avoidance sequence");
    avoidingObstacle = true;          // Set the flag
    avoidanceState = BACKING_UP;      // Set the initial state
    avoidanceStateStartTime = millis(); // Record the start time for this state

    // Notify clients that obstacle avoidance has started
    StaticJsonDocument<128> doc;
    doc["active"] = true;
    doc["distance"] = lastDistance;
    String obstacleJson;
    serializeJson(doc, obstacleJson);
    String obstacleMessage = "OBSTACLE:" + obstacleJson;
    webSocket.broadcast(net::WebSocket::DataType::TEXT, obstacleMessage.c_str(), obstacleMessage.length());

    // Immediately start the first action of the avoidance sequence
    Serial.println("Starting backward movement for avoidance");
    CAR_moveBackward(); // Start backing up
  }

  // --- State Machine Logic (only executes if avoidingObstacle is true) ---
  if (avoidingObstacle) {
    unsigned long currentTime = millis();
    unsigned long stateElapsedTime = currentTime - avoidanceStateStartTime; // Time spent in current state

    switch (avoidanceState) {
      case BACKING_UP:
        // State Action: Continue backing up (started when state was entered)
        // State Transition Condition: Back up for a fixed duration (e.g., 1 second)
        if (stateElapsedTime > 1000) { // Back up duration: 1000 ms
          Serial.println("Backing up completed, transitioning to COMPLETING");
          avoidanceState = COMPLETING;         // Move to the next state
          avoidanceStateStartTime = currentTime; // Reset timer for the new state

          // Action upon exiting BACKING_UP state: Stop the car before the pause
          CAR_stop();
        }
        break;

      case COMPLETING:
        // State Action: Pause briefly after backing up (car is already stopped)
        // State Transition Condition: Pause for a fixed duration (e.g., 0.3 seconds)
        if (stateElapsedTime > 300) { // Pause duration: 300 ms
          Serial.println("Obstacle avoidance maneuver completed");
          avoidanceState = IDLE;             // Return to IDLE state
          avoidingObstacle = false;          // Clear the avoidance flag
          lastSentCommand = CMD_STOP;        // Reset last command to prevent immediate forward motion

          // Notify clients that obstacle avoidance is finished
          StaticJsonDocument<128> doc;
          doc["active"] = false;             // Indicate avoidance is no longer active
          doc["distance"] = lastDistance;    // Send final distance reading
          String clearJson;
          serializeJson(doc, clearJson);
          String clearMessage = "OBSTACLE:" + clearJson;
          webSocket.broadcast(net::WebSocket::DataType::TEXT, clearMessage.c_str(), clearMessage.length());

          // No motor action needed here, car is already stopped and state is reset.
          // The car will remain stopped until a new command is received.
        }
        break;

      default:
        // Should not happen in normal operation
        Serial.println("ERROR: Invalid obstacle avoidance state. Resetting to IDLE.");
        avoidanceState = IDLE;
        avoidingObstacle = false;
        CAR_stop(); // Ensure car is stopped
        break;
    }
  }

  // --- Exit Critical Section ---
  stateUpdateInProgress = false; // Release the mutex
}


// =============================================================================
// Authorization Timeout Check Function
// =============================================================================
/**
 * @brief Checks if the authorized session has expired due to inactivity.
 * @details If `isAuthorized` is true, it compares the current time (`millis()`)
 * with the `lastAuthorizedActivity` timestamp. If the difference exceeds
 * `AUTH_TIMEOUT`, it resets `isAuthorized` to false, clears `authorizedUser`,
 * and broadcasts an "RFID: " message indicating the session expired.
 */
void checkAuthTimeout() {
  // Only check timeout if a user is currently authorized
  if (isAuthorized) {
    // Check if the time elapsed since the last activity exceeds the timeout period
    if (millis() - lastAuthorizedActivity > AUTH_TIMEOUT) {
      isAuthorized = false;    // De-authorize the user
      authorizedUser = "";     // Clear the authorized user name

      // Prepare JSON message indicating session expiration
      StaticJsonDocument<128> doc;
      doc["authorized"] = false;
      doc["message"] = "Session expired";

      String timeoutJson;
      serializeJson(doc, timeoutJson);

      // Prefix the message
      String timeoutMessage = "RFID:" + timeoutJson;

      // Broadcast the timeout message to all clients
      webSocket.broadcast(net::WebSocket::DataType::TEXT,
                         timeoutMessage.c_str(),
                         timeoutMessage.length());

      Serial.println("Authorization timeout - session expired");
      CAR_stop(); // Stop the car as a safety measure upon timeout
    }
  }
}

// =============================================================================
// WiFi Connection Management Function
// =============================================================================
/**
 * @brief Periodically checks the WiFi connection status and attempts reconnection if lost.
 * @details Runs approximately every 5 seconds. If `WiFi.status()` is not `WL_CONNECTED`,
 * it attempts to reconnect using `WiFi.begin()`. Prints status messages to Serial.
 */
void checkWiFiConnection() {
  static unsigned long lastWiFiCheck = 0; // Time of the last connection check
  unsigned long currentMillis = millis();

  // Check connection status periodically (e.g., every 5 seconds)
  if (currentMillis - lastWiFiCheck < 5000) {
    return; // Not time to check yet
  }

  lastWiFiCheck = currentMillis; // Record the time of this check

  // If WiFi is not connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Attempting to reconnect...");

    // Attempt to reconnect using the stored credentials
    WiFi.begin(ssid, password);

    int retries = 0;
    // Wait for connection, with a retry limit (e.g., 10 attempts * 500ms = 5 seconds)
    while (WiFi.status() != WL_CONNECTED && retries < 10) {
      delay(500);
      Serial.print(".");
      retries++;
    }

    // Check the result after attempting reconnection
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi reconnected");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP()); // Print the new IP address (might change)
    } else {
      Serial.println("\nWiFi reconnection failed");
      // Consider further actions if reconnection fails persistently (e.g., reset ESP32)
    }
  }
}


// =============================================================================
// Setup Function
// =============================================================================
/**
 * @brief Initializes the ESP32, peripherals, networking, and servers.
 * @details Runs once at startup.
 * - Initializes Serial communication.
 * - Configures motor control pins and ultrasonic sensor pins as outputs/inputs.
 * - Stops motors initially.
 * - Connects to the configured WiFi network with retries. Halts if connection fails.
 * - Starts the HTTP server on port 80.
 * - Starts the WebSocket server on port 81 and sets up the connection/message handlers.
 * - Initializes the SPI bus and the MFRC522 RFID reader, including a hardware reset.
 * - Checks the RFID reader version to verify initialization.
 * - Performs an initial ultrasonic sensor reading.
 * - Prints status messages to the Serial monitor.
 */
void setup() {
  // Start Serial communication for debugging
  Serial.begin(9600);
  Serial.println("\n\n--- ESP32 RC Car Initializing ---");

  // --- Initialize Motor Control Pins ---
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  Serial.println("Motor pins initialized.");

  // --- Initialize Ultrasonic Sensor Pins ---
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.println("Ultrasonic sensor pins initialized.");

  // Ensure motors are stopped at startup
  CAR_stop();
  Serial.println("Motors stopped initially.");

  // --- Connect to WiFi ---
  Serial.print("Connecting to WiFi SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); // Start connection attempt
  int wifi_retries = 0;
  // Wait for connection with a timeout (20 * 500ms = 10 seconds)
  while (WiFi.status() != WL_CONNECTED && wifi_retries < 20) {
    delay(500);
    Serial.print(".");
    wifi_retries++;
  }

  // Check connection result
  if (WiFi.status() != WL_CONNECTED) {
      Serial.println("\nWiFi connection FAILED! Halting execution.");
      // Stop here if WiFi connection is critical for operation
      while(true) { delay(1000); } // Infinite loop
  }

  // WiFi connection successful
  Serial.println("\nWiFi connected successfully!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); // Print assigned IP address

  // --- Start HTTP Server ---
  httpServer.begin();
  Serial.println("HTTP server started on port 80.");

  // --- Configure and Start WebSocket Server ---
  webSocket.begin(); // Initialize the WebSocket server

  // Register a callback function to handle new client connections
  webSocket.onConnection([](net::WebSocket& ws) {
    // When a client connects, register the message handler for that specific client
    ws.onMessage(handleWebSocketMessage);
    Serial.println("WebSocket client connected");
    // Optionally send a welcome message or initial state here
  });
  Serial.println("WebSocket server started on port 81.");

  // --- Initialize RFID Reader ---
  Serial.println("Initializing RFID reader...");
  // Perform a hardware reset on the RFID module for potentially better stability
  pinMode(RFID_RST_PIN, OUTPUT);
  digitalWrite(RFID_RST_PIN, LOW);
  delay(50); // RST low duration
  digitalWrite(RFID_RST_PIN, HIGH);
  delay(50); // Wait for chip to stabilize after reset

  SPI.begin();        // Initialize SPI bus (SCK, MOSI, MISO pins)
  rfid.PCD_Init();    // Initialize MFRC522 reader

  // Check if the reader initialized correctly by reading its version register
  byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);
  if (version == 0x00 || version == 0xFF) {
    // 0x00 or 0xFF typically indicates communication failure
    Serial.println("WARNING: RFID reader failed to initialize correctly. Check wiring/connections.");
  } else {
    Serial.print("RFID Reader Version: 0x");
    Serial.println(version, HEX); // Print the firmware version (e.g., 0x91 or 0x92)
    Serial.println("RFID Reader initialized. Waiting for card/tag scan for authorization.");
  }

  // --- Initial Sensor Readings ---
  updateUltrasonicSensor(); // Get an initial distance reading
  Serial.print("Initial distance reading: ");
  Serial.print(lastDistance);
  Serial.println(" cm");

  Serial.println("--- RC Car System Ready ---");
}

// =============================================================================
// Main Loop
// =============================================================================
/**
 * @brief The main execution loop of the program.
 * @details Continuously performs the following actions:
 * - Checks WiFi connection status and attempts reconnection if needed (`checkWiFiConnection`).
 * - Scans for RFID tags/cards and handles authorization (`checkRFID`).
 * - Checks for authorization timeout (`checkAuthTimeout`).
 * - Runs the obstacle avoidance state machine (`handleObstacleAvoidance`).
 * - Sends telemetry data to clients (`sendTelemetryData`).
 * - Listens for and handles incoming HTTP requests (`httpServer.available`).
 * - Listens for and processes incoming WebSocket messages (`webSocket.listen`).
 */
void loop() {
  // Maintain WiFi connection
  checkWiFiConnection();

  // Check for RFID card scans and handle authorization
  checkRFID();

  // Check if the authorized session has timed out
  checkAuthTimeout();

  // Run the obstacle avoidance logic (includes ultrasonic updates)
  handleObstacleAvoidance(); // This manages states and motor actions during avoidance

  // Send periodic status updates to clients
  sendTelemetryData();

  // --- Handle HTTP Client Connections ---
  WiFiClient httpClient = httpServer.available(); // Check for incoming HTTP clients
  if (httpClient) { // If a new client has connected
    Serial.println("[HTTP] New Client Connection");
    if (httpClient.connected()) { // Double-check connection
        if (httpClient.available()) { // If there's data waiting to be read
            // Read the first line of the request (e.g., "GET / HTTP/1.1")
            String line = httpClient.readStringUntil('\r');
            httpClient.find("\r\n\r\n"); // Consume the rest of the headers

            Serial.print("<< [HTTP] Request: ");
            Serial.println(line);

            // Simple routing: Respond to GET requests for the root path "/"
            if (line.startsWith("GET /")) {
                // Send standard HTTP response headers
                httpClient.println("HTTP/1.1 200 OK");
                httpClient.println("Content-Type: text/html");
                httpClient.println("Connection: close"); // Close connection after response
                httpClient.println(); // End of headers
                // Send the HTML content stored in the "index.h" file
                httpClient.println(HTML_CONTENT);
            } else {
                // Respond with Method Not Allowed for other request types (POST, etc.)
                httpClient.println("HTTP/1.1 405 Method Not Allowed");
                httpClient.println("Connection: close");
                httpClient.println();
            }
        }
    }
    // Short delay before closing allows data to be sent
    delay(5);
    // Close the connection
    httpClient.stop();
    Serial.println("[HTTP] Client Disconnected");
  }

  // --- Process WebSocket Communications ---
  // This function checks for new messages, handles disconnections, etc.
  // for all connected WebSocket clients. The actual message processing
  // happens in the `handleWebSocketMessage` callback.
  webSocket.listen();

  // Optional small delay to prevent WDT issues if loop is too tight,
  // but yield() in handleWebSocketMessage helps.
  // delay(1);
}

// =============================================================================
// Low-Level Motor Control Functions
// =============================================================================

/**
 * @brief Moves the car forward.
 * @details Sets the IN pins for both motors to drive forward and applies PWM
 * signal to ENA and ENB pins based on `motorSpeed`.
 */
void CAR_moveForward() {
  // Motor A Forward: IN1=HIGH, IN2=LOW
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  // Motor B Forward: IN3=LOW, IN4=HIGH (Note: May need swapping based on wiring)
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  // Set speed for both motors
  analogWrite(ENA, motorSpeed); // Apply PWM to Motor A enable pin
  analogWrite(ENB, motorSpeed); // Apply PWM to Motor B enable pin
}

/**
 * @brief Moves the car backward.
 * @details Sets the IN pins for both motors to drive backward and applies PWM
 * signal to ENA and ENB pins based on `motorSpeed`.
 */
void CAR_moveBackward() {
  // Motor A Backward: IN1=LOW, IN2=HIGH
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  // Motor B Backward: IN3=HIGH, IN4=LOW (Note: May need swapping based on wiring)
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Set speed for both motors
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
}

/**
 * @brief Turns the car left
 * @details Motor A moves backward, Motor B moves forward. Applies PWM signal
 * to ENA and ENB pins based on `motorSpeed`.
 */
void CAR_turnLeft() {
  // Motor A Backward: IN1=LOW, IN2=HIGH
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  // Motor B Forward: IN3=LOW, IN4=HIGH
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  // Set speed for both motors
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
}

/**
 * @brief Turns the car right (原地右转 - pivots right).
 * @details Motor A moves forward, Motor B moves backward. Applies PWM signal
 * to ENA and ENB pins based on `motorSpeed`.
 */
void CAR_turnRight() {
  // Motor A Forward: IN1=HIGH, IN2=LOW
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  // Motor B Backward: IN3=HIGH, IN4=LOW
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Set speed for both motors
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
}

/**
 * @brief Stops the car.
 * @details Sets all IN pins to LOW (brake low) and sets PWM duty cycle to 0
 * on ENA and ENB pins, effectively disabling the motors.
 */
void CAR_stop() {
  // Set all direction pins LOW
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Set speed (PWM duty cycle) to 0
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}