// #include <Arduino.h>
// #include "display.hpp"

// // #include <TinyGPS++.h>
// // #include <HardwareSerial.h>
// //
// // TinyGPSPlus gps;
// // HardwareSerial gpsSerial(1);
// //
// // void setup() {
// //   Serial.begin(115200);  // Monitor baud rate
// //   gpsSerial.begin(9600, SERIAL_8N1, 12, 13);  // RX=16, TX=17
// //   Serial.println("GPS Test Start");
// // }
// //
// // void loop() {
// //   while (gpsSerial.available() > 0) {
// //     char c = gpsSerial.read();
// //     if (gps.encode(c)) {
// //       if (gps.location.isUpdated()) {
// //         Serial.print("Latitude: "); Serial.println(gps.location.lat(), 6);
// //         Serial.print("Longitude: "); Serial.println(gps.location.lng(), 6);
// //       }
// //     }
// //   }
// //   delay(1000);
// // }

// #include <LoRa.h>
// #include <SPI.h>

// // WIFI_LoRa_32 ports

// // GPIO5  -- SX1278's SCK
// // GPIO19 -- SX1278's MISO
// // GPIO27 -- SX1278's MOSI
// // GPIO18 -- SX1278's CS
// // GPIO14 -- SX1278's RESET
// // GPIO26 -- SX1278's IRQ(Interrupt Request)

// #define SS 18
// #define RST 14
// #define DI0 26
// #define BAND 433E6

// #define SCK 5
// #define MISO 19
// #define MOSI 27

// #define LED_BUILDIN 25

// void onReceive(int packetSize);

// void setup() {
//   Serial.begin(115200);
//   // while (!Serial)
//   //   ; // if just the the basic function, must connect to a computer

//   // SPI.begin(SCK, MISO, MOSI, SS);
//   // LoRa.setPins(SS, RST, DI0);

//   // Serial.println("LoRa Receiver Callback");

//   // if (!LoRa.begin(BAND)) {
//   //   Serial.println("Starting LoRa failed!");
//   //   while (1)
//   //     ;
//   // }

//   // // register the receive callback
//   // LoRa.onReceive(onReceive);

//   // // put the radio into receive mode
//   // LoRa.receive();
//   displaySetup();

// }

// void loop() {
//   // do nothing
// }

// void onReceive(int packetSize) {
//   // received a packet
//   Serial.print("Received packet '");

//   // read packet
//   for (int i = 0; i < packetSize; i++) {
//     Serial.print((char)LoRa.read());
//   }

//   // print RSSI of packet
//   Serial.print("' with RSSI ");
//   Serial.println(LoRa.packetRssi());
// }

// // Sender
// // int counter = 0;
// //
// // void setup() {
// //   pinMode(LED_BUILDIN, OUTPUT); // Send success, LED will bright 1 second
// //
// //   Serial.begin(115200);
// //   while (!Serial)
// //     ; // If just the the basic function, must connect to a computer
// //
// //   SPI.begin(SCK, MISO, MOSI, SS);
// //   LoRa.setPins(SS, RST, DI0);
// //   Serial.println("LoRa Sender");
// //
// //   if (!LoRa.begin(BAND)) {
// //     Serial.println("Starting LoRa failed!");
// //     while (1)
// //       ;
// //   }
// //   Serial.println("LoRa Initial OK!");
// // }
// //
// // void loop() {
// //   Serial.print("Sending packet: ");
// //   Serial.println(counter);
// //
// //   // send packet
// //   LoRa.beginPacket();
// //   LoRa.print("hello ");
// //   LoRa.print(counter);
// //   LoRa.endPacket();
// //
// //   counter++;
// //   digitalWrite(LED_BUILDIN, HIGH); // turn the LED on (HIGH is the voltage level)
// //   delay(1000);            // wait for a second
// //   digitalWrite(LED_BUILDIN, LOW);  // turn the LED off by making the voltage LOW
// //   delay(1000);            // wait for a second
// //
// //   delay(3000);
// // }
