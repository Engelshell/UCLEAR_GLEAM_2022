#ifndef LIB_CUSTOM_SERIAL_H
#define LIB_CUSTOM_SERIAL_H
/** Custom Data Serializer

A naieve method of serializing data by using structs+unions.
Intended to be used with Xbee3 data communications to efficiently send and receive
serialized data without a slow and difficult to include library.
---This is unsafe and should not be used for any context that depends on arbitrary user input.



*/


//Any modification of this will require modification of the deserializer function
struct Vec3 {
    float x;
    float y;
    float z;
};
struct BNO055 {
    uint32_t temperature; //pad to 32bit //4
    Vec3 acceleration; //12
    Vec3 orientation; //12
    Vec3 angvelocity; //12 
    Vec3 linearAccel; //12
    Vec3 magnetometer; //12
    Vec3 gravity; //12
}; //76
struct CCS811 {
    uint32_t co2; //4
    uint32_t ppm_tvoc; //4
};
struct BMP085 {
    int32_t pressure;
    float altitude;
};
union Sensors {
    int32_t code = 0x9713; //unique code identifying this data structure
    BNO055 bno055;
    CCS811 ccs811;
    BMP085 bmp085;
    uint32_t humidity;
};

struct servo {
    int32_t direction; //1 for forward 2 for backward
    int32_t speed; //1000 for max speed
    int32_t duration; //how long the operation lasts, in milliseconds
};
union receiveCommand {
    int32_t code = 0x1009; //unique code identifying this data structure
    servo cameraTilt;
};

//not needed
/*uint8_t custom_serialize(Sensors sensors, uint8_t * outputBuffer) {
    outputBuffer = (uint8_t *)&sensors;
}*/

uint8_t custom_deserialize(uint8_t * inputBuffer, Sensors * outSensors) {
    //change from 8 bit to 32 bit representation, same data.
    uint32_t * data32 = (uint32_t *)inputBuffer;\
    outSensors->bno055.temperature = data32[0];
    //outSensors->BNO055.acceleration = Vec3(data32[1], data32[2], data32[3]);
    //outSensors->BNO055.orientation =  Vec3(data32[4], data32[5], data32[6]);


}

#endif