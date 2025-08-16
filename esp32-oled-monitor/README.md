# ESP32 OLED Server Stats

This project displays server statistics on a small OLED screen using an ESP32. It fetches JSON data from a FastAPI server and shows CPU, RAM, and network usage in real time.

## Features
- Connects to Wi-Fi
- Fetches server stats via HTTP GET request
- Displays CPU usage, RAM usage, and network throughput on a 0.96" I2C OLED
- Uses ArduinoJson for parsing JSON responses

## Hardware Required
- ESP32 development board
- 0.96" OLED display (I2C)
- Jumper wires

## Wiring
| OLED Pin | ESP32 Pin |
|-----------|-----------|
| VCC       | 3.3V      |
| GND       | GND       |
| SDA       | GPIO 21   |
| SCL       | GPIO 22   |

> Note: Adjust GPIO pins if your ESP32 board uses different defaults.

## Software Required
- Arduino IDE
- ESP32 board support installed
- ArduinoJson library
- Adafruit SSD1306 library
- Adafruit GFX library

## Installation
1. Connect the OLED to the ESP32 as per the wiring table.
2. Install required Arduino libraries via Library Manager.
3. Open `05_Simple_Knob.ino` in Arduino IDE.
4. Set your Wi-Fi SSID and password in the code:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
