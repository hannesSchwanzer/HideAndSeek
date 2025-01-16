#include "Display.hpp"
#include <QMC5883LCompass.h>
#include "Pins.hpp"
#include <Wire.h>
#include <SPI.h>
#include "Communication.hpp"
#include "GPSHandler.cpp"


#define MODE 1
// 0: Receive; 1: send
 byte address = 0x01;
Communication communication;
float ownLat = 50.098092;
float ownLon = 8.215985;
byte currentAzimuth = 0;
Display display(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCK, TFT_RST, ownLat, ownLon);


// HardwareSerial für GPS
HardwareSerial gpsSerial(1); // RX=17, TX=23
// GPSHandler-Objekt erstellen
GPSHandler gpsHandler(&gpsSerial);
unsigned long previousGPSMillis = 0;
unsigned long previousCompassMillis = 0;

const unsigned long gpsInterval = 10000;     // GPS-Intervall: 10 Sekunden
const unsigned long compassInterval = 2000;  // Kompass-Intervall: 5 Sekunden



QMC5883LCompass compass;

void setup() {
  Serial.begin(115200);
  display.displaySetup();
  compass.init();
  compass.calibrate();
  // GPSHandler initialisieren
  gpsHandler.begin();

  // Kompass initialisieren und kalibrieren
  gpsHandler.calibrateCompass();



  float otherCoords[][2] = 
  {
    {50.099248, 8.214117}, 
    {50.095493, 8.262105}, 
    {50.104345,  8.23314},
    {50.104192, 8.145831}
  };
  display.drawMap(otherCoords, 4, 0);
  communication.setup();
}
void loopDisplay(){
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
}
void loopCommunication(){

#if MODE == 0
  while (communication.hasMessage()) {
    Serial.println("New message");
    LoRaMessage message;
    if (communication.getNextMessage(message)) {
      communication.printMessage(message);
    } else {
      Serial.println("Error retrieving message from queue");
    }
  }
#elif MODE == 1
  communication.sendGPSData(4,2);
#endif
}


void loop() {
  //loopCommunication();
  loopDisplay();
  delay(100);
      unsigned long currentMillis = millis();

  // GPS-Daten alle 10 Sekunden auslesen
    if (currentMillis - previousGPSMillis >= gpsInterval) {
        previousGPSMillis = currentMillis;

        // Latitude und Longitude mit den neuen Methoden ausgeben
        double latitude = gpsHandler.getLatitude();
        double longitude = gpsHandler.getLongitude();

        Serial.println("GPS-Daten:");
        Serial.print("Latitude: ");
        Serial.println(latitude, 6);
        Serial.print("Longitude: ");
        Serial.println(longitude, 6);

        if (latitude == 0.0 && longitude == 0.0) {
            Serial.println("Keine gültigen GPS-Daten verfügbar.");
        }
    }

       // Kompassdaten alle 5 Sekunden auslesen
    if (currentMillis - previousCompassMillis >= compassInterval) {
        previousCompassMillis = currentMillis;

        // Kompassrichtung als Azimut holen
        float heading = gpsHandler.getCompassHeading();
        Serial.print("Kompassrichtung: ");
        Serial.print(heading);  // Azimut in Grad ausgeben
        Serial.println("°");

        // Kompassrichtung als Text (z.B. N, NE, S, etc.)
        char direction[3];
        gpsHandler.getCompassDirection(direction);
        Serial.print("Kompassrichtung (Text): ");
        Serial.println(direction);
    }
}

