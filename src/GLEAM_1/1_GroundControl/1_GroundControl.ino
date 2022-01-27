#include <Arduino.h>
#include <Printers.h>
#include <Wire.h>
#include <SPI.h>
#include <string.h>
#include <XBee.h>
#include <stdint.h>
#include <Printers.h>
#include "2_serializer.h" //our own serializer utility

//#include <SoftwareSerial.h>
//#include <stdlib.h>
//#include <string.h>

//SoftwareSerial nss(18,19);

/* Xbee3 Configuration
   CE (Device Role) = Form Network[0]
   ID (Extended PAN ID) = 1337
   NJ (Node Join Time) = FF
   AP (API Enable) = API Mode With Escapes [2]
   BD (UART Baud Rate) = 9600
 
 Note that some code will use "AT" commands that can change these settings, causing
 the devices to not work correctly with each other.
*/

XBeeWithCallbacks xbee;

Sensors sensors;

XBeeAddress64 MajorTomMAC = XBeeAddress64(0x0013A20041BE7661);
XBeeAddress64 GroundControlMAC = XBeeAddress64(0x0013A20041C0275F);

#include "4_xbee.h"



void setup() {
  Serial.begin(115200);

  xbeeSetup();

  Serial.println("SETUP STAGE 2: Waiting for Xbee Initialization");
  delay(500); //wait for xbee to initialize 0.5 seconds

}

void loop() {
  // Continuously let xbee read packets and call callbacks.
  xbee.loop();



  delay(100);
}
