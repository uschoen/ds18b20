#ifndef DS1820HA_H
    #define DS1820HA_H
    #include <Arduino.h>
    #include "OneWire.h"
    #include "ArduinoJson.h"
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
    #ifndef ONE_WIRE_PORT
        #define ONE_WIRE_PORT {6,7}
    #endif
    #ifndef ONE_WIRE_BUS_COUNT
        #define ONE_WIRE_BUS_COUNT 2
    #endif
    #ifndef DS18B20_MAX_SENSORS
        #define DS18B20_MAX_SENSORS 8 // max DS18B20 sensors to handle
    #endif //DS18B20_MAX_SENSORS
    #ifdef DS18B20_UPDATE_TIME
        #define DS18B20_UPDATE_TIME 1000
    #endif

    class ds1820HA{
        private: 
            OneWire oneWireBus[ONE_WIRE_BUS_COUNT];
            int OneWirePins[ONE_WIRE_BUS_COUNT]=ONE_WIRE_PORT;
            DallasTemperature SensorsBus[ONE_WIRE_BUS_COUNT];
            JsonDocument _config;
            ulong sensorInterval = DS1820_UPDATE_TIME;
            ulong lastSensorInterval =0;
            void stringToDeviceAddress(String stringAddress,DeviceAddress& tempDeviceAdress);
            String deviceAddresToString(DeviceAddress deviceAddress);
            void setAllSensorNoValid();
            void setSensorNoValid(String sensorAdress);
            void setSensorValid(String sensorAddress);
            void updateSensors(int busID);
        public:
            ds1820HA();
            void begin(JsonDocument& CFG);
            void begin();
            /*
            set the interval to read the temperature
            Arguments:
                uint16_t updateTime in milli second;
            */
            void setSensorInterval(ulong updateTime);
            void loop();
    };
#endif //DS1820HA_H