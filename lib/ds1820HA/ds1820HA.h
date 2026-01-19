#ifndef DS1820HA_H
    #define DS1820HA_H
    #include <Arduino.h>
    #include "OneWire.h"
    #include "DallasTemperature.h"      // https://github.com/milesburton/Arduino-Temperature-Control-Library

    /*
    insert in platformio.ini 
    build_flags = 
        '-D ONE_WIRE_PORT_0=6'
        '-D ONE_WIRE_PORT_1=7'
        '-D DS18B20_MAX_SENSORS=8'
        '-D DS1820_UPDATE_TIME=1000'
        or as config flag or in our
    own configuratione file
    */
    #ifndef ONE_WIRE_PORT_0
        #define ONE_WIRE_PORT_0 6
    #endif
    #ifndef ONE_WIRE_PORT_1
        #define ONE_WIRE_PORT_1 7
    #endif
    #ifndef DS18B20_MAX_SENSORS
        #define DS18B20_MAX_SENSORS 8 // max DS18B20 sensors to handle
    #endif //DS18B20_MAX_SENSORS
    #ifdef DS18B20_UPDATE_TIME
        #define DS18B20_UPDATE_TIME 1000
    #endif

    typedef struct {
        DeviceAddress adr;
        float LastTempC;
        bool valid;
        int can_id;
        char32_t HAdeviceID;
        int busID;
        int deviceID;
    } senorData;


    class ds1820HA{
        private: 
            OneWire oneWire0;         
            OneWire oneWire1; 
            OneWire WBus[2];     
            DallasTemperature SensorBus0;                
            DallasTemperature SensorBus1;
            DallasTemperature SensorsBus[2];
            senorData TemperatorSensors[DS18B20_MAX_SENSORS];
            uint16_t updateTime = DS18B20_UPDATE_TIME;

            void disableSensor(uint8_t SensorID);
            void disableAllSensor();

        public:
            ds1820HA(uint8_t busPin0 = ONE_WIRE_PORT_0, uint8_t busPin1 = ONE_WIRE_PORT_1);
            void begin();
            void begin(uint8_t busPin0, uint8_t busPin1);
            void setUpdateTime(uint16_t updateTime);
            void loop();
    };
#endif //DS1820HA_H