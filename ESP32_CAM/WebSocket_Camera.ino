// Using OV2640 2MP camera module (1/4" CMOS, UXGA 1600x1200, supports JPEG, YUV, RGB output formats)

/**
 * @file ESP32_CAM_WebSocket_Stream.ino
 * @brief Real-time video streaming from ESP32-CAM over WebSocket
 * 
 * This program initializes an ESP32-CAM module, connects to WiFi, and streams
 * video frames to clients connected via WebSocket. It allows remote quality
 * adjustment through JSON commands and optimizes streaming performance.
 * 
 * @note Uses a custom WebSocketServer implementation from the net namespace
 * @date April 17, 2025
 */

#include "esp_camera.h"
#include <WiFi.h>
#include "WebSocketServer.h" // Using quotes for local header
#include <ArduinoJson.h>

/**
 * @brief GPIO pin definitions for AI-THINKER ESP32-CAM module
 * 
 * These pin mappings connect the ESP32 microcontroller to the OV2640
 * camera module according to the AI-THINKER ESP32-CAM reference design.
 */
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

/**
 * @brief WiFi network credentials
 * 
 * SSID and password for connecting to the local WiFi network.
 * The ESP32-CAM will be accessible on this network via its IP address.
 */
const char* ssid = " ";
const char* password = " ";

/**
 * @brief WebSocket server instance on port 82
 * 
 * Creates a WebSocket server using the net namespace for handling
 * client connections and streaming video data.
 */
net::WebSocketServer webSocket(82);

/**
 * @brief Camera configuration constants
 * 
 * FRAME_SIZE: Resolution of the camera stream (VGA = 640x480)
 * JPEG_QUALITY: Compression quality (0-63, lower number = higher quality)
 */
#define FRAME_SIZE FRAMESIZE_VGA
#define JPEG_QUALITY 10  // 0-63, lower is higher quality

/**
 * @brief Initialize the ESP32 camera with appropriate settings
 * 
 * Configures camera parameters including pins, clock frequency, frame size,
 * and quality settings. Settings are optimized based on available PSRAM.
 * 
 * @return bool True if camera initialized successfully, False otherwise
 */
bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Initialize with high specs for buffer
  if (psramFound()) {
    config.frame_size = FRAME_SIZE;
    config.jpeg_quality = JPEG_QUALITY;
    config.fb_count = 2;
  } else {
    Serial.println("PSRAM not found. Camera may not initialize properly.");
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return false;
  }
  
  // Set additional camera parameters for better streaming
  sensor_t * s = esp_camera_sensor_get();
  s->set_brightness(s, 1);
  s->set_contrast(s, 1);
  
  return true;
}

/**
 * @brief Tracks number of connected WebSocket clients
 * 
 * Used to determine when camera frames should be captured and sent,
 * preventing unnecessary processing when no clients are connected.
 */
int numClients = 0;

/**
 * @brief Process text commands received from WebSocket clients
 * 
 * Parses JSON messages from clients and handles any supported commands.
 * Currently implements quality adjustment via the "set_quality" parameter.
 * 
 * @param client Reference to the WebSocket client that sent the command
 * @param message Pointer to the text message content
 * @param length Length of the message in bytes
 */
void handleTextCommand(net::WebSocket& client, const char* message, uint16_t length) {
  String msg = String(message);
  DynamicJsonDocument doc(200);
  DeserializationError error = deserializeJson(doc, msg);
  
  if (error) {
    Serial.println("Failed to parse JSON command");
    return;
  }
  
  // Check if it's a quality change command
  if (doc.containsKey("set_quality")) {
    int quality = doc["set_quality"];
    quality = constrain(quality, 0, 63);
    
    sensor_t * s = esp_camera_sensor_get();
    s->set_quality(s, quality);
    
    Serial.printf("Quality set to %d\n", quality);
  }
}

/**
 * @brief Handle incoming WebSocket messages
 * 
 * Callback function for WebSocket message events. Routes text messages
 * to the appropriate command handler based on data type.
 * 
 * @param client Reference to the client that sent the message
 * @param dataType Type of the message (TEXT or BINARY)
 * @param message Pointer to the message content
 * @param length Length of the message in bytes
 */
void handleWebSocketMessage(net::WebSocket& client, net::WebSocket::DataType dataType, const char* message, uint16_t length) {
  if (dataType == net::WebSocket::DataType::TEXT) {
    // Handle text commands
    handleTextCommand(client, message, length);
  }
}

/**
 * @brief Handle new WebSocket client connections
 * 
 * Called when a new client connects to the WebSocket server.
 * Increments client counter, sets up message handlers, and sends
 * initial camera configuration information to the client.
 * 
 * @param client Reference to the newly connected WebSocket client
 */
void handleClientConnection(net::WebSocket& client) {
  numClients++;
  Serial.println("Client connected");
  Serial.printf("Number of clients: %d\n", numClients);
  
  // Set up message handler for this client
  client.onMessage(handleWebSocketMessage);
  
  // Send camera information to client
  DynamicJsonDocument doc(200);
  doc["type"] = "camera_info";
  doc["framesize"] = FRAME_SIZE;
  doc["quality"] = JPEG_QUALITY;
  
  String json;
  serializeJson(doc, json);
  client.send(net::WebSocket::DataType::TEXT, json.c_str(), json.length());
}

/**
 * @brief Capture and broadcast camera frames to all clients
 * 
 * Captures a frame from the camera and sends it to all connected
 * WebSocket clients as binary data. Skips processing if no clients
 * are connected to save resources.
 */
void sendCameraFrames() {
  if (numClients == 0) return; // Don't capture if nobody is connected
  
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  
  // Send the frame data to all connected clients - CHANGED TO MATCH net::WebSocket API
  webSocket.broadcast(net::WebSocket::DataType::BINARY, (const char*)fb->buf, fb->len);
  
  esp_camera_fb_return(fb);
}

/**
 * @brief Arduino setup function - runs once at startup
 * 
 * Initializes hardware components, connects to WiFi network,
 * starts the WebSocket server, and configures camera settings
 * for optimal streaming performance.
 */
void setup() {
  Serial.begin(115200);
  Serial.println();
  
  // Initialize camera
  if (!initCamera()) {
    Serial.println("Camera initialization failed!");
    while(1) delay(100);
  }
  // Add before WiFi.begin():
  /*IPAddress staticIP(192, 168, 242, 149); // Same as RC car
  IPAddress gateway(192, 168, 242, 1);    // Router IP
  IPAddress subnet(255, 255, 255, 0);     // Subnet mask
  WiFi.config(staticIP, gateway, subnet);*/

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Camera Stream Ready! Connect to: http://");
  Serial.println(WiFi.localIP());
  
  // Start WebSocket server - REVISED TO USE net::WebSocket API
  webSocket.begin();
  
  // Track connections with onConnection handler
  webSocket.onConnection(handleClientConnection);
  
  Serial.println("WebSocket server started on port 82");

  WiFi.setSleep(false); // Disable WiFi sleep mode for lower latency

  // Optimize camera settings
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, FRAME_SIZE);
  s->set_quality(s, JPEG_QUALITY);
  s->set_hmirror(s, 1); // Optional: mirror horizontally
  s->set_brightness(s, 1); // Increase brightness slightly
}

/**
 * @brief Frame rate control variables
 * 
 * lastFrameTime: Timestamp of the last frame capture
 * frameInterval: Target time between frames in milliseconds (50ms = 20 FPS)
 */
unsigned long lastFrameTime = 0;
const int frameInterval = 50; // ~20 frames per second

/**
 * @brief Arduino main loop function - runs repeatedly
 * 
 * Handles incoming WebSocket connections and messages,
 * and captures/sends camera frames at the specified interval
 * to maintain consistent frame rate.
 */
void loop() {
  // CHANGED webSocket.loop() TO MATCH net::WebSocket API
  webSocket.listen();
  
  unsigned long currentMillis = millis();
  if (currentMillis - lastFrameTime > frameInterval) {
    lastFrameTime = currentMillis;
    sendCameraFrames();
  }
}

  
