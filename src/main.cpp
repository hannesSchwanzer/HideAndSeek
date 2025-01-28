#include "Game.hpp"
#include "Pins.hpp"
#include <Wire.h>
#include <SPI.h>
Game game;

void setup() {
    Serial.begin(115200);             // Serielle Kommunikation starten
    game.initGame();
    Serial.println("start");

}

bool buttonPressed(int pin) { return digitalRead(pin) == LOW; }


void loop() {
    game.loopGame();
}

