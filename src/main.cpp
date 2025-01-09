#include "display.hpp"
#include <QMC5883LCompass.h>
#include "pins.hpp"
#include <Wire.h>
#include <SPI.h>
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
}

void loop() {
  float otherCoords[][2] = 
  {
    {50.099248, 8.214117}, 
    {50.095493, 8.262105}, 
    {50.104345,  8.23314},
    {50.104192, 8.145831}
  };
  
  

  compass.read();
  currentAzimuth = compass.getAzimuth();
  printf("Azimuth: %d\n", currentAzimuth);

  
  display.drawMap(otherCoords, 4, currentAzimuth);
  delay(100);
}
