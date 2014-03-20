## Digispark Development sans Arduino IDE

In the top level directory, a slightly modified Arduino.mk has been added. 
Include this in the user Makefile and you can compile and upload for Digispark from the command line:

    $> git clone https://github.com/christopherpoole/DigisparkArduinoIntegration.git
    $> DigisparkArduinoIntegration/examples/Digispark_Examples/NoIDEBlink/
    $> make
    $> sudo make dsupload
    micronucleus --run build-cli/main.hex
    > Please plug in the device ... 
    > Press CTRL+C to terminate the program 

