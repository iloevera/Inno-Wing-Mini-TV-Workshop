/**
 * @file    mini_tv_master.ino
 * @brief   Switch pages between temperature and humidity
 * @author  Ilo
 * @date    2025-09-12
 * @version 1.0
 */

#include <TFT_eSPI.h> // Drawing text, images, and shapes on the TFT display
#include <SPI.h>      // Serial Data Protocol

#include <WiFi.h>           // WiFi connectivity
#include <HTTPClient.h>     // HTTP requests
#include <ArduinoJson.h>    // JSON parsing  
#include <ESP32Encoder.h>   // Rotary encoder
#include "esp_sleep.h"

// --- TFT Configuration ---
TFT_eSPI tft = TFT_eSPI();
#define TFT_LED 25

// --- Rotary encoder pins ---
#define ROTARY_PINA 12
#define ROTARY_PINB 14
#define ROTARY_PINSW 13

// --- Wi-Fi credentials ---
#define WIFI_SSID "Wi-Fi.HK via HKU"

// --- API endpoint (for weather data request) ---
const char* serverName = "https://data.weather.gov.hk/weatherAPI/opendata/weather.php?lang=en&dataType=rhrread";

ESP32Encoder encoder;
volatile bool buttonPressed = false;
unsigned long lastButtonTime = 0;

// Data arrays
struct RainfallData { String place; float max; };
struct TempData { String place; float value; };

RainfallData rainfall[32];
TempData temperature[32];
int rainfallCount = 0;
int tempCount = 0;

// Mode: 0 = rainfall, 1 = temperature
volatile int mode = 0;

// Independent indices
int rainIndex = 0;
int tempIndex = 0;

int rainBase = 0;
int tempBase = 0;

// Button ISR
void IRAM_ATTR isrButton() {
  unsigned long now = millis();
  if (now - lastButtonTime > 200) {   // 200 ms debounce
    buttonPressed = true;
    lastButtonTime = now;
  }
}

// WiFi + API
void connectWiFi() {
  WiFi.begin(WIFI_SSID);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
}

void APIGetRequest(const char *url, DynamicJsonDocument &doc)
{
  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();

  if (httpResponseCode != HTTP_CODE_OK)
  {
    Serial.println("Error Calling API!");
    delay(5000);
    return;
  }

  String payload = http.getString();
  deserializeJson(doc, payload); // Parse the payload into a JSON document
  http.end();
}

void fetchWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    DynamicJsonDocument doc(20 * 1024);

    APIGetRequest(serverName, doc);

    // Rainfall
    rainfallCount = 0;
    for (JsonObject item : doc["rainfall"]["data"].as<JsonArray>()) {
      rainfall[rainfallCount].place = item["place"].as<String>();
      rainfall[rainfallCount].max = item["max"].as<float>();
      rainfallCount++;
    }

    // Temperature
    tempCount = 0;
    for (JsonObject item : doc["temperature"]["data"].as<JsonArray>()) {
      temperature[tempCount].place = item["place"].as<String>();
      temperature[tempCount].value = item["value"].as<float>();
      tempCount++;
    }
  }
}

// Display
void drawUI() {
  // Top bar
  tft.fillRect(0, 0, tft.width(), tft.height()/3, TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(0, 30);
  tft.setTextSize(3);
  if (mode == 0) tft.print(" > Rainfall (mm)\n   Temperature (C)");
  else tft.print("   Rainfall (mm)\n > Temperature (C)");

  // Side bar
  if (mode == 0 && rainfallCount > 0) {
    int index = (rainIndex % rainfallCount + rainfallCount) % rainfallCount;

    tft.fillRect(0, tft.height()/3, tft.width()/2, tft.height()*2/3, TFT_DARKGREY);
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.setCursor(0, 130);

    tft.setTextSize(1);
    for (int i=0; i<10; i++) {
      tft.print(" ");
      tft.println(rainfall[(index - 10 + i + rainfallCount) % rainfallCount].place);
    }

    tft.setTextSize(2);
    tft.print("> ");
    tft.println(rainfall[index].place);

    tft.setTextSize(1);
    for (int i=0; i<10; i++) {
      tft.print(" ");
      tft.println(rainfall[(index + i + 1 + rainfallCount) % rainfallCount].place);
    }

    // Big number
    tft.fillRect(tft.width()/2, tft.height()/3, tft.width()/2, tft.height()*2/3, TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(5);
    tft.setCursor(tft.width()/2 + 20, tft.height()*2/3 - 56/2);
    tft.print(rainfall[index].max, 1);
    tft.print(" mm");
  }
  else if (mode == 1 && tempCount > 0) {
    int index = (tempIndex % tempCount + tempCount) % tempCount;

    tft.fillRect(0, tft.height()/3, tft.width()/2, tft.height()*2/3, TFT_DARKGREY);
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.setCursor(0, 130);

    tft.setTextSize(1);
    for (int i=0; i<10; i++) {
      tft.print(" ");
      tft.println(temperature[(index - 10 + i + tempCount) % tempCount].place);
    }

    tft.setTextSize(2);
    tft.print("> ");
    tft.println(temperature[index].place);

    tft.setTextSize(1);
    for (int i=0; i<10; i++) {
      tft.print(" ");
      tft.println(temperature[(index + i + 1 + tempCount) % tempCount].place);
    }

    // Big number
    tft.fillRect(tft.width()/2, tft.height()/3, tft.width()/2, tft.height()*2/3, TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(5);
    tft.setCursor(tft.width()/2 + 20, tft.height()*2/3 - 56/2);
    tft.print(temperature[index].value, 1);
    tft.print(" C");
  }
}

// Setup & Loop
void setup() {
  Serial.begin(115200);
  tft.init();
  pinMode(TFT_LED, OUTPUT);  
  digitalWrite(TFT_LED, HIGH); // turn on LED
  tft.setRotation(1);

  // Encoder setup
  ESP32Encoder::useInternalWeakPullResistors = puType::up; // enable pullups
  encoder.attachHalfQuad(ROTARY_PINA, ROTARY_PINB); // attach pins
  encoder.setCount(0); // start at 0

  // Button setup
  pinMode(ROTARY_PINSW, INPUT_PULLUP);
  attachInterrupt(ROTARY_PINSW, isrButton, FALLING);

  connectWiFi();
  fetchWeatherData();
  drawUI();
}

void loop() {
  static int lastPos = 0;
  int pos = encoder.getCount();

  if (pos != lastPos) {
    if (mode == 0) {
      rainIndex = pos - rainBase;
    } else {
      tempIndex = pos - tempBase;
    }
    lastPos = pos;
    drawUI();
  }

  if (buttonPressed) {
    buttonPressed = false;

    if (mode == 0) {
      // leaving rainfall mode, remember current encoder count
      rainBase = encoder.getCount() - rainIndex;
    } else {
      // leaving temperature mode
      tempBase = encoder.getCount() - tempIndex;
    }
    
    mode = 1 - mode; // toggle mode
    drawUI();
  }

}
