// DigiMouse test and usage documentation
// CAUTION!!!! This does click things!!!!!!!!
// Created by Sean Murphy (duckythescientist)

#include "DigiMouse.h"

void setup() {
  // Do nothing? It seems as if the USB hardware is ready to go on reset
}

void loop() {
  // If not using plentiful DigiMouse.delay(), make sure to call
  // DigiMouse.update() at least every 50ms
  
  // move across the screen
  // these are signed chars
  DigiMouse.moveY(10); //down 10
  DigiMouse.delay(500);
  DigiMouse.moveX(20); //right 20
  DigiMouse.delay(500);
  DigiMouse.scroll(5);
  DigiMouse.delay(500);
  
  // or DigiMouse.move(X, Y, scroll) works
  
  // three buttons are the three LSBs of an unsigned char
  DigiMouse.setButtons(1<<0); //left click
  DigiMouse.delay(500);
  DigiMouse.setButtons(0); //unclick all
  DigiMouse.delay(500);
}
