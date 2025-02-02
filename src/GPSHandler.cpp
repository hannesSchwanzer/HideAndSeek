#include "GPSHandler.hpp"
#include <Arduino.h>
#include <HardwareSerial.h>

GPSHandler::GPSHandler() : gpsSerial(Serial1) {  // Automatically choose Serial1 (or another port)
        // No need to pass serial port in the constructor
}

void GPSHandler::setup() {
    gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    compass.init();
    calibrateCompass();
    Serial.println("GPS & Kompass initialisiert.");
}

void GPSHandler::calibrateCompass() {
    compass.setCalibration(-1537, 1266, -1961, 958, -1342, 1492);
}

// GPS-Daten abrufen und in die Position-Struktur speichern
bool GPSHandler::readLocation(Position& position) {
    while (gpsSerial.available() > 0) {
        if (gps.encode(gpsSerial.read())) {
            if (gps.location.isValid()) {
                position.lat = gps.location.lat();
                position.lon = gps.location.lng();
                return true;
            }
        }
    }
    return false;
}

// Kompassrichtung abrufen
int GPSHandler::getCompassHeading() {
    compass.read();
    currentAzimuth = compass.getAzimuth();
    return currentAzimuth;
}

// Kompassrichtung als Text
void GPSHandler::getCompassDirection(char* direction) {
    int azimuth = compass.getAzimuth();
    compass.getDirection(direction, azimuth);
}
