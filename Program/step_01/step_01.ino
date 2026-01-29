/**
 * @file    step_01.ino
 * @brief   Display custom text on the screen
 * @author  Aom, Ilo
 * @date    2025-09-12
 * @version 1.0
 */

#include <TFT_eSPI.h> // Drawing text, images, and shapes on the TFT display

// --- TFT Configuration ---
TFT_eSPI tft = TFT_eSPI();
#define TFT_LED 25

// --- Display Parameters ---
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

// --- Custom Text ---
const char *msg = "Hello World!";
const int FONT_SIZE = 4; // number between 1 and 8

void setup()
{
  //  Screen Setup
  tft.init();
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH); // turn on LED
  tft.setRotation(1); // Landscape
  Serial.begin(115200);
  tft.fillScreen(TFT_BLACK);

  /* Your code starts here
   *
   */
}

void loop()
{
/* Your code runs here
 *
 */
  delay(1000);
}
