#include <Arduino.h>
#include <ds1820HA.h>
/*
insert in platformio.ini 
build_flags = 
    '-D ONE_WIRE_PORT_0="6"'
    '-D ONE_WIRE_PORT_1="7"'
    or as config flag or in our
own configuratione file
*/
#ifndef ONE_WIRE_PORT_0
    #define ONE_WIRE_PORT_0 6
#endif
#ifndef ONE_WIRE_PORT_1
    #define ONE_WIRE_PORT_1 7
#endif
ds1820HA TempSensor;

#define DEBUG


void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    while (!Serial){};
    Serial.println("start ds1820 example");
  #endif
  /*
  start without parameters. Define ONE_WIRE_PORT_0 and
  ONE_WIRE_PORT_1 with the BUS Pins 
  TempSensor.begin();
  */
  TempSensor.begin();
  TempSensor.begin(ONE_WIRE_PORT_0,ONE_WIRE_PORT_1);

}

void loop() {
  TempSensor.loop();
 
}

