#include <Arduino.h>
#include <ds1820HA.h>
/*
insert in platformio.ini 
build_flags = 
	'-D DEBUG'
	'-D ONE_WIRE_PORT={6,7}'
  '-D ONE_WIRE_BUS_COUNT=2'
	'-D DS18B20_MAX_SENSORS=16'
	'-D DS1820_UPDATE_TIME=1000'
    or as config flag or in our configuratione file
*/

ds1820HA TempSensor;

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    while (!Serial){};
    Serial.println("start ds1820 example");
  #endif
  
  TempSensor.begin();
  

}

void loop() {
  TempSensor.loop();
 
}

