//DigiJoystick test and usage documentation

#include "DigiJoystick.h"
char myBuf[8] = {0};

unsigned int count = 0;

void setup() {
  //Do nothing? It seems as if the USB hardware is ready to go on reset

}


void loop() {
  
  DigiJoystick.update();//call this at least every 50ms
  //calling more often than that is fine
  //this will actually only send the data every once in a while unless the data is different
  
  //toggle button 1
  if(!count--) myBuf[6] = !myBuf[6];//about 2Hz blinking
  //this means looping happens at about 65536 times per second
  //15uS per loop
  
  //actually set the data values
  DigiJoystick.setValues( (myBuf));
  //{X, Y, XROT, YROT, ZROT, SLIDER, buttonLowByte, buttonHighByte}
  
  //we can also set values like this
  DigiJoystick.setX((unsigned char)(count>>8));
  DigiJoystick.setY((unsigned char)0x30);
  DigiJoystick.setXROT((unsigned char)0x60);
  DigiJoystick.setYROT((unsigned char)0x90);
  DigiJoystick.setZROT((unsigned char)0xB0);
  DigiJoystick.setSLIDER((unsigned char)0xF0);
  
  //we can also set buttons like this (lowByte, highByte)
  //DigiJoystick.setButtons(0x00, 0x00);

}
