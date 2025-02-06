#include "Compass.hpp"

Compass::Compass()
    : xMin(32767), xMax(-32768), yMin(32767), yMax(-32768),
      zMin(32767), zMax(-32768), xOffset(0), yOffset(0), zOffset(0), azimuth(0) {}

void Compass::setup() {
    compass.init();
    delay(1000);
    compass.calibrate();
}

float Compass::getAzimuth() {
    compass.read();
    int x = compass.getX();
    int y = compass.getY();
    int z = compass.getZ();
    

    // Min/Max-Werte aktualisieren
    xMin = min(x, xMin);
    xMax = max(x, xMax);
    yMin = min(y, yMin);
    yMax = max(y, yMax);
    zMin = min(z, zMin);
    zMax = max(z, zMax);

    // Offsets berechnen
    xOffset = (xMin + xMax) / 2;
    yOffset = (yMin + yMax) / 2;
    zOffset = (zMin + zMax) / 2;

    // Korrigierte Werte berechnen
    int xCorrected = x - xOffset;
    int yCorrected = y - yOffset;
    int zCorrected = z - zOffset;


        // Azimut-Winkel berechnen
    
    azimuth = atan2((float)yCorrected, (float)xCorrected) * 180.0 / PI;
    Serial.print("Azimuth -- "); Serial.print(azimuth);
    if (azimuth < 0) azimuth += 360;

    return azimuth;
}

int Compass::getA() {
    compass.read();
    return compass.getAzimuth();
}
