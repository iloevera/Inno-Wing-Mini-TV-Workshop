/**
 * @file    step_01_example.ino
 * @brief   Display custom text on the screen
 * @author  Aom, Ilo
 * @date    2025-09-12
 * @version 1.0
 */

// Required Libraries: Adafruit GFX, Adafruit ST7735, TFT, Arduino JSON

#include <TFT_eSPI.h> // Drawing text, images, and shapes on the TFT display
#include <SPI.h>      // Serial Data Protocol

// --- TFT Configuration ---
TFT_eSPI tft = TFT_eSPI();
#define TFT_LED 25

// --- Display Parameters ---
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

// --- Custom Text ---
const char *msg = "Hello, Mini TV!";
const int FONT_SIZE = 4;

void displayText(const char *text, int textSize = 1, int x = 0, int y = 0)
{
  tft.fillScreen(TFT_BLACK);              // clear screen with black background
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // set text color
  tft.drawString(text, x, y, textSize);   // display text
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

  int textWidth = tft.textWidth(msg, FONT_SIZE);
  int textHeight = tft.fontHeight(FONT_SIZE);
  
  displayText(msg, FONT_SIZE, ( SCREEN_WIDTH - textWidth ) / 2 , ( SCREEN_HEIGHT - textHeight ) / 2);
}

void loop()
{
  delay(1000);
}
