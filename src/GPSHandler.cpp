#include "GPSHandler.hpp"
#include "Globals.hpp"
#include <Arduino.h>
#include <HardwareSerial.h>
#include "configValues.hpp"

GPSHandler::GPSHandler() : gpsSerial(Serial1) {  // Automatically choose Serial1 (or another port)
        // No need to pass serial port in the constructor
}

bool GPSHandler::setup() {
    gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

    unsigned long startTime = millis();
    Position tempPosition;

    // Wait until a valid GPS signal is found or timeout occurs
    while (true) {  // 2-minute timeout
        if (readLocation(tempPosition)) {
            DEBUG_PRINTLN("GPS-Signal gefunden!");
            return true;  // Exit setup when signal is found
        }
        delay(500);  // Reduce CPU usage while waiting
    }

    DEBUG_PRINTLN("Kein GPS-Signal gefunden!");
    return false;
}


// GPS-Daten abrufen und in die Position-Struktur speichern
bool GPSHandler::readLocation(Position& position) {
    // position = {50.098092, 8.215985};
    // position = {50.099248, 8.214117};
    // position = {50.095493, 8.262105};
    // return true;
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

