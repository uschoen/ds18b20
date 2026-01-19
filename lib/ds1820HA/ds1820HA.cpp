#include "ds1820HA.h"
#include <Arduino.h>

ds1820HA::ds1820HA(){}

void ds1820HA::begin(){
    for (int busID=0;busID>ONE_WIRE_BUS_COUNT;busID++){
        oneWireBus[busID].begin(OneWirePins[busID]);
        SensorsBus[busID].setOneWire(&oneWireBus[busID]);
        #ifdef DEBUG
            Serial.print("start oneWire bus ");
            Serial.println(busID);
        #endif
        SensorsBus[busID].begin();
    }
    delay(300);
}
/*  setup ds1820 class with import old ds1820 sensors
    and settings
    config -> JsonDokument

      [98:98:87:2d:fe:h2:00]={
          DeviceAddress  sensorAddress;
          int            lastTempC;
          uint           canAddress;
          uint           interval;
          uint           ds1820id;
          bool           valid;
          bool           connected;
      }
      [12:fa:12:2d:ff:b2:10]={....}
      [a2:ea:02:4d:aa:c2:99]={....}
      [....]
      */
void ds1820HA::begin(JsonDocument& config){
    this->_config=config;
    this->setAllSensorNoValid();
    this->begin();
}
/*
        stringToDeviceAddress(String);
        ----------------------------------------------------
        convert a ATring to a DeviceAddress
        from:
        0x28:0x1D:0x39:0x31:0x2:0x0:0x0:0xF0 
        to:
        0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 
*/
void ds1820HA::stringToDeviceAddress(String stringAddress,DeviceAddress& tempDeviceAdress){
    char* ptr =  const_cast<char*>(stringAddress.c_str());
    for (int i = 0; i < 8; i++){
        tempDeviceAdress[i] = strtoul(ptr, &ptr, 16);
        ptr++;
    }
}
 /*
        void deviceAddresToString(DeviceAddress);
        ----------------------------------------------------------
        convert a int 8 array to a string with format
        nn:nn:nn:nn:nn:nn:nn:nn
       
        return String
      */
String ds1820HA::deviceAddresToString(DeviceAddress deviceAddress){
    String  deviceString;
    for (uint8_t i = 0; i < 8; i++)
    {
        // zero pad the address if necessary
        if (deviceAddress[i] < 16) {
            deviceString+=String("0");
        }
        deviceString+=String(deviceAddress[i], HEX);
        if (i<7){deviceString=deviceString+String(":");}        
    }
    return deviceString;
}
/* set all sensor to no valid
    set value "valid" to false
*/
void ds1820HA::setAllSensorNoValid(){
    JsonObject sensors = this->_config.as<JsonObject>();
    for (JsonPair kv : sensors) {
          String sensorAddress=String(kv.key().c_str());
          this->setSensorNoValid(sensorAddress);
        }   
}
/* set a sensor to no valid
*/
void ds1820HA::setSensorNoValid(String sensorAddress){
    #ifdef DEBUG
        Serial.print("set sensor ");
        Serial.print(sensorAddress);
        Serial.println("to no valid");
    #endif
    this->_config[sensorAddress]["valid"]=false;
}
/* set a sensor to no valid
*/
void ds1820HA::setSensorValid(String sensorAddress){
    #ifdef DEBUG
        Serial.print("set sensor ");
        Serial.print(sensorAddress);
        Serial.println("to valid");
    #endif
    this->_config[sensorAddress]["valid"]=true;
}
/* set interval to read sensor values (defualt 1000ms/1sec)
    parameter:
        ulong updateTime in milli second;
*/
void ds1820HA::setSensorInterval(ulong updateInterval){
    this->sensorInterval=updateInterval;
};
void ds1820HA::updateSensors(int busID){
    uint8_t foundDevices=SensorsBus[busID].getDeviceCount();
    #ifdef DEBUG
        Serial.print("found for busID:");
        Serial.print(busID);
        Serial.print(", ");
        Serial.print(foundDevices);
        Serial.println(" device/s");
    #endif 
    if (foundDevices>0){
        // found more than 1 devices
        for(uint8_t ds1820id=0;ds1820id<foundDevices; ds1820id++){ 
            DeviceAddress ds1820DeviceAddress; 
            String StringDeviceAddress;
            if(SensorsBus[busID].getAddress(ds1820DeviceAddress, ds1820id)){ 
                StringDeviceAddress=this->deviceAddresToString(ds1820DeviceAddress);
                if (this->_config[StringDeviceAddress].is<String>()){
                    #ifdef DEBUG
                        Serial.print("update device address: ");
                        Serial.println(StringDeviceAddress);
                    #endif
                    this->setSensorValid(StringDeviceAddress);
                }else{
                    #ifdef DEBUG
                        Serial.print("add device address: ");
                        Serial.println(StringDeviceAddress);
                    #endif
                    this->_config[StringDeviceAddress]["deviceAdress"]=StringDeviceAddress;
                    this->_config[StringDeviceAddress]["busID"]=busID;
                    this->_config[StringDeviceAddress]["temperature"]=9999;
                    this->_config[StringDeviceAddress]["canID"]="";
                    this->_config[StringDeviceAddress]["homeAssistantID"]=StringDeviceAddress;
                    this->_config[StringDeviceAddress]["enable"]=false;
                    this->setSensorValid(StringDeviceAddress);
                }
            }else{
                #ifdef DEBUG
                    Serial.print("can not get device address: ");
                    Serial.print(StringDeviceAddress);
                    Serial.print(" for id: ");
                    Serial.println(ds1820id);
                #endif
            }
        }
    }
}
void ds1820HA::loop(){
    if (millis() - this->lastSensorInterval >= this->sensorInterval){        // check evry 1s
        this->lastSensorInterval =millis();
        this->setAllSensorNoValid();
        for (uint busID=0;busID>sizeof(SensorsBus);busID++){
            #ifdef DEBUG
                Serial.print("check bus ");
                Serial.print(busID);
                Serial.println("for new onewire devices");
            #endif
            SensorsBus[busID].begin(); // begin().. find new devices an refresh the device counts.
            this->updateSensors(busID);            
        }
    }
}   