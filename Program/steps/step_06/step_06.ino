/**
 * @file    mini_tv_master.ino
 * @brief   Check if screen and rotary encoder are working
 * @author  Aom, Ilo
 * @date    2025-09-12
 * @version 1.0
 */

// Required Libraries: Adafruit GFX, Adafruit ST7735, TFT, Arduino JSON, ESP32Encoder

#include <TFT_eSPI.h> // Drawing text, images, and shapes on the TFT display
#include <SPI.h>      // Serial Data Protocol

#include <WiFi.h>        // WiFi connectivity
#include <HTTPClient.h>  // HTTP requests
#include <ArduinoJson.h> // JSON parsing
#include <time.h>
#include <ESP32Encoder.h>
#include "esp_sleep.h"

#include "Icons.h"

// --- TFT Configuration ---
TFT_eSPI tft = TFT_eSPI();
#define TFT_LED 25

// --- Rotary Encoder Pins + Configurations ---
#define ROTARY_PINA 12
#define ROTARY_PINB 14
#define ROTARY_PINSW 13

// --- Button Pin ---
#define BUTTON_PIN 33

ESP32Encoder encoder;
volatile bool buttonPressed = false;
unsigned long lastButtonTime = 0;
unsigned long lastUpdateTime = 0;

// --- WiFi Configuration ---
#define WIFI_SSID "am"
#define WIFI_PASSWORD "0803536035"

// --- Display Parameters ---
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320
#define GMT_OFFSET 8

// --- Function Declaration ---
void displayText(const char *text, int textSize, int x, int y);
void IRAM_ATTR isrButton();

void displayText(const char *text, int textSize = 5, int x = 0, int y = 0)
{

  // screen.fillScreen(TFT_BLACK);           // clear screen with black background
  tft.fillRect(210, 0, SCREEN_WIDTH - 210, SCREEN_HEIGHT, TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // set text color
  tft.drawString(text, x, y, textSize);   // display text
}

// Button ISR
void IRAM_ATTR isrButton() {
  unsigned long now = millis();
  if (now - lastButtonTime > 200) {   // 200 ms debounce
    buttonPressed = true;
    lastButtonTime = now;
  }
}

class Page
{
protected:
  void drawBitmapGif(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h, uint16_t frameCount)
  {
    uint32_t offset = 0;
    for (uint16_t frameId = 0; frameId < frameCount; frameId++)
    {
      offset = frameId * w * h;
      tft.pushImage(x, y, w, h, bitmap + offset);
      delay(500); // Delay between frames
    }
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

public:
  virtual void setup() {};

  virtual void render() = 0;
};

class PageIconText : public Page
{
public:
  const IconSequence *icon;
  char text[128];

  int iconPosX = 30;
  int iconPosY = (SCREEN_HEIGHT - icon->height) / 2;

  //  int textPosX = iconPosX + icon->width + 30;
  int textPosX = 250;
  int textPosY = SCREEN_HEIGHT / 2;

  PageIconText(const IconSequence *icon, char *text) : icon(icon)
  {
    strcpy(this->text, text);
  }

  void render() override
  {
    tft.fillScreen(TFT_BLACK);
    this->drawBitmapGif(iconPosX, iconPosY, icon->frames, icon->width, icon->height, icon->count);
    displayText(this->text, 4, this->textPosX, this->textPosY - 20);
  }
};

class PageTemperature : public Page
{
public:
    struct TempData { String place; float value; };
    TempData temperature[32];
    int tempCount = 0;

    // Independent indices
    int tempIndex = 0;
    int tempBase = 0;

    const char *API = "https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType=rhrread";
    
  PageTemperature() : Page()
  {
  }

  void setup() override
  {
    DynamicJsonDocument doc(8192);
    Serial.println("[PageWeather] Fetching weather...");

    this->APIGetRequest(this->API, doc);

    tempCount = 0;
    for (JsonObject item : doc["temperature"]["data"].as<JsonArray>()) {
        temperature[tempCount].place = item["place"].as<String>();
        temperature[tempCount].value = item["value"].as<float>();
        tempCount++;
    }
  }

  void render() override
  {
    tft.fillRect(0, 0, tft.width(), tft.height()/3, TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.setCursor(0, 30);
    tft.setTextSize(3);
    tft.print("   Rainfall (mm)\n > Temperature (C)");

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

    void updateTempIndex(int pos) {
        tempIndex = pos - tempBase;
    }

    void updateTempBase(int encoderPos) {
        tempBase = encoderPos - tempIndex;
    }
};

class PageTime : public PageIconText
{
private:
  int gmtOffset = 8; // GMT+8
public:
  explicit PageTime(int textSize) : PageIconText(&icon_clock, "00:00")
  {
  }

  void setup() override
  {
    Serial.println("[PageTime] Setting up NTP...");
    configTime(this->gmtOffset * 60 * 60, 0, "pool.ntp.org");
  }

  void render() override
  {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
      Serial.println("[PageTime] Unable to obtain time.");
      return;
    }
    Serial.printf("[PageTime] Current time: %s\n", this->text);


    strftime(this->text, sizeof(this->text), "%H:%M", &timeinfo);
    PageIconText::render();
    delay(500);
  }
};

class PageGif : public Page
{
private:
  const IconSequence *icon;

public:
    explicit PageGif(const IconSequence *icon) : icon(icon)
    {
    }
    
    void render() override
    {
        tft.fillScreen(TFT_BLACK);
        this->drawBitmapGif((SCREEN_WIDTH - icon->width) / 2, (SCREEN_HEIGHT - icon->height) / 2, icon->frames, icon->width, icon->height, icon->count);
    }
};

// --- Page Initialization ---
Page *pages[] = {
    new PageTime(GMT_OFFSET),
    new PageTemperature(),
    new PageGif(&icon_ghost)
};

Page *pageNoWifi = new PageIconText(&icon_ghost, "No WiFi");

// Time = 0, Temperature = 1, Gif = 2
int pageIdx = 0;

void setup()
{
  //  Screen Setup
  tft.init();
  pinMode(TFT_LED, OUTPUT);  
  digitalWrite(TFT_LED, HIGH); // turn on LED
  tft.setRotation(1); // Landscape
  Serial.begin(115200);

  // Encoder setup
  ESP32Encoder::useInternalWeakPullResistors = puType::up; // enable pullups
  encoder.attachHalfQuad(ROTARY_PINA, ROTARY_PINB); // attach pins
  encoder.setCount(0); // start at 0

  // Button setup
  pinMode(ROTARY_PINSW, INPUT_PULLUP);
  attachInterrupt(ROTARY_PINSW, isrButton, FALLING);

  tft.fillScreen(TFT_BLACK);

  //  WiFi Setup
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting to WiFi...");
    displayText("Connecting to WiFi...", 4, 150, SCREEN_HEIGHT / 2);
    delay(500);
  }

  tft.fillScreen(TFT_BLACK);
  delay(1000);
  for (auto *page : pages)
  {
    page->setup();
  }
  pages[pageIdx]->render();
  lastUpdateTime = millis();
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("[PageNoWifi] Connecting to WiFi...");
    tft.fillScreen(TFT_BLACK);
    pageNoWifi->render();
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.printf(".");
    }
    for (auto *page : pages)
    {
      page->setup();
    }
    pages[pageIdx]->render();
    lastUpdateTime = millis();
  }

  static int lastPos = 0;
  int pos = encoder.getCount();

    // update page data based on encoder position
  if (pos != lastPos)
  {
    if (pageIdx == 1) // Temperature Page
    {
        auto *tempPage = static_cast<PageTemperature *>(pages[pageIdx]);
        tempPage->updateTempIndex(pos);
        pages[pageIdx]->render();
    }
    lastPos = pos;
  }

  if (buttonPressed)
  {
    buttonPressed = false;
    if (pageIdx == 1)
    {
        // leaving temperature mode, remember current encoder count
        auto *tempPage = static_cast<PageTemperature *>(pages[pageIdx]);
        tempPage->updateTempBase(encoder.getCount());
    }

    pageIdx = (pageIdx + 1) % (sizeof(pages) / sizeof(pages[0]));
    pages[pageIdx]->render();
    lastUpdateTime = millis();
  }

  if (millis() - lastUpdateTime > 5 * 1000) // Update every 5 seconds
  {
    pages[pageIdx]->render();
    lastUpdateTime = millis();
  }
}
