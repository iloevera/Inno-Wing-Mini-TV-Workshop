/**
 * @file    mini_tv_master.ino
 * @brief   Connecting to WiFi + Make API Calls (showing temperature and humidity)
 * @author  Aom, Ilo
 * @date    2025-09-12
 * @version 1.0
 */

// Required Libraries: Adafruit GFX, Adafruit ST7735, TFT, Arduino JSON

#include <TFT_eSPI.h> // Drawing text, images, and shapes on the TFT display
#include <SPI.h>      // Serial Data Protocol

#include <WiFi.h>        // WiFi connectivity
#include <HTTPClient.h>  // HTTP requests
#include <ArduinoJson.h> // JSON parsing

// --- TFT Configuration ---
TFT_eSPI tft = TFT_eSPI();
#define TFT_LED 25

// --- WiFi Configuration ---
#define WIFI_SSID "Wi-Fi.HK via HKU"

// --- Display Parameters ---
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320
const int FONT_SIZE = 6;

// --- Function Declaration ---
void displayText(const char *text, int textSize, int x, int y);
void APIGetRequest(const char *url, DynamicJsonDocument &doc);
void fetchWeatherData();

void setup()
{
  //  Screen Setup
  tft.init();
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH); // turn on LED
  tft.setRotation(1); // Landscape
  Serial.begin(115200);
  tft.fillScreen(TFT_BLACK);

  //  WiFi Setup
  WiFi.begin(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting to WiFi...");
    displayText("Connecting to WiFi...", 2, 10, 50);
    delay(500);
  }

  displayText("Let's begin...", 4, 50, 50);
  delay(2000);
}

void loop()
{
  // Fetch and display weather data
  fetchWeatherData();
}

void displayText(const char *text, int textSize = 1, int x = 0, int y = 0)
{
  tft.fillScreen(TFT_BLACK);              // clear screen with black background
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // set text color
  tft.drawString(text, x, y, textSize);   // display text
}

void APIGetRequest(const char *url, DynamicJsonDocument &doc)
{
  HTTPClient http;
  http.begin(url);
  int httpResponseCode = http.GET();

  if (httpResponseCode != HTTP_CODE_OK)
  {
    displayText("Error Calling API!", 4, 50, 50);
    delay(5000);
    return;
  }

  String payload = http.getString();
  deserializeJson(doc, payload); // Parse the payload into a JSON document
}

void fetchWeatherData()
{
  DynamicJsonDocument doc(8192);
  Serial.println("Fetching weather...");
  const char *weatherAPI = "https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=rhrread";

  APIGetRequest(weatherAPI, doc);

  int temperature = doc["temperature"]["data"][0]["value"].as<int>();
  int humidity = doc["humidity"]["data"][0]["value"].as<int>();

  char msg[16];
  snprintf(msg, sizeof(msg), "%d c %dp", temperature, humidity);
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" Humidity: ");
  Serial.println(humidity);

  int textWidth = tft.textWidth(msg, FONT_SIZE);
  int textHeight = tft.fontHeight(FONT_SIZE);
  
  displayText(msg, FONT_SIZE, ( SCREEN_WIDTH - textWidth ) / 2 , ( SCREEN_HEIGHT - textHeight ) / 2);
  delay(10000);
}
