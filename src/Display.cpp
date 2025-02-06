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

void Display::setCenter(Position center) {
  fieldCenter = center;
}

void Display::transformAndRotate(float lat, float lon, float ownLat, float ownLon, int &x, int &y, int currentAzimuth) {
    float distance = haversine(ownLat, ownLon, lat, lon);
    float angle = atan2(lat - ownLat, lon - ownLon); // Richtung berechnen

    float azimuthRad = radians(currentAzimuth);
    float totalAngle = angle + azimuthRad;

    // Begrenze die Distanz auf die maximale Ansichtsdistanz
    if (distance > VIEW_DISTANCE) distance = VIEW_DISTANCE;

    // Normalisiere Distanz auf den sichtbaren Bereich
    float normDistance = (distance / VIEW_DISTANCE) * (SCREEN_WIDTH / 2 - 1); // Randabstand

    // Transformiere und rotiere
    float rotatedX = normDistance * cos(totalAngle) - normDistance * sin(totalAngle);
    float rotatedY = normDistance * sin(totalAngle) + normDistance * cos(totalAngle);

    // Verschiebe ins Bildschirmkoordinatensystem
    x = SCREEN_WIDTH / 2 + rotatedX;
    y = SCREEN_HEIGHT / 2 - rotatedY;

    // Begrenze die Pixelkoordinaten, falls sie außerhalb des sichtbaren Bereichs liegen
    x = max(0, min(SCREEN_WIDTH - 1, x));
    y = max(0, min(SCREEN_HEIGHT - 1, y));
}

void Display::drawMap(Player players[], Player ownPlayer, byte otherPlyaerCount, int azimuth, unsigned long gameTime) {
  // Eigene Position zeichnen (Pfeil in der Mitte)
  //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  tft.fillScreen(ST7735_BLACK);
  //wenn das center nicht gesetzt ist, wird die map nicht gezeichnet
  if (fieldCenter.lat == 0.0 && fieldCenter.lon == 0.0) {
    DEBUG_PRINTF("CENTER NOCH NICHT GEZEICHNET\n");
    return;
  }

  tft.fillTriangle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 5,
                   SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2 + 5,
                   SCREEN_WIDTH / 2 + 5, SCREEN_HEIGHT / 2 + 5,
                   ST7735_RED);
  DEBUG_PRINTF("Own position drawn\n");
  DEBUG_PRINTF("Own Lat: %f, Own Lon: %f\n", ownPlayer.position.lat, ownPlayer.position.lon);

    // **Spielfeld-Kreis zeichnen**
  int circleX, circleY;
  transformAndRotate(fieldCenter.lat, fieldCenter.lon, ownPlayer.position.lat, ownPlayer.position.lon, circleX, circleY, azimuth);

  // Radius auf den Bildschirm skalieren
  float screenRadius = (FIELD_RADIUS / VIEW_DISTANCE) * (SCREEN_WIDTH / 2 - 5);
  // Zeichne roten Kreis (auch wenn außerhalb des Displays)

  unsigned long currentTime = millis();

  // Andere Koordinaten zeichnen
  for (byte i = 0; i < otherPlyaerCount; i++) {
    int x, y;
    transformAndRotate(players[i].position.lat, players[i].position.lon, ownPlayer.position.lat, ownPlayer.position.lon,  x, y, azimuth);
    

    if (players[i].is_hunter && gameTime >= HUNTER_REVEAL) {
      tft.fillCircle(x, y, 3, ST7735_RED); // Punkte darstellen
    } else {
      tft.fillCircle(x, y, 3, ST7735_GREEN); // Punkte darstellen
    }
  
  }
  tft.drawCircle(circleX, circleY, screenRadius, ST7735_BLUE);

  DEBUG_PRINTF("Map drawn\n");
  
}



//draw menue Sceens


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
