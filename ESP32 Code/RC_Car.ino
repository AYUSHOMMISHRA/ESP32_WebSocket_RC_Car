/**
 * ===============================================================================
 * ESP32 WiFi RC Car Controller
 * ===============================================================================
 * 
 * @file      RC_Car.ino
 * @brief     Complete implementation of a WiFi-controlled RC car using ESP32
 * @author    Ayush Verma / Ayush Mishra
 * @date      April 03, 2025
 * @version   1.0.0
 * 
 * @copyright Copyright (c) 2025 Ayush Verma & Ayush Mishra. All rights reserved.
 * @license   MIT License
 * 
 * -------------------------------------------------------------------------------
 * PROJECT DESCRIPTION:
 * -------------------------------------------------------------------------------
 * This project implements a comprehensive solution for controlling a RC car
 * wirelessly through WiFi using an ESP32 microcontroller. The system provides
 * a web-based interface accessible through any browser, allowing real-time
 * control of the car's movements with minimal latency.
 * 
 * The implementation creates two server endpoints:
 * 1. HTTP Server (Port 80): Serves the web control interface
 * 2. WebSocket Server (Port 81): Handles real-time bidirectional communication
 * 
 * The control interface allows for standard directional commands:
 * - Forward
 * - Backward
 * - Left Turn
 * - Right Turn
 * - Stop
 * 
 * -------------------------------------------------------------------------------
 * HARDWARE REQUIREMENTS:
 * -------------------------------------------------------------------------------
 * - ESP32 Development Board
 * - L298N Motor Driver Module
 * - DC Motors (2x)
 * - RC Car Chassis
 * - 7.4V-12V Battery Pack
 * - Jumper Wires
 * 
 * -------------------------------------------------------------------------------
 * PIN CONNECTIONS:
 * -------------------------------------------------------------------------------
 * ESP32 | L298N Motor Driver
 * ---------------------------
 * GPIO13 | ENA (Enable Motor A)
 * GPIO15 | IN1 (Input 1 Motor A)
 * GPIO14 | IN2 (Input 2 Motor A)
 * GPIO27 | ENB (Enable Motor B)
 * GPIO26 | IN3 (Input 3 Motor B)
 * GPIO25 | IN4 (Input 4 Motor B)
 * 
 * -------------------------------------------------------------------------------
 * DEPENDENCIES:
 * -------------------------------------------------------------------------------
 * - WiFi.h (Arduino ESP32 Core)
 * - WebSocketServer.h (Custom WebSocket implementation)
 * - index.h (HTML/CSS/JS for web interface)
 * 
 * -------------------------------------------------------------------------------
 * REPOSITORY STRUCTURE:
 * -------------------------------------------------------------------------------
 * - wifi_car_controller.ino: Main code file (this file)
 * - WebSocketServer.h: Custom WebSocket server implementation
 * - index.h: Web interface HTML content
 * - README.md: Project documentation
 * - LICENSE: License information
 * - /docs: Additional documentation
 * - /schematics: Circuit diagrams
 * 
 * -------------------------------------------------------------------------------
 * USAGE:
 * -------------------------------------------------------------------------------
 * 1. Update WiFi credentials (ssid, password)
 * 2. Flash code to ESP32
 * 3. Open Serial Monitor (9600 baud) to view debug information
 * 4. Connect to ESP32's IP address in a web browser
 * 5. Use the web interface to control the car
 * 
 * -------------------------------------------------------------------------------
 */

#include <WiFi.h>
#include "WebSocketServer.h"  // Custom WebSocket implementation
#include "index.h"          // Web interface HTML content

/**
 * Command Definitions
 * 
 * These constants define the command codes sent from the web interface
 * to control the car. The values are chosen to allow for potential
 * bit-wise combination of commands in future enhancements.
 */
#define CMD_STOP     0  // 0b000000 - Stop all motors
#define CMD_FORWARD  1  // 0b000001 - Move forward
#define CMD_BACKWARD 2  // 0b000010 - Move backward
#define CMD_LEFT     4  // 0b000100 - Turn left
#define CMD_RIGHT    8  // 0b001000 - Turn right

/**
 * Motor Driver Pin Configuration
 * 
 * These constants define the ESP32 GPIO pins connected to the L298N motor driver.
 * The L298N controls two DC motors with direction and speed control capabilities.
 * 
 * ENA/ENB: PWM-capable pins to control motor speed
 * IN1-IN4: Direction control pins for both motors
 * 
 * Motor A (Left):  ENA, IN1, IN2
 * Motor B (Right): ENB, IN3, IN4
 */
const int ENA = 13;  // Enable pin for Motor A (Left Motor)
const int IN1 = 15;  // Input 1 for Motor A
const int IN2 = 14;  // Input 2 for Motor A
const int ENB = 27;  // Enable pin for Motor B (Right Motor)
const int IN3 = 26;  // Input 3 for Motor B
const int IN4 = 25;  // Input 4 for Motor B

/**
 * WiFi Configuration
 * 
 * SSID and password for the WiFi network.
 * The ESP32 will connect to this network and create servers
 * accessible to devices on the same network.
 * 
 * @note For security in public repositories, consider using environment variables
 *       or a separate configuration file not tracked by version control.
 */
const char *ssid = "";      // WiFi network name
const char *password = "";  // WiFi password

/**
 * Server Instances
 * 
 * Two server objects are created to handle different aspects of communication:
 * 1. httpServer: Traditional HTTP server to deliver the web interface
 * 2. webSocket: WebSocket server for real-time bidirectional communication
 */
WiFiServer httpServer(80);           // HTTP server on port 80
net::WebSocketServer webSocket(81);  // WebSocket server on port 81

/**
 * @brief Processes incoming WebSocket messages and controls the RC car
 * 
 * This callback function is triggered whenever a connected WebSocket client
 * sends a message. It parses the message as a command code and executes
 * the corresponding motor control function.
 * 
 * @param client    The WebSocket client that sent the message
 * @param dataType  Type of data received (text/binary)
 * @param message   Pointer to the message data
 * @param length    Length of the message in bytes
 * 
 * @details The function expects text messages containing a single integer
 *          representing a command code. It converts the message to an integer
 *          and executes the appropriate motor control function based on
 *          the command code.
 */
void handleWebSocketMessage(net::WebSocket &client, net::WebSocket::DataType dataType, const char *message, uint16_t length) {
  // Only process text messages
  if (dataType == net::WebSocket::DataType::TEXT) {
    // Convert message bytes to string
    String cmd_str = "";
    for(uint16_t i = 0; i < length; i++) {
      cmd_str += (char)message[i];
    }

    // Parse command string to integer
    int command = cmd_str.toInt();
    Serial.print("Received command: ");
    Serial.println(command);

    // Execute corresponding motor control function
    switch (command) {
      case CMD_STOP:
        Serial.println("Stop");
        CAR_stop();
        break;
      case CMD_FORWARD:
        Serial.println("Move Forward");
        CAR_moveForward();
        break;
      case CMD_BACKWARD:
        Serial.println("Move Backward");
        CAR_moveBackward();
        break;
      case CMD_LEFT:
        Serial.println("Turn Left");
        CAR_turnLeft();
        break;
      case CMD_RIGHT:
        Serial.println("Turn Right");
        CAR_turnRight();
        break;
      default:
        Serial.println("Unknown command");
        // No action for unknown commands
    }
  }
}

/**
 * @brief One-time setup function that runs at startup
 * 
 * This function initializes all components of the system:
 * 1. Serial communication for debugging
 * 2. GPIO pins for motor control
 * 3. WiFi connection
 * 4. HTTP and WebSocket servers
 * 
 * The function attempts to connect to WiFi with a timeout, and if
 * connection fails, it halts the program to prevent uncontrolled operation.
 */
void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("\n\n====================================");
  Serial.println("ESP32 WiFi RC Car Controller");
  Serial.println("====================================");
  Serial.println("Initializing system...");

  // Configure motor control pins
  Serial.println("Configuring motor control pins...");
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Initialize motors to stopped state
  Serial.println("Setting initial motor state...");
  CAR_stop();
  
  // Enable motor drivers at full power
  // Note: These could be PWM signals for speed control
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);

  // Connect to WiFi network
  Serial.print("Connecting to WiFi network: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  // Wait for connection with timeout
  int wifi_retries = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_retries < 20) {
    delay(500);
    Serial.print(".");
    wifi_retries++;
  }

  // Handle connection failure
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nWiFi connection FAILED!");
    Serial.println("Check your credentials or network availability.");
    Serial.println("System halted for safety reasons.");
    while(true) delay(1000);  // Infinite loop to prevent operation
  }

  // Display network connection details
  Serial.println("\nWiFi connection successful!");
  Serial.print("Connected to: ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Connect to this IP address in your web browser to control the car.");

  // Start HTTP server
  Serial.println("Starting HTTP server...");
  httpServer.begin();
  Serial.println("HTTP server started on port 80");

  // Configure and start WebSocket server
  Serial.println("Starting WebSocket server...");
  webSocket.begin();
  
  // Set up WebSocket connection callback
  webSocket.onConnection([](net::WebSocket& ws) {
    // Configure message handler for each new connection
    ws.onMessage(handleWebSocketMessage);
    Serial.println("WebSocket client connected");
  });

  Serial.println("WebSocket server started on port 81");
  Serial.println("System initialization complete!");
  Serial.println("====================================");
}

/**
 * @brief Main program loop that runs continuously
 * 
 * This function handles two main tasks:
 * 1. Process incoming HTTP requests to serve the web interface
 * 2. Process WebSocket communications for real-time control
 * 
 * The HTTP server handles new connections, serves the HTML interface,
 * and then closes the connection. The WebSocket server maintains
 * persistent connections for real-time bidirectional communication.
 */
void loop() {
  // Check for incoming HTTP client connections
  WiFiClient httpClient = httpServer.available();
  if (httpClient) {
    Serial.println("[HTTP] New client connection");
    
    if (httpClient.connected()) {
      if (httpClient.available()) {
        // Read the first line of the HTTP request
        String line = httpClient.readStringUntil('\r');
        
        // Skip to end of headers
        httpClient.find("\r\n\r\n");

        Serial.print("<< [HTTP] ");
        Serial.println(line);

        // Serve the web interface for GET requests
        if (line.startsWith("GET /")) {
          // Send HTTP response headers
          httpClient.println("HTTP/1.1 200 OK");
          httpClient.println("Content-Type: text/html");
          httpClient.println("Connection: close");
          httpClient.println();
          
          // Send the HTML interface
          httpClient.println(HTML_CONTENT);
        } else {
          // Respond with Method Not Allowed for non-GET requests
          httpClient.println("HTTP/1.1 405 Method Not Allowed");
          httpClient.println("Connection: close");
          httpClient.println();
        }
      }
    }
    
    // Short delay to allow data transmission
    delay(5);
    
    // Close the connection
    httpClient.stop();
    Serial.println("[HTTP] Client disconnected");
  }

  // Process WebSocket communications
  // This handles incoming connections and messages
  webSocket.listen();
}

/**
 * @brief Makes the RC car move forward
 * 
 * Sets the motor driver pins to move both motors forward simultaneously.
 * The specific pin states are based on the physical configuration of
 * the motors on the chassis.
 * 
 * @details
 * Left Motor (Motor A):  IN1=HIGH, IN2=LOW
 * Right Motor (Motor B): IN3=LOW, IN4=HIGH
 * 
 * The seemingly opposite logic between the two motors is due to their
 * physical mounting orientation on the chassis. These pin states ensure
 * both motors rotate in the direction that propels the car forward.
 */
void CAR_moveForward() {
  digitalWrite(IN1, HIGH);  // Left motor forward
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);   // Right motor forward
  digitalWrite(IN4, HIGH);
}

/**
 * @brief Makes the RC car move backward
 * 
 * Sets the motor driver pins to move both motors backward simultaneously.
 * The specific pin states are based on the physical configuration of
 * the motors on the chassis.
 * 
 * @details
 * Left Motor (Motor A):  IN1=LOW, IN2=HIGH
 * Right Motor (Motor B): IN3=HIGH, IN4=LOW
 * 
 * These pin states are the opposite of those used in CAR_moveForward(),
 * causing both motors to rotate in the reverse direction.
 */
void CAR_moveBackward() {
  digitalWrite(IN1, LOW);   // Left motor backward
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);  // Right motor backward
  digitalWrite(IN4, LOW);
}

/**
 * @brief Makes the RC car turn left (counter-clockwise)
 * 
 * Implements a differential drive turning mechanism by rotating
 * the right motor forward while the left motor rotates backward.
 * This creates a counter-clockwise rotation around the car's center.
 * 
 * @details
 * Left Motor (Motor A):  IN1=LOW, IN2=HIGH (backward)
 * Right Motor (Motor B): IN3=LOW, IN4=HIGH (forward)
 * 
 * This creates a "tank turn" or "pivot turn" where the car rotates
 * around its center axis rather than following a curved path.
 */
void CAR_turnLeft() {
  digitalWrite(IN1, LOW);   // Left motor backward
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);   // Right motor forward
  digitalWrite(IN4, HIGH);
}

/**
 * @brief Makes the RC car turn right (clockwise)
 * 
 * Implements a differential drive turning mechanism by rotating
 * the left motor forward while the right motor rotates backward.
 * This creates a clockwise rotation around the car's center.
 * 
 * @details
 * Left Motor (Motor A):  IN1=HIGH, IN2=LOW (forward)
 * Right Motor (Motor B): IN3=HIGH, IN4=LOW (backward)
 * 
 * This creates a "tank turn" or "pivot turn" where the car rotates
 * around its center axis rather than following a curved path.
 */
void CAR_turnRight() {
  digitalWrite(IN1, HIGH);  // Left motor forward
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);  // Right motor backward
  digitalWrite(IN4, LOW);
}

/**
 * @brief Stops all motors of the RC car
 * 
 * Sets all motor driver input pins to LOW, which effectively
 * applies an electronic brake to the motors for quick stopping.
 * 
 * @details
 * When all input pins (IN1-IN4) are set to LOW, the motor terminals
 * are effectively shorted together, creating resistance to motion
 * and quickly bringing the motors to a stop.
 * 
 * This is different from simply cutting power, as it actively
 * resists motion rather than allowing the motors to coast to a stop.
 */
void CAR_stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
