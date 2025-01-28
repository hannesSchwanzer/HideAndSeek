#pragma once 

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <Arduino.h>
#include "Player.hpp"

// Display-Spezifikationen
#include "configValues.hpp"

class Display {
  
  private:
    Adafruit_ST7735 tft;
    float ownLat;
    float ownLon;

    // Private Methoden
    float haversine(float lat1, float lon1, float lat2, float lon2);
    void transformAndRotate(float lat, float lon, float ownLat, float ownLon, int &x, int &y, byte currentAzimuth);

  public:
    // Konstruktor
    Display();

    // Öffentliche Methoden
    void displaySetup();
    void drawMap(Player players[], Player ownPlayer, byte otherPlayerCount, byte azimuth);
    void drawStartScreen();
    void drawWaitingScreen(bool isHost, byte playerCount);
    void resetDisplay();

};
