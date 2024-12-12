//Pin for ESP32
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <math.h>
#include <Wire.h>
#include "pins.hpp"

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 160
#define MAX_DISTANCE 1000

class Display{
  Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
  float ownLat = 47.997186;
  float ownLon = 8.216903;
  
  public:
      
      Display()
      {
        ownLat = 47.997186;
      }


      void displaySetup();
      void drawMap(float coords[][2], int numCoords, byte azimuth);
  
  private:    
      void transformAndRotate(float lat, float lon, int &x, int &y, byte currentAzimuth);
      float haversine(float lat1, float lon1, float lat2, float lon2);
      
};



float Display::haversine(float lat1, float lon1, float lat2, float lon2) {
  float R = 6371000; // Erdradius in Metern
  float dLat = radians(lat2 - lat1);
  float dLon = radians(lon2 - lon1);
  float a = sin(dLat / 2) * sin(dLat / 2) +
            cos(radians(lat1)) * cos(radians(lat2)) *
            sin(dLon / 2) * sin(dLon / 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return R * c;
}


void Display::transformAndRotate(float lat, float lon, int &x, int &y, byte currentAzimuth) {
    float distance = haversine(ownLat, ownLon, lat, lon);
    float angle = atan2(lat - ownLat, lon - ownLon); // Richtung berechnen
    // Kombiniere die Rotationen
    float azimuthRad = radians(currentAzimuth);
    float totalAngle = angle + azimuthRad;
    if (distance > MAX_DISTANCE) distance = MAX_DISTANCE;

    // Normalisiere Distanz auf den Bildschirmradius (vorläufig)
    float normDistance = (distance / MAX_DISTANCE) * (SCREEN_WIDTH / 2 - 1); // Randabstand

    // Transformiere und rotiere
    float rotatedX = normDistance * cos(totalAngle) - normDistance * sin(totalAngle);
    float rotatedY = normDistance * sin(totalAngle) + normDistance * cos(totalAngle);

    // Verschiebe ins Bildschirmkoordinatensystem
    x = SCREEN_WIDTH / 2 + rotatedX;
    y = SCREEN_HEIGHT / 2 - rotatedY;

    // Begrenze die Pixelkoordinaten, falls sie außerhalb des sichtbaren Bereichs liegen
    x = max(0, min(SCREEN_WIDTH - 1, x));
    y = max(0, min(SCREEN_HEIGHT - 1, y));
    printf("x: %d, y: %d\n", x, y);
}



void Display::drawMap(float coords[][2], int numCoords, byte azimuth) {
  // Eigene Position zeichnen (Pfeil in der Mitte)
  tft.fillScreen(ST7735_BLACK);

  tft.fillTriangle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 5,
                   SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2 + 5,
                   SCREEN_WIDTH / 2 + 5, SCREEN_HEIGHT / 2 + 5,
                   ST7735_RED);
  
  // Andere Koordinaten zeichnen
  for (int i = 0; i < numCoords; i++) {
    int x, y;
    transformAndRotate(coords[i][0], coords[i][1], x, y, azimuth);
    tft.fillCircle(x, y, 3, ST7735_GREEN); // Punkte darstellen
  }

  printf("Map drawn\n");
}

void Display::displaySetup(){
    tft.initR(INITR_BLACKTAB);
     // Display initialisieren
}