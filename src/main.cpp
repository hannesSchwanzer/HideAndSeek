#include <Arduino.h>
#include "display.hpp"

// #include <TinyGPS++.h>
// #include <HardwareSerial.h>
//
// TinyGPSPlus gps;
// HardwareSerial gpsSerial(1);
//
// void setup() {
//   Serial.begin(115200);  // Monitor baud rate
//   gpsSerial.begin(9600, SERIAL_8N1, 12, 13);  // RX=16, TX=17
//   Serial.println("GPS Test Start");
// }
//
// void loop() {
//   while (gpsSerial.available() > 0) {
//     char c = gpsSerial.read();
//     if (gps.encode(c)) {
//       if (gps.location.isUpdated()) {
//         Serial.print("Latitude: "); Serial.println(gps.location.lat(), 6);
//         Serial.print("Longitude: "); Serial.println(gps.location.lng(), 6);
//       }
//     }
//   }
//   delay(1000);
// }

void onReceive(int packetSize);

void setup() {
  Serial.begin(115200);
  displaySetup();

}
