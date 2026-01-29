Common_types.h needs to be downloaded to the folder of this 
step as it contains the struct the GIFs are stored in.  

See Google Colab file (https://colab.research.google.com/drive/1M2ghhb-EPsKnnBAMrDEIjULArmjGbAZA?usp=sharing)  

Success criteria for this step is being able to display a 
gif on the screen

uint32_t offset = 0;
for (uint16_t frameId = 0; frameId < frameCount; frameId++)
{
offset = frameId * w * h;
tft.pushImage(x, y, w, h, bitmap + offset);
delay(500); // Delay between frames
}