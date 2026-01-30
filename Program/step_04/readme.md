<h1> Step 4: Rotary Encoder and Button</h1>
This step will teach you to read inputs from your 
rotary encoder and your button.

<b>Due to problems with the hardware connection, 
please only connect the rotary encoder after your code is 
running.</b> The same applies if you are connecting a button. We 
are sorry for this inconvenience.

Furthermore, due to limitations of the ESP32, only one power 
source can be enabled at a time. As such, please only plug in 
EITHER your USB-C cable OR your battery, as the port may not 
be detectable otherwise.

---
<b>Setup</b>  

Download the ESP32Encoder library via library manager

---

The rotary encoder works via two dials, DT (data) and CLK (clock). 
CLK keeps a steady pulse while DT is affected by the turning 
of the dial. Since DT moves either slightly earlier or later 
than CLK, we are able to judge the difference between the two 
dials to determine the direction of rotation.

Clockwise rotation:  
```text
CLK:  __|‾‾|__|‾‾|__|‾‾|__  (leads by 90°)
DT:   ____|‾‾|__|‾‾|__|‾‾|  (follows behind)
Time: → → → → → → → → → → →
```
Counter-clockwise rotation:
```text
DT:   __|‾‾|__|‾‾|__|‾‾|__  (leads by 90°)
CLK:  ____|‾‾|__|‾‾|__|‾‾|  (follows behind)
Time: → → → → → → → → → → →
```

The step_04.ino contains functions that allow the variables
encoderCount, encoderChanged, currentButtonState, buttonPressed 
to be changed at any point in the program, with your job being 
to figure out how to use them meaningfully.

Example 1 for this step includes a basic implementation of 
the button and encoder, and Example 2 includes a complete 
interaction between the encoder and data from the weather API. 
Your job at this step is to be able to interact between the 
weather API data and the rotary encoder and/or button in some 
sort of way. This is quite a challenging step so expect to 
spend some time here.  

For more information, see:  
How to read from a button:
https://docs.arduino.cc/built-in-examples/digital/Button/

Rotary Encoder Repo:
https://github.com/madhephaestus/ESP32Encoder/tree/master

Good luck!