#include <Arduino.h>
#include <SPI.h>
#include "display.hpp"
#include <QMC5883LCompass.h>

QMC5883LCompass compass;
byte currentAzimuth = 0;

void setup() {
  Serial.begin(115200);
  compass.setADDR(0x0D);
  compass.init();
  displaySetup();


  float otherCoords[][2] = {
    {50.099248, 8.214117}, 
    {50.09549318153879, 8.262105363437584}, 
    {50.104345996335,  8.233146693343492},
    {50.10419249325381, 8.145831915331785}
};
  drawMap(otherCoords, 4, 0);

}

void loop() {
  float otherCoords[][2] = {
    {50.099248, 8.214117}, 
    {50.09549318153879, 8.262105363437584}, 
    {50.104345996335,  8.233146693343492},
    {50.10419249325381, 8.145831915331785}
  };
  compass.read();
  currentAzimuth = compass.getAzimuth();

  printf("Azimuth: %d\n", currentAzimuth);
  drawMap(otherCoords, 4, currentAzimuth);
  delay(1000);
}