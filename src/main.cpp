#include "Game.hpp"
#include "Pins.hpp"
#include <Wire.h>
#include <SPI.h>

Game game;

void setup() {
    Serial.begin(115200);             // Serielle Kommunikation starten
    game.initGame();
    DEBUG_PRINTLN("start");

}

void loop() {
    game.loopGame();
}

