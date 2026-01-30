<h1> Step 2: Display GIF </h1>  

For this step, we are going to be drawing GIFs to our screen.  

---
<b>Setup</b>  
Download the Common_types.h file from this step and put it 
in the same folder as your code
---

To display a GIF, we will be drawing out different frames of 
a static image every time you loop the screen. You will first 
need to convert your image into a C-array, a format the 
TFT_eSPI library can understand. See the Google Colab file at 
(https://colab.research.google.com/drive/1M2ghhb-EPsKnnBAMrDEIjULArmjGbAZA?usp=sharing)

The displayText() function should be similar to what you 
completed in step 1. A good way to think about the drawBitmapGif() 
function is to imagine reading the amount of pixels for one 
frame of your GIF, drawing it, then reading the amount of pixels 
for the next frame of your GIF.

Your Common_types.h file should have a struct containing 
information about your GIF. For instance, if your GIF was 
named icon, icon->width would tell me the width of your 
icon. Check the Common_types.h file for more information.

Success criteria for this step is being able to display a 
gif on the screen. Good luck!

