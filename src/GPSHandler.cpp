#ifndef GPS_HANDLER_H
#define GPS_HANDLER_H

#include <TinyGPS++.h>
#include <QMC5883LCompass.h>
#include "Globals.hpp"


class GPSHandler {
private:
    TinyGPSPlus gps;                     // GPS-Bibliothek
    HardwareSerial* gpsSerial;           // Serieller Port für GPS
    QMC5883LCompass compass;         // Kompass
    int currentAzimuth = 0;     // Azimut als float


public:
    // Konstruktor
    GPSHandler(HardwareSerial* serial)
        : gpsSerial(serial) {}

    // Initialisierungsmethode
    void begin() {
        gpsSerial->begin(9600);          // GPS-Modul initialisieren

        // Kompass initialisieren
        compass.init();
        compass.calibrate();
        DEBUG_PRINTLN("Kompass initialisiert.");
        
 
    }

    // Methode zum Lesen der GPS-Daten
    bool readLocation(double& latitude, double& longitude) {
        while (gpsSerial->available() > 0) {
            if (gps.encode(gpsSerial->read())) {
                if (gps.location.isValid()) {
                    latitude = gps.location.lat();
                    longitude = gps.location.lng();
                    return true;
                }
            }
        }
        return false;
    }

    // Latitude ausgeben
    double getLatitude() {
        while (gpsSerial->available() > 0) {
            if (gps.encode(gpsSerial->read())) {
                if (gps.location.isValid()) {
                    return gps.location.lat();
                }
            }
        }
        return 0.0;  // Rückgabe von 0.0, falls ungültige Daten vorliegen
    }

    // Longitude ausgeben
    double getLongitude() {
        while (gpsSerial->available() > 0) {
            if (gps.encode(gpsSerial->read())) {
                if (gps.location.isValid()) {
                    return gps.location.lng();
                }
            }
        }
        return 0.0;  // Rückgabe von 0.0, falls ungültige Daten vorliegen
    }

    // Kompass kalibrieren
    void calibrateCompass() {
        // Füge hier die Kalibrierungsdaten ein
        compass.setCalibration(-1537, 1266, -1961, 958, -1342, 1492); 
        DEBUG_PRINTLN("Kompass kalibriert.");
    }

    // Methode zur Berechnung der Kompassrichtung
    int getCompassHeading() {
        compass.read();  // Werte vom Kompass lesen
        currentAzimuth = compass.getAzimuth();  // Azimut (Richtung) abrufen
 

        return currentAzimuth;
    }

    // Methode zur Berechnung der Kompassrichtung als Text (z.B., N, NE, S, etc.)
    void getCompassDirection(char* direction) {
        int azimuth = compass.getAzimuth();  // Azimut (Richtung) abrufen
        compass.getDirection(direction, azimuth);  // Richtungsbezeichner ermitteln
    }
};

#endif
