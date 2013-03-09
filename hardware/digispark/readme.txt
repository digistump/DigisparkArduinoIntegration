
Arduino-Tiny is an open source set of ATtiny "cores" for the Arduino platform. 

The Arduino platform currently supports Atmel ATmega processors. There is a need
for the Arduino platform to work with physically smaller DIP package processors.
The intent of this project is fulfill that need. Specifically, our goal is to
provide a core that enables Arduino users to work with the ATtiny 84 (44/24),
ATtiny 85 (45/25), and ATtiny 2313 processors. 

For more information about Arduino, see the website at: http://www.arduino.cc/


INSTALLATION

First ensure the Arduino software is correctly installed.

Next...

* Ensure the Arduino IDE is NOT running.

* Download the Arduino-Tiny archive (ZIP-file).

* Locate the Arduino Sketch folder.  This is the folder where the Arduino IDE
  stores Sketches.

* Ensure the "hardware" folder exists under the Arduino Sketch folder.  For
  example, if the Arduino Sketch folder is...

      C:\Projects\Arduino\

  Ensure this folder exists...

      C:\Projects\Arduino\hardware\
      
* Extract the contents of the archive into the "hardware" folder.  For example,
  if the Arduino Sketch folder is...

      C:\Projects\Arduino

  After extracting, the following two files should exist...

      C:\Projects\Arduino\hardware\tiny\boards.txt
      C:\Projects\Arduino\hardware\tiny\bootloaders\attiny84\empty.hex

  And the following folder should contain the Arduino-Tiny core...

      C:\Projects\Arduino\hardware\tiny\cores\tiny\

* Open the "boards.txt" file and change both of the "upload.using" entries to
  the appropriate value for your setup.  Following from the examples above, the
  file would be here...

      C:\Projects\Arduino\hardware\tiny\boards.txt

* Start the Arduino IDE and ensure there are two new boards listed under the 
  [Tools] [Board] menu...

      "ATtiny84 @ 8 MHz"
      "ATtiny84 @ 1 MHz"

You are now ready to use Arduino-Tiny!


CREDITS

Arduino is an open source project, supported by many.

The Arduino team is composed of Massimo Banzi, David Cuartielles, Tom Igoe,
Gianluca Martino, and David A. Mellis.

Arduino uses the GNU avr-gcc toolchain, avrdude, avr-libc, and code from
Processing and Wiring.

Arduino-Tiny is based on work by David A. Mellis, René Bohne, R. Wiersma, 
Alessandro Saporetti, and Brian Cook.

