#pragma once 

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Arduino.h>

// Display-Spezifikationen
#include "configValues.hpp"

class Display {
  
  private:
    Adafruit_ST7735 tft;
    float ownLat;
    float ownLon;

    // Private Methoden
    float haversine(float lat1, float lon1, float lat2, float lon2);
    void transformAndRotate(float lat, float lon, int &x, int &y, byte currentAzimuth);

  public:
    // Konstruktor
    Display(uint8_t cs, uint8_t dc, uint8_t mosi, uint8_t sck, uint8_t rst, float lat, float lon);

    // Öffentliche Methoden
    void displaySetup();
    void drawMap(float coords[][2], int numCoords, byte azimuth);
    void setOwnPosition(float lat, float lon);
    void getOwnPosition(float &lat, float &lon) const;
};
