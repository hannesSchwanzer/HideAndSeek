#pragma once

#include <QMC5883LCompass.h>

class Compass {
public:
    Compass();
    void setup();
    float getAzimuth();
    int getA();

private:
    QMC5883LCompass compass;
    int xMin, xMax, yMin, yMax, zMin, zMax;
    int xOffset, yOffset, zOffset;
    float azimuth;
};
