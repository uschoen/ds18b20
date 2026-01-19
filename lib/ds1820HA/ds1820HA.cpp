#include "ds1820HA.h"
#include <Arduino.h>

ds1820HA::ds1820HA(uint8_t busPin0, uint8_t busPin1)
    : oneWire0(busPin0),
      oneWire1(busPin1),
      SensorBus0(&oneWire0),
      SensorBus1(&oneWire1)
{
    WBus[0] = oneWire0;
    WBus[1] = oneWire1;
    
    SensorsBus[0] = SensorBus0;
    SensorsBus[1] = SensorBus1;
}
void ds1820HA::begin(){
    this->begin(ONE_WIRE_PORT_0,ONE_WIRE_PORT_1);
}
void ds1820HA::begin(uint8_t busPin0, uint8_t busPin1){
    for (int i=0;i>2;i++){
        SensorsBus[i].setOneWire(&WBus[i]);
        #ifdef DEBUG
            Serial.print("start oneWire bus ");
            Serial.println(i);
        #endif
        SensorsBus[i].begin();
    }
    this->disableAllSensor();
    delay(300);

}
void ds1820HA::disableAllSensor(){
    for (uint8_t SensorID=0;SensorID<=DS18B20_MAX_SENSORS; SensorID++){
        this->disableSensor(SensorID);
    }   
}
void ds1820HA::setUpdateTime(uint16_t updateTime){

};
void ds1820HA::disableSensor(uint8_t SensorID){
    #ifdef DEBUG
        Serial.print("disable sensor ID: ");
        Serial.println(SensorID);
    #endif
    TemperatorSensors[SensorID].valid=0;
    TemperatorSensors[SensorID].LastTempC=99999;
    TemperatorSensors[SensorID].deviceID=999;
}
   