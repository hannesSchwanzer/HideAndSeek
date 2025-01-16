#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h> 
#include <math.h>
#include "Pins.hpp"
#include "Display.hpp"

Display::Display(uint8_t cs, uint8_t dc, uint8_t sda, uint8_t sck,  uint8_t rst, float lat, float lon) 
  : tft(cs, dc, sda, sck, rst){
    ownLat = lat;
    ownLon = lon;
  }

// Getter und Setter für Position
void Display::setOwnPosition(float lat, float lon) {
    ownLat = lat;
    ownLon = lon;
}

void Display::getOwnPosition(float &lat, float &lon) const {
    lat = ownLat;
    lon = ownLon;
}

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
    printf("Azimuth - rad: %d\n", currentAzimuth);
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
    //printf("x: %d, y: %d\n", x, y);
}



void Display::drawMap(float coords[][2], int numCoords, byte azimuth) {
  // Eigene Position zeichnen (Pfeil in der Mitte)
  //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  tft.fillScreen(ST7735_BLACK);

  tft.fillTriangle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 5,
                   SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2 + 5,
                   SCREEN_WIDTH / 2 + 5, SCREEN_HEIGHT / 2 + 5,
                   ST7735_RED);
  printf("Own position drawn\n");
  printf("Own Lat: %f, Own Lon: %f\n", ownLat, ownLon);
  // Andere Koordinaten zeichnen
  for (int i = 0; i < numCoords; i++) {
    int x, y;
    transformAndRotate(coords[i][0], coords[i][1], x, y, azimuth);
    tft.fillCircle(x, y, 3, ST7735_GREEN); // Punkte darstellen
  
  }
  printf("Map drawn\n");
  
  //SPI.endTransaction();

}

void Display::displaySetup(){

    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_BLACK);

     // Display initialisiere
}
