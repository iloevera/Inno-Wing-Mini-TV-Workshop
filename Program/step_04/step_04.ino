#include <ESP32Encoder.h>
#include <TFT_eSPI.h> // Drawing text, images, and shapes on the TFT display
#include <SPI.h>      // Serial Data Protocol

TFT_eSPI tft = TFT_eSPI();

// --- Control Parameters ---
#define ROTARY_PINA 12
#define ROTARY_PINB 14
#define ROTARY_PINSW 13

#define BUTTON 33

// --- Display Parameters ---
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320
#define TFT_LED 25

// --- Button Init ---
ESP32Encoder encoder;
volatile int32_t encoderCount = 0;
volatile bool encoderChanged = false;
volatile bool currentButtonState = false;
volatile bool buttonPressed = false;
char *encoderMsg;
char *buttonMsg;

void IRAM_ATTR encoderISR() {
    encoderCount = encoder.getCount();
    encoderChanged = true;
}

void IRAM_ATTR buttonISR() {
    currentButtonState = !(currentButtonState);
    buttonPressed = true;
}

void displayText(const char *text, int textSize = 4, int x = 0, int y = 0)
{
  tft.fillScreen(TFT_BLACK);              // clear screen with black background
  tft.setTextColor(TFT_WHITE, TFT_BLACK); // set text color
  tft.drawString(text, x, y, textSize);   // display text
}

void setup()
{
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);
  //  Screen Setup
  tft.init();
  tft.setRotation(1); // Landscape
  Serial.begin(115200);
  tft.fillScreen(TFT_BLACK);

  Serial.begin(115200);
  
  pinMode(BUTTON, INPUT);
  
  // Enable the weak pull up resistors for encoder
  ESP32Encoder::useInternalWeakPullResistors = puType::up;

  // Attach encoder pins
  encoder.attachHalfQuad(ROTARY_PINA, ROTARY_PINB);
  
  // Set starting count value
  encoder.setCount(0);
  encoderCount = 0;
  
  attachInterrupt(digitalPinToInterrupt(ROTARY_PINA), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROTARY_PINB), encoderISR, CHANGE);
  
  attachInterrupt(digitalPinToInterrupt(BUTTON), buttonISR, CHANGE);
  currentButtonState = bool(digitalRead(BUTTON));

  displayText("Screen Start!", 4, 100, 100);

  delay(2000);
}

void loop() {

    delay(1000);
}