#include <Arduino.h>

int ledPin =  1;    // LED connected to digital pin 1

void setup() {
    pinMode(ledPin, OUTPUT);
}

void loop() {
    digitalWrite(ledPin, HIGH);
    delay(50);
    digitalWrite(ledPin, LOW);
    delay(50);
}

int main(void) {
    init();
    setup();
    for (;;) {
        loop();
    }
}
