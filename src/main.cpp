#include "display.hpp"
#include <QMC5883LCompass.h>
#include "pins.hpp"
#include <Wire.h>
#include <SPI.h>
#include "Communication.hpp"

#define MODE 1
// 0: Receive; 1: send
 byte address = 0x01;
 Communication communication(address);

float ownLat = 50.098092;
float ownLon = 8.215985;
byte currentAzimuth = 0;
Display display(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, ownLat, ownLon);

QMC5883LCompass compass;

void setup() {
  Serial.begin(115200);
  display.displaySetup();
  compass.init();
  compass.calibrate();



  float otherCoords[][2] = 
  {
    {50.099248, 8.214117}, 
    {50.095493, 8.262105}, 
    {50.104345,  8.23314},
    {50.104192, 8.145831}
  };
  display.drawMap(otherCoords, 4, 0);
  communication.setup();


  
}

#if MODE == 0
void loop() {
  float otherCoords[][2] = 
  {
    {50.099248, 8.214117}, 
    {50.095493, 8.262105}, 
    {50.104345,  8.23314},
    {50.104192, 8.145831}
  };
  while (communication.hasMessage()) {
    Serial.println("New message");
    LoRaMessage message;
    if (communication.getNextMessage(message)) {
      communication.printMessage(message);
    } else {
      Serial.println("Error retrieving message from queue");
    }
  }
  

  compass.read();
  currentAzimuth = compass.getAzimuth();
  printf("Azimuth: %d\n", currentAzimuth);

  
  display.drawMap(otherCoords, 4, currentAzimuth);

  delay(10000);
}
#elif MODE == 1
void loop() {
  communication.sendGPSData(4,2);
  delay(1000);
}
#endif

