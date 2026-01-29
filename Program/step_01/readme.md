<h1> Step 1: Display Text </h1>  
(Insert picture of this code running)

For this step, we are going to be writing text to our 
screen via the ESP32. 
To achieve this, we will be using 3 functions, as follows:  

- tft.fillScreen(uint16_t color);
    - Fills the screen with a solid colour; uint16_t color 
  refers to a colour code written as an integer. For our 
  purposes, replace it with TFT_BLACK
- tft.setTextColor(uint16_t color); 
  - Change the colour of the text, can replace uint16_t 
  color with TFT_WHITE
- tft.drawString(text, x, y, textSize);
  - Displays your message. text is your message, x and y are 
  the position of your text, and textSize is the font size 
  of your text;

Your task is to fill out these 3 functions in the step_01.ino 
file, either under void setup() (runs once at the start) or 
void loop() (loops constantly when the device is on). Your 
code is considered successful if you are able to display your 
own custom message.

Don't forget to end every line of your code with a semicolon (;)

For more information, see:
(https://praxismechatronics.com/wp-content/uploads/2024/11/tft_eSPI-cheat-sheet-PM.pdf)