
//#include <SoftwareSerial.h>
#include <Arduino.h>
#include <Printers.h>
#include <Wire.h>
#include <SPI.h>
#include <string.h>
#include <XBee.h> //xbee3
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h> //imu sensor, get library in arduino software
#include <Adafruit_BMP085.h> //get library in arduino software
#include <Adafruit_CCS811.h> //air quality sensor, library in arduino software
#include <utility/imumaths.h> // needed by BNO055
#include "2_serializer.h" //our own serializer utility


/* Xbee3 Configuration
   CE (Device Role) = Join Network[0]
   ID (Extended PAN ID) = 1337
   NJ (Node Join Time) = FF
   AP (API Enable) = API Mode With Escapes [2]
   BD (UART Baud Rate) = 115200
 
 Note that some code will use "AT" commands that can change these settings, causing
 the devices to not work correctly with each other.
*/

/* Teensy 3.5 Configuration
//https://www.pjrc.com/teensy/td_uart.html
Teensey 3.5 has 4 hardware serial ports, named the same as arduinos
Serial is used by USB, Serial1 in the code here is used by our Xbee3


*/

//union with structured data of our sensors, used to serialize so we can send 
//our sensor data wirelessly
Sensors sensors;

//Xbee type but with useful callbacks, saves us from having to create 
//an event system ourselves, see below how events are subscribed and used.
XBeeWithCallbacks xbee;

//You can get the 64-bit MAC of an Xbee with the XCTU software
//Add an 0x infront of it, example: 1234 becomes 0x1234
XBeeAddress64 MajorTomMAC = XBeeAddress64(0x0013A20041BE7661);
XBeeAddress64 GroundControlMAC = XBeeAddress64(0x0013A20041C0275F);

//Each I2C device must have a different address or "ID" from one another.

//BNO055
uint16_t BNO055_SAMPLERATE_DELAY_MS = 100;
//BNO055_ADDRESS_A (0x28)
//BNO055_ID (0xA0)
Adafruit_BNO055 sensor_BNO055 = Adafruit_BNO055();
//https://docs.kubos.com/1.0.0/kubos-core/sensors/bno055.html
//TODO: see if we need to change operating modes, currently default
Adafruit_BNO055::adafruit_bno055_opmode_t sensor_BNO055_mode = 
          Adafruit_BNO055::adafruit_bno055_opmode_t::OPERATION_MODE_CONFIG;

//CCS811_HW_ID_CODE 0x81
//CCS811_ADDRESS (0x5A)
Adafruit_CCS811 sensor_CCS811 = Adafruit_CCS811();

//BMP085_I2CADDR 0x77
Adafruit_BMP085 sensor_BMP085 = Adafruit_BMP085();
uint8_t sensor_BMP085_mode = BMP085_ULTRAHIGHRES;


unsigned long setupTime; //used for benchmarking performance
bool startupError = false;
bool hasSensorBNO055 = false;
bool hasSensorCCS811 = false;
bool hasSensorBMP085 = false;

//has to be down here so above variables are defined
#include "3_sensors.h"
#include "4_xbee.h"

void setup() {
  //README: The pins for Serial1 and others can be different for different boards
  //Start USB serial
  //Teensy 3.5: pins 0 and 1
  Serial.begin(115200);
 
  xbeeSetup();

  Serial.println("SETUP STAGE 2: Waiting for Xbee Initialization");
  delay(500); //wait for xbee to initialize 0.5 seconds

  sensorsSetup();

  Serial.println(F("SETUP STAGE 2: Sensor Setup Completed"));
  setupTime = millis();
}


int amount = 0;
bool oneshot = false;

void loop() {

  //Joe poepell code.
  int time = (millis() - setupTime)/1000;                                     // Time we are converting
  int hr = time/3600;                                                         // Number of hours
  int mins = (time-hr*3600)/60;                                               // Remove the number of hours and calculate the minutes.
  int sec = time-hr*3600-mins*60;                                            // Remove the number of hours and minutes, leaving only seconds.
  //timer = (String(hr) + ":" + String(mins) + ":" + String(sec));             // Converts to HH:MM:SS string

  // Continuously let xbee read packets and call callbacks.
  xbee.loop();

  doSensors();

  //ZBTxRequest tx = ZBTxRequest(GroundControlMAC, dat, (uint8_t)sizeof(dat));
  //ZBTxRequest tx = ZBTxRequest(GroundControlMAC, arr, sizeof(arr));
  ZBTxRequest tx = ZBTxRequest(GroundControlMAC, (uint8_t *) &sensors, sizeof(sensors));
  xbee.send(tx);

  amount += 88;
  
  if(millis() > 10000) {
    if(!oneshot) {
      char buff[50];
      sprintf(buff, "%llu bytes", amount);
      Serial.print(buff);
      Serial.println();
      oneshot = true;
    }
  }
  delay(2000);
}
