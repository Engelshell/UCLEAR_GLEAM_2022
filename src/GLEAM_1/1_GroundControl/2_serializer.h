#ifndef LIB_CUSTOM_SERIAL_H
#define LIB_CUSTOM_SERIAL_H

#include <MsgPack.h>


// input to msgpack
int i = 123;
float f = 1.23;
MsgPack::str_t s = "str"; // std::string or String
MsgPack::arr_t<int> v {1, 2, 3}; // std::vector or arx::vector
MsgPack::map_t<String, float> m {{"one", 1.1}, {"two", 2.2}, {"three", 3.3}}; // std::map or arx::map

// output from msgpack
int ri;
float rf;
MsgPack::str_t rs;
MsgPack::arr_t<int> rv;
MsgPack::map_t<String, float> rm;

/** Custom Data Serializer

A naieve method of serializing data by using structs+unions.
Intended to be used with Xbee3 data communications to efficiently send and receive
serialized data without a slow and difficult to include library.
---This is unsafe and should not be used for any context that depends on arbitrary user input.



*/


struct SensorsC {
    int i;
    float f;
    MsgPack::str_t s;
    MSGPACK_DEFINE(i, f, s); // -> [i, f, s]
};



SensorsC sensorc;

MsgPack::Packer packer;



void test() {
    packer.serialize(sensorc);
    const uint8_t * serialdata = packer.data();
    // -> packer.serialize(i, f, s, arr_size_t(3), c.i, c.f, c.s)



    int ii;
    float ff;
    MsgPack::str_t ss;
    CustomClass cc;

    MsgPack::Unpacker unpacker;
    unpacker.feed(packer.data(), packer.size());
    unpacker.deserialize(ii, ff, ss, cc);
}

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
    int key1; BNO055 bno055; 
    int key2; CCS811 ccs811;
    int key3; BMP085 bmp085;
    uint32_t humidity;
    MsgPack::str_t s;
    MSGPACK_DEFINE_MAP(key1, bno055);
    //MSGPACK_DEFINE(code, bno055, ccs811, bmp085, humidity); // -> [i, f, s]
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

//returns -1 on error
uint8_t custom_deserialize(uint8_t * inputBuffer, uint8_t size, Sensors * outSensors) {
    if(size != sizeof(Sensors)) {
        return -1;
    }
    //change from 8 bit to 32 bit representation, same data.
    uint32_t * data32 = (uint32_t *)inputBuffer;
    outSensors->bno055.temperature = data32[0];
    outSensors->bno055.acceleration.x = (float)data32[1]; //floats are 32-bit as well
    outSensors->bno055.acceleration.y = (float)data32[2];
    outSensors->bno055.acceleration.z = (float)data32[3];
    //Serial.println( outSensors->bno055.temperature);
   // Serial.println( outSensors->bno055.acceleration.x);

    //outSensors->bno055.temperature = data32[0];
    //outSensors->BNO055.acceleration = Vec3(data32[1], data32[2], data32[3]);
    //outSensors->BNO055.orientation =  Vec3(data32[4], data32[5], data32[6]);


}

#endif