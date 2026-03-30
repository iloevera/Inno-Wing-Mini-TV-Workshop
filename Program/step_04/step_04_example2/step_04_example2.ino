#include <TFT_eSPI.h> 
#include <SPI.h>      
#include <WiFi.h>           
#include <HTTPClient.h>     
#include <ArduinoJson.h>    
#include <ESP32Encoder.h>   

// --- Hardware Setup ---
TFT_eSPI tft = TFT_eSPI();
ESP32Encoder encoder;
#define TFT_LED 25
#define ROTARY_PINA 12
#define ROTARY_PINB 14
#define BUTTON_PIN 13 // Changed from ROTARY_PINSW to match convention

// --- Wi-Fi & API Setup ---
#define WIFI_SSID "Wi-Fi.HK via HKU"
const char* serverName = "https://data.weather.gov.hk/weatherAPI/opendata/weather.php?lang=en&dataType=rhrread";

// --- Global Variables for Attendees ---
int currentMode = 0; // 0 = Rainfall, 1 = Temperature
int rainIndex = 0;   // Which city we are looking at for rain
int tempIndex = 0;   // Which city we are looking at for temp
long lastCount = 0;

// Debounce Tracking
int lastSteadyState = HIGH;       
int lastFlickerableState = HIGH;  
unsigned long lastDebounceTime = 0;

// --- Internal Data Storage (Do not modify) ---
struct WeatherData { 
  String place; 
  float value; 
};
WeatherData rainfall[32];
WeatherData temperature[32];
int rainfallCount = 0;
int tempCount = 0;


// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  
  // 1. Hardware Init
  pinMode(TFT_LED, OUTPUT);  
  digitalWrite(TFT_LED, HIGH);
  tft.init();
  tft.setRotation(1);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  ESP32Encoder::useInternalWeakPullResistors = puType::up; 
  encoder.attachHalfQuad(ROTARY_PINA, ROTARY_PINB); 
  encoder.setCount(0); 

  // 2. Visual Loading Screen
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 100);
  tft.print("Connecting to WiFi...");

  // 3. Network & Data
  connectWiFi();
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 100);
  tft.print("Fetching Weather Data...");
  fetchWeatherData();
  
  // 4. Start UI
  drawUI(); 
}

// ==========================================
// LOOP: Attendee Sandbox
// ==========================================
void loop() {
  long currentCount = encoder.getCount();
  int buttonReading = digitalRead(BUTTON_PIN);

  // --- ATTENDEE WORKSPACE STARTS HERE ---

  // TASK 1: Handle Scrolling (Encoder)
  if (currentCount != lastCount) {
    
    // 1a. Check which direction we turned
    if (currentCount > lastCount) {
      // Turned Right: Move down the list
      if (currentMode == 0){ 
        rainIndex++; 
      } 
      else { 
        tempIndex++; 
      }
    } else {
      // Turned Left: Move up the list
      if (currentMode == 0) { 
        rainIndex--; 
      } 
      else{ 
        tempIndex--; 
      }
    }

    //Redraw the screen to show the new selection
    drawUI();
    
    lastCount = currentCount; // Save the new position
  }

  // TASK 2: Handle Mode Toggling (Button)
  if (buttonReading != lastFlickerableState) {
    lastDebounceTime = millis();
    lastFlickerableState = buttonReading;
  }

  if ((millis() - lastDebounceTime) > 50) {
    if (buttonReading != lastSteadyState) {
      lastSteadyState = buttonReading;

      // If the button is pressed down
      if (lastSteadyState == LOW) {
        currentMode = 1 - currentMode;
        drawUI();
        // 2a. Toggle the currentMode between 0 and 1
        
        
        // 2b. Redraw the screen to show the new mode

        
        
      }
    }
  }

  // --- ATTENDEE WORKSPACE ENDS HERE ---
  delay(10);
}


// ============================
// BOILERPLATE HELPER FUNCTIONS
// ============================

void connectWiFi() {
  WiFi.begin(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
}

void fetchWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    if (http.GET() == HTTP_CODE_OK) {
      DynamicJsonDocument doc(20 * 1024);
      deserializeJson(doc, http.getString());
      
      rainfallCount = 0;
      for (JsonObject item : doc["rainfall"]["data"].as<JsonArray>()) {
        rainfall[rainfallCount].place = item["place"].as<String>();
        rainfall[rainfallCount].value = item["max"].as<float>();
        rainfallCount++;
      }
      
      tempCount = 0;
      for (JsonObject item : doc["temperature"]["data"].as<JsonArray>()) {
        temperature[tempCount].place = item["place"].as<String>();
        temperature[tempCount].value = item["value"].as<float>();
        tempCount++;
      }
    }
    http.end();
  }
}

void drawUI() {
  // Top bar
  tft.fillRect(0, 0, tft.width(), tft.height()/3, TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.setCursor(0, 30);
  tft.setTextSize(3);
  if (currentMode == 0) tft.print(" > Rainfall (mm)\n   Temperature (C)");
  else                  tft.print("   Rainfall (mm)\n > Temperature (C)");

  // Content Area
  int count = (currentMode == 0) ? rainfallCount : tempCount;
  int rawIndex = (currentMode == 0) ? rainIndex : tempIndex;
  WeatherData* dataArr = (currentMode == 0) ? rainfall : temperature;

  if (count > 0) {
    // Math to wrap the index around the list safely
    int index = (rawIndex % count + count) % count; 

    // Side bar list
    tft.fillRect(0, tft.height()/3, tft.width()/2, tft.height()*2/3, TFT_DARKGREY);
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.setCursor(0, 130);

    tft.setTextSize(1);
    for (int i=0; i<10; i++) {
      tft.print(" "); tft.println(dataArr[(index - 10 + i + count) % count].place);
    }

    tft.setTextSize(2);
    tft.print("> "); tft.println(dataArr[index].place);

    tft.setTextSize(1);
    for (int i=0; i<10; i++) {
      tft.print(" "); tft.println(dataArr[(index + i + 1 + count) % count].place);
    }

    // Big number display
    tft.fillRect(tft.width()/2, tft.height()/3, tft.width()/2, tft.height()*2/3, TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(5);
    tft.setCursor(tft.width()/2 + 20, tft.height()*2/3 - 56/2);
    tft.print(dataArr[index].value, 1);
    tft.print((currentMode == 0) ? " mm" : " °C");
  }
}