#include <Arduino.h>
#include <ds1820HA.h>
#include <ArduinoJson.h> 
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
JsonDocument config;
void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    while (!Serial){};
    Serial.println("");
    Serial.println("");
    Serial.println("start ds1820 example");
  #endif
  
  //String cfg="{\"wifi\":\"gps\",\"password\":\"ggfggf\",\"devices\":{\"28:ff:d4:55:91:15:01:9e\":[{\"deviceAddress\":\"28:ff:d4:55:91:15:01:9e\",\"deviceID\":0,\"busID\":0,\"temperature\":-127,\"canID\":\"\",\"homeAssistantID\":\"28:ff:d4:55:91:15:01:9e\",\"connected\":true,\"enable\":false}]}}}";
  //String cfg="{\"wifi\":\"gps\",\"password\":\"ggfggf\",\"devices\":[]}";
	//String cfg="{\"wifi\":\"gps\",\"password\":\"ggfggf\"}";
  String cfg="{\"devices\":{\"28:ff:e2:34:91:15:01:99\":[{\"deviceAddress\":\"28:ff:e2:34:91:15:01:99\",\"deviceID\":1,\"busID\":0,\"temperature\":28.9,\"canID\":\"\",\"homeAssistantID\":\"28:ff:e2:34:91:15:01:99\",\"connected\":true,\"enable\":true}],\"28:ff:d4:55:91:15:01:9e\":[{\"deviceAddress\":\"28:ff:d4:55:91:15:01:9e\",\"deviceID\":0,\"busID\":0,\"temperature\":28.8,\"canID\":\"\",\"homeAssistantID\":\"28:ff:d4:55:91:15:01:9e\",\"connected\":true,\"enable\":true}]}}";
  deserializeJson(config,cfg);
  TempSensor.begin(config);
  // TempSensor.begin();
  

}

void loop() {
  TempSensor.loop();
  serializeJson(config,Serial);
  Serial.println(" CONFIG ZENTRAL");
  delay(4000);
}

