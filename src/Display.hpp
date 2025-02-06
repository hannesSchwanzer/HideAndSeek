#pragma once 

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h> 
#include <math.h>
#include "Pins.hpp"
#include "Display.hpp"
#include "Player.hpp"

// Display-Spezifikationen
#include "configValues.hpp"

class Display {
  
  private:
    Adafruit_ST7735 tft;
    Position fieldCenter = {0.0, 0.0};

    // Private Methoden
    float haversine(float lat1, float lon1, float lat2, float lon2);
    void transformAndRotate(float lat, float lon, float ownLat, float ownLon, int &x, int &y, int currentAzimuth);

  public:
    // Konstruktor
    Display();


    // Öffentliche Methoden
    void displaySetup();
    void drawMap(Player players[], Player ownPlayer, byte otherPlayerCount, int azimuth, unsigned long gameTime);
    void drawStartScreen();
    void drawWaitingScreen(bool isHost, byte playerCount);
    void resetDisplay();
    void drawLoosingScreen();
    void drawWinningScreen();
    void setCenter(Position center);
    void drawString(const char* str);
    void drawMapExtras(const char* str, int time);
};
