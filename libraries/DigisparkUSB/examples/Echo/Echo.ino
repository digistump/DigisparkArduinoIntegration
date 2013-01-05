#include <DigiUSB.h>

void setup() {
  DigiUSB.begin();
}

void get_input() {

  int lastRead;
  // when there are no characters to read, or the character isn't a newline
  while (1==1) {
    if(DigiUSB.available()){
      //something to read
      lastRead = DigiUSB.read();
      DigiUSB.write(lastRead);
      
      if(lastRead == '\n')
        break;
    }
    // refresh the usb port
    DigiUSB.refresh();
    delay(10);

  }

}


void loop() {
  //print output
  DigiUSB.println("Waiting for input...");
  //get input
  get_input();

}