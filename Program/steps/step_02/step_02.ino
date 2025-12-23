/**
 * @file    mini_tv_master.ino
 * @brief   Generating GIF icon pixels + display on the screen
 * @author  Aom, Ilo
 * @date    2025-09-12
 * @version 1.0
 */

// Required Libraries: Adafruit GFX, Adafruit ST7735, TFT, Arduino JSON, ESP32Encoder

#include <TFT_eSPI.h> // Drawing text, images, and shapes on the TFT display
#include <SPI.h>      // Serial Data Protocol

#include "Icons.h"

// --- TFT Configuration ---
TFT_eSPI tft = TFT_eSPI();
#define TFT_LED 25

// --- Display Parameters ---
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320
#define GMT_OFFSET 8

// --- Icon Display (Customized) ---
const IconSequence *icon = &icon_ghost;

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
  uint32_t offset = 0;
  for (uint16_t frameId = 0; frameId < frameCount; frameId++)
  {
    offset = frameId * w * h;
    tft.pushImage(x, y, w, h, bitmap + offset);
    delay(500); // Delay between frames
  }
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

void loop()
{
  // tft.fillScreen(TFT_BLACK);
  drawBitmapGif((SCREEN_WIDTH - icon->width) / 2, (SCREEN_HEIGHT - icon->height) / 2, icon->frames, icon->width, icon->height, icon->count);
}
