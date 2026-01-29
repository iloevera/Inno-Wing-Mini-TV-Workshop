#include <TFT_eSPI.h> // Drawing text, images, and shapes on the TFT display
#include <SPI.h>      // Serial Data Protocol

#include "Common_types.h"
// #include "name_of_your_file.h"

// --- TFT Configuration ---
TFT_eSPI tft = TFT_eSPI();
#define TFT_LED 25

// --- Display Parameters ---
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

// --- Icon Display (Customized) ---
// const IconSequence *icon = &name_of_your_icon;

// --- Function Declaration ---
void displayText(const char *text, int textSize, int x, int y);
void drawBitmapGif(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h, uint16_t frameCount);

void displayText(const char *text, int textSize = 5, int x = 0, int y = 0)
{
  // screen.fillScreen(TFT_BLACK);           // clear screen with black background
  tft.fillRect(210, 0, SCREEN_WIDTH - 210, SCREEN_HEIGHT, TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // set text color
  tft.drawString(text, x, y, textSize);   // display text
}

void drawBitmapGif(int16_t x, int16_t y, const uint16_t *bitmap, int16_t w, int16_t h, uint16_t frameCount)
{
 // Your code here
}

void setup()
{
  //  Screen Setup
  tft.init();
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH); // turn on LED
  tft.setRotation(1); // Landscape
  Serial.begin(115200);
  tft.fillScreen(TFT_BLACK);

  delay(1000);
}

void loop() {
  // Your code here
}