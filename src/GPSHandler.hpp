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
    int currentAzimuth = 0;

public:
    GPSHandler();
    bool setup();
    bool readLocation(Position& position);
};
