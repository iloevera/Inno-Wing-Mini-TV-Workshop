Using TFT_eSPI library on Arduino (https://github.com/Bodmer/TFT_eSPI)
Pull Pin 25 to HIGH to light screen, PWM Pin 25 for brightness control
Once Power Switch is implemented set Pin 5 to INPUT_PULLDOWN, if LOW then enter sleep

Pin Config
TFT Screen:
#define TFT_MISO 19  // Unused
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
#define TFT_RST   4  // Reset pin (could connect to RST pin)

Rotary Encoder
#define SW       13  // Button function
#define DT       12
#define CLK      14
