//Pin for ESP32
#define TFT_CS         25  //case select connect to pin 5
#define TFT_RST        13 //reset connect to pin 15
#define TFT_DC         12 //AO connect to pin 32  (not sure that this is really used)  try pin 25
#define TFT_MOSI       21 //Data = SDA connect to pin 23
#define TFT_SCLK       22 //Clock = SCK connect to pin 18

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <math.h>

// For ST7735-based displays, we will use this call
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

float ownLat = 50.098422;
float ownLon = 8.216903;

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 160

#define MAX_DISTANCE 1000


float haversine(float lat1, float lon1, float lat2, float lon2) {
  float R = 6371000; // Erdradius in Metern
  float dLat = radians(lat2 - lat1);
  float dLon = radians(lon2 - lon1);
  float a = sin(dLat / 2) * sin(dLat / 2) +
            cos(radians(lat1)) * cos(radians(lat2)) *
            sin(dLon / 2) * sin(dLon / 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return R * c;
}


void transformToScreen(float lat, float lon, int &x, int &y) {
  float distance = haversine(ownLat, ownLon, lat, lon);
  float angle = atan2(lat - ownLat, lon - ownLon); // Richtung berechnen

  // Begrenze Distanz auf MAX_DISTANCE
  if (distance > MAX_DISTANCE) distance = MAX_DISTANCE;

  // Normalisiere auf Bildschirmkoordinaten
  float normDistance = (distance / MAX_DISTANCE) * (SCREEN_WIDTH / 2 - 10); // Randabstand
  x = SCREEN_WIDTH / 2 + normDistance * cos(angle);
  y = SCREEN_HEIGHT / 2 - normDistance * sin(angle); // y-Achse invertiert
}

void drawMap(float coords[][2], int numCoords) {
  // Eigene Position zeichnen (Pfeil in der Mitte)
  tft.fillTriangle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 5,
                   SCREEN_WIDTH / 2 - 5, SCREEN_HEIGHT / 2 + 5,
                   SCREEN_WIDTH / 2 + 5, SCREEN_HEIGHT / 2 + 5,
                   ST7735_RED);
  
  // Andere Koordinaten zeichnen
  for (int i = 0; i < numCoords; i++) {
    int x, y;
    transformToScreen(coords[i][0], coords[i][1], x, y);
    tft.fillCircle(x, y, 3, ST7735_GREEN); // Punkte darstellen
  }
}
void displaySetup(){
    tft.initR(INITR_BLACKTAB); // Display initialisieren
    tft.fillScreen(ST7735_BLACK);

    float otherCoords[][2] = {
    {50.099248, 8.214117}, 
    {50.09549318153879, 8.262105363437584}, 
    {50.104345996335,  8.233146693343492},
    {50.10419249325381, 8.145831915331785}

  };
    drawMap(otherCoords, 4);
}