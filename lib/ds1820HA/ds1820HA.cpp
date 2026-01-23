#include <ds1820HA.h>
#include <Arduino.h>

ds1820HA::ds1820HA(){}

void ds1820HA::begin(){
    #ifdef DEBUG
        Serial.println("beginn onewire setup ds1820HA");
    #endif 
    if ((*this->_config)["devices"].is<JsonObject>()){
         #ifdef DEBUG
            Serial.println("found 'devices' in config");
        #endif
        //this->_devices=this->_config["devices"];
    }else{
        #ifdef DEBUG
            Serial.println("add 'devices' to config");
        #endif
        (*this->_config)["devices"].to<JsonObject>();
    }
    #ifdef DEBUG_JSON
        serializeJson((*this->_config),Serial);
        Serial.println(" CONFIG");
        serializeJson((*this->_config)["devices"],Serial);
        Serial.println(" DEVICES");
    #endif
    this->setAllSensorNotConnected();
    for (int busID=0;busID<ONE_WIRE_BUS_COUNT;busID++){
        #ifdef DEBUG
            Serial.print("start oneWire bus ");
            Serial.print(busID);
            Serial.print(" pin: ");
            Serial.println(this->OneWirePins[busID]);
        #endif
        this->oneWireBus[busID].begin(this->OneWirePins[busID]);
        this->SensorsBus[busID].setOneWire(&this->oneWireBus[busID]);
        this->SensorsBus[busID].begin();
    }
    delay(300);
}
/*  setup ds1820 class with import old ds1820 sensors
    and settings
    config -> JsonDokument

      [98:98:87:2d:fe:h2:00]={
          string          deviceAddress;
          int             deviceID,
          float           temperature;
          int             busID;
          uint            canID;
          bool            connected;
          String          homeAssistantID;
          bool            enable,
      }
      [12:fa:12:2d:ff:b2:10]={....}
      [a2:ea:02:4d:aa:c2:99]={....}
      [....]
      */
void ds1820HA::begin(JsonDocument& config){
    #ifdef DEBUG
        Serial.println("beginn ds1820HA");
    #endif
    this->_config=&config;
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
/* set all sensor to no connected
    set value "connected" to false
*/
void ds1820HA::setAllSensorNotConnected(){
    #ifdef DEBUG
        Serial.println("set all Sensors to not connected");
    #endif
    JsonObject sensors=(*this->_config)["devices"].as<JsonObject>();
    for (JsonPair devicePair : sensors) {
        String deviceAddr = devicePair.key().c_str();
        JsonArray instances = devicePair.value().as<JsonArray>();
        for (JsonObject deviceData : instances) {
            this->setSensorNotConnected(deviceData);     
        }  
    }
}
/* set a sensor to no connected
*/
void ds1820HA::setSensorNotConnected(JsonObject& deviceData){
    #ifdef DEBUG
        Serial.print("set sensor ");
        Serial.print(deviceData["deviceAddress"].as<const char*>());
        Serial.println(" to no connected");
    #endif
    deviceData["connected"]=false;
}
void ds1820HA::setSensorEnable(JsonObject& deviceData){
    #ifdef DEBUG
        Serial.print("enable sensor ");
        Serial.println(deviceData["deviceAddress"].as<const char*>());
    #endif
    deviceData["enable"]=true;
};
void ds1820HA::setSensorDisable(JsonObject& deviceData){
    #ifdef DEBUG
        Serial.print("disable sensor ");
        Serial.println(deviceData["deviceAddress"].as<const char*>());
    #endif
    deviceData["enable"]=false;
};
/* set a sensor to no connected
*/
void ds1820HA::setSensorConnected(JsonObject& deviceData){
    #ifdef DEBUG
        Serial.print("set sensor ");
        Serial.print(deviceData["deviceAddress"].as<const char*>());
        Serial.println(" to connected");
    #endif
    deviceData["connected"]=true;
}
/* set interval to read sensor values (defualt 1000ms/1sec)
    parameter:
        ulong updateTime in milli second;
*/
void ds1820HA::setSensorInterval(ulong updateInterval){
    this->sensorInterval=updateInterval;
};
void ds1820HA::updateSensors(int busID){
    uint8_t foundDevices=this->SensorsBus[busID].getDeviceCount();
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
            if(this->SensorsBus[busID].getAddress(ds1820DeviceAddress, ds1820id)){ 
                StringDeviceAddress=this->deviceAddresToString(ds1820DeviceAddress);
                if ((*this->_config)["devices"][StringDeviceAddress].is<JsonArray>()){
                    #ifdef DEBUG
                        Serial.print("update device address: ");
                        Serial.println(StringDeviceAddress);
                    #endif
                    JsonArray instances=(*this->_config)["devices"][StringDeviceAddress].as<JsonArray>();
                    for (JsonObject deviceData : instances) {
                        deviceData["deviceID"]=ds1820id;
                        deviceData["busID"]=busID;
                        this->setSensorConnected(deviceData);
                    }
                }else{
                    #ifdef DEBUG
                        Serial.print("add device address: ");
                        Serial.println(StringDeviceAddress);
                    #endif
                    JsonObject deviceData=(*this->_config)["devices"][StringDeviceAddress].add<JsonObject>();
                    #ifdef DEBUG_JSON
                        serializeJson(this->_config["devices"],Serial);
                        Serial.println(" ADD DEVICES");
                    #endif
                    deviceData["deviceAddress"]=StringDeviceAddress;
                    deviceData["deviceID"]=ds1820id;
                    deviceData["busID"]=busID;
                    deviceData["temperature"]=DEVICE_DISCONNECTED_C;
                    deviceData["canID"]="";
                    deviceData["homeAssistantID"]=StringDeviceAddress;
                    this->setSensorConnected(deviceData);
                    this->setSensorDisable(deviceData);
                    
                }
                #ifdef DEBUG_JSON
                    serializeJson(this->_config["devices"],Serial);
                    Serial.println(" ADD/UPDATE");
                #endif
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
void ds1820HA::readAllSensorsTemp(){
    JsonObject sensors=(*this->_config)["devices"].as<JsonObject>();
    for (JsonPair kv : sensors) {
        // kv.key() ist die Adresse (z.B. "28:ff:d4...")
        JsonObject sensorData = kv.value()[0];
        DeviceAddress deviceAdr;
        if ((!sensorData["enable"].as<bool>()) or (!sensorData["connected"].as<bool>())){continue;}
        this->stringToDeviceAddress(String(kv.key().c_str()) ,deviceAdr);
        #ifdef DEBUG
            Serial.print("read sensor: ");
            Serial.print(kv.key().c_str());
            Serial.print(" ,busid:");
            Serial.println(sensorData["busID"].as<int>());
        #endif
        this->SensorsBus[sensorData["busID"].as<int>()].requestTemperaturesByAddress(deviceAdr);
        float temperatur=round(this->SensorsBus[sensorData["busID"].as<int>()].getTempC(deviceAdr)* 10.0) / 10.0;
        if (temperatur==DEVICE_DISCONNECTED_C){
            #ifdef DEBUG
                Serial.println("can not read temperatur,device disconnected");
            #endif
            this->setSensorNotConnected(sensorData);
        }else{
            sensorData["temperature"]=temperatur;
            #ifdef DEBUG
                Serial.print("read temperature: ");
                Serial.println(sensorData["temperature"].as<float>());
            #endif
        }
    }    
}
void ds1820HA::loop(){
    if (millis() - this->lastSensorInterval >= this->sensorInterval){        // check evry 1s
        this->lastSensorInterval =millis();
        #ifdef DEBUG_JSON
            Serial.println("check for new devices");
            serializeJson(this->_config["devices"],Serial);
            Serial.println(" START");
        #endif
        this->setAllSensorNotConnected();
        // update all sensors
        for (uint busID=0;busID<ONE_WIRE_BUS_COUNT;busID++){
            this->SensorsBus[busID].begin(); // begin().. find new devices an refresh the device counts.
            this->updateSensors(busID);            
        }
        this->readAllSensorsTemp();
    }
}   