<h1>Welcome to the Mini TV Workshop!</h1> 
<b> This step relates to the setup of the workshop, 
which we have already completed for the Computer Lab. 
Step 0 is just here in case you want to try this on 
your own computer.</b>

---
For this project, you'll need to install the following 
software:
- Arduino IDE (https://www.arduino.cc/en/software/)

As well as the following packages inside of Arduino IDE:
- Boards Managers -> esp32 by Espressif Systems
  (Note: NOT Arduino ESP32 Boards by Arduino)
- Library Managers -> TFT_eSPI by Bodmer

NOTE: The esp32 board may fail to download due to its size. 
Follow the solution here in case that happens:
(https://forum.arduino.cc/t/downloading-esp32-3-3-5-fails/1420739/7)
---

Next, download the User_Setup.h file in this folder, and 
replace the existing User_Setup.h file in This PC > 
Documents > Arduino > libraries > TFT_eSPI. This is a setup 
specifically configured for the screen we are using.  

Lastly, run an example program from this Github Repo. We 
recommend using Program > steps > step_01 > step_01_example.

If the screen lights up, you're good to go!

---
<b>Hardware Connection Details: </b>  

Pull Pin 25 to HIGH to light screen, PWM Pin 25 for
brightness control  
Once Power Switch is implemented set Pin 5 to INPUT_PULLDOWN,
if LOW then enter sleep

<b>Pin Config</b>  
TFT Screen:  
#define TFT_MISO 19  // Unused  
#define TFT_MOSI 23  
#define TFT_SCLK 18  
#define TFT_CS   15  // Chip select control pin  
#define TFT_DC    2  // Data Command control pin  
#define TFT_RST   4  // Reset pin (could connect to RST pin

<b>Rotary Encoder</b>  
#define SW       13  // Button function  
#define DT       12  
#define CLK      14  