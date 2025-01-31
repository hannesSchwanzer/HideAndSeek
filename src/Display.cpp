#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h> 
#include <math.h>
#include "Pins.hpp"
#include "Display.hpp"

Display::Display() 
  : tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST){}

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


void Display::transformAndRotate(float lat, float lon, float ownLat, float ownLon, int &x, int &y, byte currentAzimuth) {
    float distance = haversine(ownLat, ownLon, lat, lon);
    float angle = atan2(lat - ownLat, lon - ownLon); // Richtung berechnen
    // Kombiniere die Rotationen
    float azimuthRad = radians(currentAzimuth);
    DEBUG_PRINTF("Azimuth - rad: %d\n", currentAzimuth);
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
    //DEBUG_PRINTF("x: %d, y: %d\n", x, y);
}



void Display::drawMap(Player players[], Player ownPlayer, byte otherPlyaerCount, byte azimuth) {
  // Eigene Position zeichnen (Pfeil in der Mitte)
  //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  tft.fillScreen(ST7735_BLACK);

  tft.fillTriangle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 5,
                   SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2 + 5,
                   SCREEN_WIDTH / 2 + 5, SCREEN_HEIGHT / 2 + 5,
                   ST7735_RED);
  DEBUG_PRINTF("Own position drawn\n");
  DEBUG_PRINTF("Own Lat: %f, Own Lon: %f\n", ownPlayer.position.lat, ownPlayer.position.lon);
  // Andere Koordinaten zeichnen
  for (byte i = 0; i < otherPlyaerCount; i++) {
    int x, y;
    transformAndRotate(players[i].position.lat, players[i].position.lon, ownPlayer.position.lat, ownPlayer.position.lon,  x, y, azimuth);
    tft.fillCircle(x, y, 3, ST7735_GREEN); // Punkte darstellen
  
  }
  DEBUG_PRINTF("Map drawn\n");
  
}

void Display::drawStartScreen() {
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(20, 30);
  tft.print("A -  HOST GAME");
  tft.setCursor(20, 90);
  tft.print("B - JOIN GAME");
}

void Display::drawWaitingScreen(bool isHost, byte playerCount) {
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(20, 30);
  if (isHost) {
    tft.print("You are the Host");
  }
  tft.setCursor(20, 50);
  tft.print("Current Players:");
  tft.setCursor(20, 70);
  tft.print(playerCount+1, 20);
if (isHost) {
    tft.setCursor(20, 110);
    tft.print("A - Start Game");
  }
}

void Display::drawLoosingScreen() {
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(20, 30);
  tft.print("Du hast Verloren!");
  tft.setCursor(20, 90);
  tft.print("Press a Button to restart");
}

void Display::drawWinningScreen() {  
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(20, 30);
  tft.print("Du hast Gewonnen!");
  tft.setCursor(20, 90);
  tft.print("Press a Button to restart");
}

void Display::resetDisplay(){
  tft.fillScreen(ST7735_BLACK);
}

void Display::displaySetup(){
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_BLACK);

     // Display initialisiere
}
