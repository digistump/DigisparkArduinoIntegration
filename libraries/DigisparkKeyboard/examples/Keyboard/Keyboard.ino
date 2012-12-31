#include "DigiKeyboard.h"

void setup() {

}


void loop() {
  
  DigiKeyboard.update();
  DigiKeyboard.sendKeyStroke(0); //this is generally not necessary but with some older systems it seems to prevent missing the first character after a delay
  DigiKeyboard.sendKeyStroke(KEY_H, MOD_SHIFT_RIGHT);
  DigiKeyboard.sendKeyStroke(KEY_E);
  DigiKeyboard.sendKeyStroke(KEY_L);
  DigiKeyboard.sendKeyStroke(KEY_L);
  DigiKeyboard.sendKeyStroke(KEY_O);
  DigiKeyboard.sendKeyStroke(KEY_SPACE);
  DigiKeyboard.sendKeyStroke(KEY_D);
  DigiKeyboard.sendKeyStroke(KEY_I);
  DigiKeyboard.sendKeyStroke(KEY_G);
  DigiKeyboard.sendKeyStroke(KEY_I);
  DigiKeyboard.sendKeyStroke(KEY_S);
  DigiKeyboard.sendKeyStroke(KEY_P);
  DigiKeyboard.sendKeyStroke(KEY_A);
  DigiKeyboard.sendKeyStroke(KEY_R);
  DigiKeyboard.sendKeyStroke(KEY_K);
  DigiKeyboard.sendKeyStroke(KEY_ENTER);
  delay(5000);



}
