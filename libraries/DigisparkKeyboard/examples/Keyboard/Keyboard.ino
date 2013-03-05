#include "DigiKeyboard.h"

void setup() {
  // don't need to set anything up to use DigiKeyboard
}


void loop() {
  // running DigiKeyboard.update() from time to time is a good idea - it talks to the computer, answering any
  // requests the computer has made recently. If you use DigiKeyboard.delay() plentifully you can omit this
  // as DigiKeyboard.delay() calls update internally.
  DigiKeyboard.update();

  // this is generally not necessary but with some older systems it seems to prevent missing the first character after a delay:
  DigiKeyboard.sendKeyStroke(0);

  // send out some keys
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

  // It's better to use DigiKeyboard.delay() over the regular Arduino delay() because it keeps
  // talking to the computer to make sure the computer knows the keyboard is alive and connected
  DigiKeyboard.delay(5000);
}
