#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Wi-Fi credentials
const char* ssid = "WIFI NAME";
const char* password = "WIFI PASSWORD";

// FastAPI server URL
const char* serverURL = "http://YOUR_SERVER_IP:8080/espdata";

// OLED configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    while(true);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.setTextWrap(false);
  display.println("Connecting to WiFi...");
  display.display();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WiFi connected!");
  display.display();
}

void loop() {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    int httpCode = http.GET();

    if(httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);

      // Parse JSON
      StaticJsonDocument<256> doc;
      DeserializationError error = deserializeJson(doc, payload);
      if(!error){
        const char* cpu = doc["cpu_total"];
        const char* ram_used = doc["ram_used"];
        const char* ram_total = doc["ram_total"];
        const char* ram_percent = doc["ram_percent"];
        const char* net_in = doc["net_in"];
        const char* net_out = doc["net_out"];

        // Display on OLED
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("CPU " + String(cpu));
        // display.println("RAM: " + String(ram_used) + "/" + String(ram_total) + " (" + String(ram_percent) + ")");
        display.println("RAM " + String(ram_percent));
        display.println("\x1f " + String(net_in));
        display.println("\x1e " + String(net_out));
        display.display();
      } else {
        Serial.println("JSON parse failed");
      }
    } else {
      Serial.print("Error in HTTP request: ");
      Serial.println(httpCode);
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }

  delay(1000); // update every 1 second
}
