# 🚗 ESP32 WiFi RC Car Controller

![Dashboard](https://raw.githubusercontent.com/ayushommishra/ESP32_WebSocket_RC_Car/main/Dashboard.png)

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Arduino](https://img.shields.io/badge/Arduino-ESP32-green.svg)](https://www.arduino.cc/)
[![WebSockets](https://img.shields.io/badge/Interface-WebSockets-orange.svg)](https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API)

A comprehensive solution for wirelessly controlling an RC car through WiFi using an ESP32 microcontroller. Control your RC car in real-time from any device with a web browser!

## ✨ Features

- **Web-based Control Interface**: Modern, responsive UI accessible from any device
- **Real-time Control**: Low-latency WebSocket communication for immediate responsiveness
- **Multiple Control Methods**:
  - On-screen touch controls
  - Keyboard input (WASD/Arrow keys)
  - Floating control panel for mobile devices
- **Status Indicators**: Real-time connection status and feedback
- **No App Required**: Works in any modern web browser

## 🛠️ Hardware Requirements

- ESP32 Development Board
- L298N Motor Driver Module
- 4x DC Motors
- RC Car Chassis
- 7.4V-12V Battery Pack
- Jumper Wires

## 📋 Pin Connections

| ESP32  | L298N Motor Driver    |
| ------ | --------------------- |
| GPIO13 | ENA (Enable Motor A)  |
| GPIO15 | IN1 (Input 1 Motor A) |
| GPIO14 | IN2 (Input 2 Motor A) |
| GPIO27 | ENB (Enable Motor B)  |
| GPIO26 | IN3 (Input 3 Motor B) |
| GPIO25 | IN4 (Input 4 Motor B) |

## 📊 System Architecture

The system consists of two main server endpoints:

1. **HTTP Server (Port 80)**: Serves the web control interface
2. **WebSocket Server (Port 81)**: Handles real-time bidirectional communication

## 🚀 Getting Started

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software)
- [ESP32 Board Package for Arduino IDE](https://github.com/espressif/arduino-esp32)

### Installation Steps

1. **Clone this repository**

   ```
   https://github.com/Sensor-Driven-Automation/ESP32_WebSocket_RC_Car.git
   ```

2. **Open the project in Arduino IDE**

   - Open `wifi_car_controller.ino` in Arduino IDE

3. **Configure WiFi settings**

   - Update the SSID and password in the code:

   ```cpp
   const char *ssid = "YOUR_WIFI_SSID";
   const char *password = "YOUR_WIFI_PASSWORD";
   ```

4. **Upload to ESP32**

   - Select your ESP32 board from Tools > Board
   - Select the correct port from Tools > Port
   - Click the Upload button

5. **Find ESP32's IP Address**

   - Open Serial Monitor (9600 baud)
   - Look for the IP address in the startup messages

6. **Connect and Control**
   - Open a web browser on any device connected to the same WiFi network
   - Navigate to the ESP32's IP address
   - Use the web interface to control the RC car

## 🎮 Control Commands

| Command | Description | Button | Keyboard        |
| ------- | ----------- | ------ | --------------- |
| 0       | Stop        | ⏹️     | Space           |
| 1       | Forward     | ⬆️     | W / Up Arrow    |
| 2       | Backward    | ⬇️     | S / Down Arrow  |
| 4       | Left Turn   | ⬅️     | A / Left Arrow  |
| 8       | Right Turn  | ➡️     | D / Right Arrow |

## 🧩 Project Structure

- `wifi_car_controller.ino`: Main code file with ESP32 implementation
- `WebSocketServer.h`: Custom WebSocket server implementation
- `index.h`: Web interface HTML content
- `/docs`: Additional documentation
- `/schematics`: Circuit diagrams

## 🌟 Web Interface

The web interface provides:

- Real-time connection status
- Multiple control methods (touch, keyboard)
- Visually pleasing design with particle effects
- Mobile-friendly responsive layout
- Floating control panel for easier mobile control

## 🛠️ Customization

### Adjusting Motor Speed

You can implement PWM control for variable speed:

```cpp
// In setup()
ledcSetup(0, 5000, 8);  // Channel 0, 5kHz, 8-bit resolution
ledcSetup(1, 5000, 8);  // Channel 1, 5kHz, 8-bit resolution
ledcAttachPin(ENA, 0);  // Attach ENA to channel 0
ledcAttachPin(ENB, 1);  // Attach ENB to channel 1

// Instead of digitalWrite(ENA, HIGH):
ledcWrite(0, speed);  // speed: 0-255
```

### Adding Features

- Battery level monitoring
- Headlights control
- Speed adjustment
- Custom movement patterns

## 📱 Browser Compatibility

Tested and working on:

- Chrome (Desktop & Mobile)
- Firefox (Desktop & Mobile)
- Safari (Desktop & Mobile)
- Edge

## 👥 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📜 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🙏 Acknowledgments

- [Ayush Verma](https://github.com/ayushverma17) and [Ayush Mishra](https://github.com/ayushommishra) - Original authors
- Arduino and ESP32 Communities
- [Font Awesome](https://fontawesome.com/) for UI icons

## 📞 Contact

For questions or support, please [open an issue](https://github.com/ayushommishra/esp32-wifi-rc-car/issues) on GitHub.

---

Made with ❤️ by [Ayush Verma](https://github.com/ayushverma17) and [Ayush Mishra](https://github.com/ayushommishra)
