/**
 * @file    mini_tv_master.ino
 * @brief   Retrieves time and weather data from external APIs and displays it.
 * @author  Ignatius de Loyola Dominique Japar
 * @date    2025-02-26
 * @version 1.0
 */

#include <WiFi.h>        // For WiFi connectivity
#include <HTTPClient.h>  // For making HTTP requests
#include <ArduinoJson.h> // For parsing JSON data

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI  19
#define OLED_CLK   18
#define OLED_DC    4
#define OLED_CS    5
#define OLED_RESET 2
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// --- Wi-Fi Configuration ---
const char* ssid = "Iloe";
const char* password = "photoshop";

/**
 * @brief Reconnects to the Wi-Fi network if the connection is lost.
 *
 * This function attempts to reconnect to the Wi-Fi network using the stored
 * SSID and password. It retries several times before giving up.
 */
void reconnectWiFi() {
  Serial.println("Attempting to reconnect to WiFi...");
  WiFi.disconnect(); // Disconnect first to ensure a clean start
  WiFi.begin(ssid, password);

  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED && retryCount < 10) { // Quickly try to reconnect
    delay(1000); // Wait for a second
    Serial.print(".");
    retryCount++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi reconnected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi reconnection failed.");
    // Consider adding a more drastic action here, like restarting the ESP32
    // ESP.restart();
  }
}

/**
 * @brief Makes an HTTP GET request to the specified URL and returns the parsed JSON document.
 *
 * @param url The URL to make the GET request to.
 * @return A pointer to the parsed DynamicJsonDocument, or nullptr if the request failed.
 *         The caller is responsible for deleting the returned document to free memory.
 */
DynamicJsonDocument* getRequest(const char* url) {
  Serial.print("Fetching JSON from: ");
  Serial.println(url);

  for (int i = 0; i < 4; ++i) {
    // Check Wi-Fi connection and reconnect if necessary
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi Disconnected. Attempting to reconnect...");
      reconnectWiFi(); // Call the reconnection function

      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Reconnection failed.  Retrying...");
        delay(1000); // Wait before retrying the request
        continue; // Retry the outer loop (getRequest attempt)
      }
    }

    HTTPClient http;
    http.begin(url);
  
    int httpResponseCode = http.GET();
  
    if (httpResponseCode <= 0) {
      Serial.print("HTTP Error code: ");
      Serial.println(httpResponseCode);
      http.end();
      delay(1000); // Wait before retrying
      continue;
    }
  
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
  
    if (httpResponseCode != HTTP_CODE_OK) {
      Serial.println("HTTP request failed");
      http.end();
      delay(1000); // Wait before retrying
      continue;
    }
  
    String payload = http.getString();
    Serial.print("Payload:");
    Serial.println(payload);
  
    DynamicJsonDocument* doc = new DynamicJsonDocument(8192); // Allocate on heap
    DeserializationError error = deserializeJson(*doc, payload);
  
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      delete doc; // Clean up memory on failure
      http.end();
      delay(1000); // Wait before retrying
      continue;
    }

    http.end();
    return doc; // Return the pointer to the JSON document
  }

   // If we reach here, all retries failed
  Serial.println("getRequest failed after multiple retries.");
  return nullptr;
}

/**
 * @brief Class for retrieving and managing time data from an external API.
 *
 * Note: This class uses an API for time for education purposes. 
 *       Using an API for time is not recommended for production systems.
 *       NTP (Network Time Protocol) is the preferred choice for networked devices.
 */
class TimeAPI {
public:
  /**
   * @brief Updates the current time from the external API.
   */
  void updateTime();

  // Read-only return functions
  int hour() const { return hour_; }
  int minute() const { return minute_; }
  int second() const { return second_; }

private:
  const char* url = "https://timeapi.io/api/time/current/zone?timeZone=Asia/Hong_Kong"; // API endpoint for time data
  const long syncInterval = 3600000; // Time synchronization interval (1 hour in milliseconds)

  unsigned long lastSyncTime = 0; // Time of the last successful synchronization
  int syncHour, syncMinute, syncSecond; // Time components from the last sync
  
  int hour_, minute_, second_; // Current time components

  /**
   * @brief Synchronizes the time with the external API.
   */
  void syncTime();
};

/**
 * @brief Updates the current time based on the last sync and the time elapsed since then.
 */
void TimeAPI::updateTime() {
  if (lastSyncTime == 0 || millis() - lastSyncTime > syncInterval) {
    syncTime();
  }

  int timeSinceSync = (millis() - lastSyncTime) / 1000; // Time elapsed since the last sync (in seconds)
  second_ = (syncSecond + timeSinceSync) % 60;
  minute_ = (syncMinute + timeSinceSync / 60) % 60;
  hour_   = (syncHour + timeSinceSync / 3600) % 24;
}

/**
 * @brief Synchronizes the time with the external API.
 */
void TimeAPI::syncTime() {
  Serial.println("Syncing time");
  DynamicJsonDocument* doc = getRequest(url);

  if (doc) {
    lastSyncTime = millis();

    // Extract time information from the JSON document
    String dateTime = (*doc)["dateTime"].as<String>();

    syncHour = dateTime.substring(11, 13).toInt();
    syncMinute = dateTime.substring(14, 16).toInt();
    syncSecond = dateTime.substring(17, 19).toInt();

    delete doc; // Free the memory allocated for the JSON document
  } else {
    Serial.println("Time sync failed.");
  }
}

/**
 * @brief Structure to hold current weather data.
 */
struct CurrentData {
  String location;
  String condition;
  int temp;
  int precip; 
};

/**
 * @brief Structure to hold forecast weather data for a single day.
 */
struct ForecastData {
  String date;
  int maxTemp;
  int minTemp;
  int riskOfRain;
  int avgHumidity;
};

/**
 * @brief Class for retrieving and managing weather data from an external API.
 */
class WeatherAPI {
public:
/**
   * @brief Updates the current weather data from the external API.
   */
  void updateCurrentData();

  /**
   * @brief Updates the forecast weather data from the external API.
   */
  void updateForecastData();

  // Read-only return functions
  CurrentData cur() const { return cur_; }
  const ForecastData* fc() const { return fc_; }

private:
  const char* url = "http://api.weatherapi.com/v1"; // Base URL for the weather API
  const char* apiKey = "8992f2fafe4c4f7384b80225252502"; // API key for accessing the weather API
  const char* pos = "22.283614,114.134549"; // Latitude and longitude for the weather 
  const int forecastDays = 3; // Number of forecast days to retrieve 

  unsigned long lastSyncTime = 0; // Time of the last successful synchronization

  CurrentData cur_;           // Current weather data
  ForecastData fc_[3];        // Forecast weather data for multiple days
};

/**
 * @brief Updates the current weather data from the external API.
 */
void WeatherAPI::updateCurrentData() {
  Serial.println("Getting current weather data");
  String reqUrl = String(url) + "/current.json?key=" + apiKey + "&q=" + pos;
  DynamicJsonDocument* doc = getRequest(reqUrl.c_str());

  if (doc) {
    // Extract current weather information from the JSON document
    cur_.location = (*doc)["location"]["name"].as<String>();
    cur_.temp = (*doc)["current"]["temp_c"].as<int>();
    cur_.condition = (*doc)["current"]["condition"]["text"].as<String>();
    cur_.precip = (*doc)["current"]["precip_mm"].as<int>();

    delete doc; // Free the memory allocated for the JSON document
  } else {
    Serial.println("Current weather data extraction failed.");
  }
}

/**
 * @brief Updates the forecast weather data from the external API.
 */
void WeatherAPI::updateForecastData() {
  Serial.println("Getting forecast weather data");
  String reqUrl = String(url) + "/forecast.json?key=" + apiKey 
                  + "&q=" + pos + "&days=" + forecastDays;
  DynamicJsonDocument* doc = getRequest(reqUrl.c_str());

  if (doc) {
    // Extract forecast weather information from the JSON document
    for (int i=0; i<forecastDays; i++) {
      fc_[i].date = (*doc)["forecast"]["forecastday"][i]["date"].as<String>();
      fc_[i].maxTemp = (*doc)["forecast"]["forecastday"][i]["day"]["maxtemp_c"].as<int>();
      fc_[i].minTemp = (*doc)["forecast"]["forecastday"][i]["day"]["mintemp_c"].as<int>();
      fc_[i].riskOfRain = (*doc)["forecast"]["forecastday"][i]["day"]["daily_chance_of_rain"].as<int>();
      fc_[i].avgHumidity = (*doc)["forecast"]["forecastday"][i]["day"]["avghumidity"].as<int>();
    }
    
    delete doc; // Free the memory allocated for the JSON document
  } else {
    Serial.println("Forecast weather data extraction failed.");
  }
}

// --- Button and Mode Configuration ---
const int buttonPin = 26; // Digital pin connected to the button (replace with your actual pin)
enum Mode {
  TIME_MODE,            // Display time
  CURRENT_WEATHER_MODE, // Display current weather
  FORECAST_WEATHER_MODE // Display forecast weather
};

// --- Global Variables ---
Mode currentMode = TIME_MODE;           // Currently selected mode
unsigned long lastButtonPressTime = 0;  // Time of the last button press
const long buttonDebounceDelay = 50;    // Debounce delay in milliseconds
bool justChangedMode = true;            // For forcing update upon mode change
unsigned long currentTime;              // Current time (millis())

// --- Object Instantiation ---
TimeAPI timeAPI;       // Instance of the TimeAPI class
WeatherAPI weatherAPI; // Instance of the WeatherAPI class

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to Wi-Fi. IP address: ");
  Serial.println(WiFi.localIP());

  // Set button pin as input
  pinMode(buttonPin, INPUT);
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    currentTime = millis();
    if (currentTime - lastButtonPressTime > buttonDebounceDelay) {
      // Debounced button press
      lastButtonPressTime = currentTime;

      // Change mode
      currentMode = static_cast<Mode>((currentMode + 1) % 3); // Cycle through modes
      Serial.print("Switching to mode: ");
      Serial.println(currentMode);
    }
  }

  currentTime = millis(); // Get current time for non-blocking delays

  // Execute code based on the current mode
  switch (currentMode) {
    case TIME_MODE:
      static unsigned long lastTimeUpdate = 0;
      if (currentTime - lastTimeUpdate >= 50) { // Update every 50ms
        timeAPI.updateTime();
        Serial.print(timeAPI.hour());
        Serial.print(":");
        Serial.print(timeAPI.minute());
        Serial.print(":");
        Serial.println(timeAPI.second());

        display.clearDisplay();
        display.setTextSize(2);      // Normal 1:1 pixel scale
        display.setTextColor(SSD1306_WHITE); // Draw white text
        display.setCursor(1, 2);     // Start at top-left corner
        display.cp437(true);         // Use full 256 char 'Code Page 437' font
        display.write("hello my name is joe biden hehehaha");
        display.display();

        lastTimeUpdate = currentTime;
      }
      break;

    case CURRENT_WEATHER_MODE:
      static unsigned long lastCurrentWeatherUpdate = 0;
      if (currentTime - lastCurrentWeatherUpdate >= 10000 || justChangedMode) { // Update every 10 seconds
        justChangedMode = false;
        weatherAPI.updateCurrentData();
        CurrentData cur = weatherAPI.cur();
        Serial.println(cur.location);
        Serial.println(cur.condition);
        Serial.print(cur.temp);
        Serial.println("degC ");
        Serial.print(cur.precip);
        Serial.println("mm");
        lastCurrentWeatherUpdate = currentTime;
      }
      break;

    case FORECAST_WEATHER_MODE:
      static unsigned long lastForecastWeatherUpdate = 0;
      if (currentTime - lastForecastWeatherUpdate >= 60000 || justChangedMode) { // Update every 60 seconds
        justChangedMode = false;
        weatherAPI.updateForecastData();
        const ForecastData* fc = weatherAPI.fc();
        for (int i = 0; i < 3; i++) {
          Serial.print(fc[i].date);
          Serial.print(": ");
          Serial.print(fc[i].maxTemp);
          Serial.print("degC ");
          Serial.print(fc[i].minTemp);
          Serial.print("degC ");
          Serial.print(fc[i].riskOfRain);
          Serial.print("% ");
          Serial.print(fc[i].avgHumidity);
          Serial.println("%");
        }
        lastForecastWeatherUpdate = currentTime;
      }
      break;
  }
}
