#pragma once

#include <TinyGPS++.h>
#include <QMC5883LCompass.h>
#include "Globals.hpp"  // Enthält die Position-Struktur
#include "Pins.hpp"
#include <HardwareSerial.h>

class GPSHandler {
private:
    TinyGPSPlus gps;
    HardwareSerial& gpsSerial; // Reference to HardwareSerial
    QMC5883LCompass compass;
    int currentAzimuth = 0;

public:
    GPSHandler();
    void setup();
    bool readLocation(Position& position);
    void calibrateCompass();
    int getCompassHeading();
    void getCompassDirection(char* direction);
};
