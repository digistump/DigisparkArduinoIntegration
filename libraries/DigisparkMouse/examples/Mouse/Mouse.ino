//DigiMouse test and usage documentation
//CAUTION!!!! This does click things!!!!!!!!
//Created by Sean Murphy (duckythescientist)

#include "DigiMouse.h"

unsigned int count = 0;

void setup() {
  //Do nothing? It seems as if the USB hardware is ready to go on reset

}

void loop() {
  
  DigiMouse.update();//call this at least every 50ms
  //calling more often than that is fine
  //this will actually only send the data every once in a while unless the data is different
  
  //move across the screen
  //these are signed chars
  if(!count--) DigiMouse.moveY(10);//down 10
  if(count == 30000) DigiMouse.moveX(20);//right 20
  if(count == 50000) DigiMouse.scroll(5);//scroll down 5
  
  //or DigiMouse.move(X, Y, scroll) works
  
  //three buttons are the three LSBs of an unsigned char
  if(count == 29000) DigiMouse.setButtons(1<<0);//left click
  if(count == 31000) DigiMouse.setButtons(0);//unclick all
  

}