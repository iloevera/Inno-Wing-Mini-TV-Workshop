#include <ESP32Encoder.h>
#include <TFT_eSPI.h> 
#include <SPI.h>      

TFT_eSPI tft = TFT_eSPI();
ESP32Encoder encoder;

// --- Hardware Pins ---
#define ROTARY_PINA 12
#define ROTARY_PINB 14
#define BUTTON 33
#define TFT_LED 25

// --- Global Variables for task1&2 ---
long currentCount = 0;
long lastCount = 0;
bool isSystemOn = false;

// --- Debounce Variables ---
int lastSteadyState = HIGH;
int lastFlickerableState = HIGH;
unsigned long lastDebounceTime = 0;


// --- Display Function ---
void displayText(String text, uint16_t color = TFT_WHITE, int textSize = 4) {
  tft.fillScreen(TFT_BLACK);              
  tft.setTextColor(color, TFT_BLACK); 
  tft.drawString(text, 50, 100, textSize);   
}

void setup() {
  Serial.begin(115200);

  // Screen Setup
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);
  tft.init();
  tft.setRotation(1); 
  tft.fillScreen(TFT_BLACK);
  
  // Hardware Setup (Fixed for stability)
  pinMode(BUTTON, INPUT_PULLUP);
  ESP32Encoder::useInternalWeakPullResistors = puType::up;
  encoder.attachHalfQuad(ROTARY_PINA, ROTARY_PINB);
  encoder.setCount(0);
  displayText("Sandbox Ready!", TFT_GREEN);
  delay(2000);
  tft.fillScreen(TFT_BLACK);
}

void loop() {
  // Read current states from hardware
  currentCount = encoder.getCount();
  int buttonReading = digitalRead(BUTTON);

  // --- ATTENDEE WORKSPACE STARTS HERE ---
  // TASK 1: Detect Encoder Rotation
  // Hint: Compare 'currentCount' with 'lastCount'

    // 1a. Print the new count to the Serial Monitor
    
    // 1b. Update the screen using displayText()
    // Example: displayText("Count: " + String(currentCount));
    
    // 1c. (Bonus) If the count is greater than 10, make the text to any color using TFT!
    
    lastCount = currentCount; // Remember the new count
  }

  // TASK 2: Button Press Logic (Debounced in background)
  // We handle the bouncing for you. Just check the logic!
  if (buttonReading != lastFlickerableState) {
    lastDebounceTime = millis();
    lastFlickerableState = buttonReading;
  }

  if ((millis() - lastDebounceTime) > 50) {
    if (buttonReading != lastSteadyState) {
      lastSteadyState = buttonReading;

      // The button is pressed down!
      if (lastSteadyState == LOW) {
        
        // 2a. Toggle the 'isSystemOn' boolean variable
        
        // 2b. If the system is ON, display "System: ON" in GREEN
        //     If the system is OFF, display "System: OFF" in RED
        
        
        Serial.println("Button Clicked!");
      }
    }
  }
        
  delay(10);
}
