


//no includes, this file can be imagined to be a continuation of main.ino

void sensorsSetup() {

  //When deploying to a live environment it may be possible that a sensor is disconnected,
  //But we can still allow the rest of the sensors to work
  sensor_BNO055.begin(sensor_BNO055_mode)
    ? hasSensorBNO055 = true : Serial.println("ERROR: Could not find BNO055 Sensor.");

  sensor_CCS811.begin() 
    ? hasSensorCCS811 = true : Serial.println("ERROR: Could not find CCS811 Sensor."); 

  sensor_BMP085.begin(sensor_BMP085_mode) 
    ? hasSensorBMP085 = true : Serial.println("ERROR: Could not find BMP085 Sensor.");
  

  Serial.println(F("SETUP STAGE 2: Sensor Setup Completed"));
}


//std::vector<float>[3] to our Vec3 struct
//Can probably just copy memory if perf is an issue
void mapValues(Vec3 * vector, sensors_vec_t * vec) {
  vector->x = vec->x; vector->y = vec->y; vector->x = vec->x;
};


void doSensors() {
 

  if(hasSensorBNO055) {
    sensors_event_t orientationData , angVelocityData , linearAccelData, magnetometerData, accelerometerData, gravityData;

      //get IMU sensor data
      sensor_BNO055.getEvent(&orientationData,   Adafruit_BNO055::VECTOR_EULER);
      sensor_BNO055.getEvent(&angVelocityData,   Adafruit_BNO055::VECTOR_GYROSCOPE);
      sensor_BNO055.getEvent(&linearAccelData,   Adafruit_BNO055::VECTOR_LINEARACCEL);
      sensor_BNO055.getEvent(&magnetometerData,  Adafruit_BNO055::VECTOR_MAGNETOMETER);
      sensor_BNO055.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
      sensor_BNO055.getEvent(&gravityData,       Adafruit_BNO055::VECTOR_GRAVITY);
      //get data from sensors_event_t type and put into our Sensors struct
      mapValues(&sensors.bno055.orientation,  &orientationData.orientation);
      mapValues(&sensors.bno055.angvelocity,  &angVelocityData.gyro);
      mapValues(&sensors.bno055.linearAccel,  &linearAccelData.acceleration);
      mapValues(&sensors.bno055.magnetometer, &magnetometerData.magnetic);
      mapValues(&sensors.bno055.acceleration, &accelerometerData.acceleration);
      mapValues(&sensors.bno055.gravity,      &gravityData.acceleration);
      //Get temperature
      sensors.bno055.temperature = sensor_BNO055.getTemp();
    
  }
  
  
  if(hasSensorCCS811) {
      //Give sensor temp and humidity data from another sensor
      if(hasSensorBNO055) {
        //TODO: Get humidity, change 0
        sensor_CCS811.setEnvironmentalData(0, (float)sensor_BNO055.getTemp());
      }
      //if sensor has data
      if(sensor_CCS811.available()) {
        //read data
        if(!sensor_CCS811.readData()) {
          sensors.ccs811.co2 = sensor_CCS811.geteCO2();
          sensors.ccs811.ppm_tvoc = sensor_CCS811.getTVOC();
        } else {
          //error, turn off reading from sensor
          hasSensorCCS811 = false;
          Serial.println("ERROR: Sensor CCS811 readData returned false.");
        }
      }
  }

  if(hasSensorBMP085) {
    sensors.bmp085.pressure = sensor_BMP085.readPressure();
    sensors.bmp085.altitude = sensor_BMP085.readAltitude();
  }
  

  
}
