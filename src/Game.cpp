#include "Game.hpp"
#include "Globals.hpp"
#include "Pins.hpp"
#include "configValues.hpp"
#include "esp32-hal.h"
#include <cstdint>
#include <cstring>

void Game::initGame() {
  display.displaySetup();
  communication.setup();
  pinMode(BUTTON_PIN_1, INPUT_PULLUP); // Mit Pull-Up-Widerstand
  pinMode(BUTTON_PIN_2, INPUT_PULLUP); // Mit Pull-Up-Widerstand
  randomSeed(millis());
  ownPlayer.player_address = random(256);
  state = INIT;
}

void Game::setState(gameState state) {
  this->state = state;

  switch (state) {
  case INIT: {
    // TODO: reset all variables
    break;
  }
  case HOST: {
    otherPlayerCount = 0;
    // startPosition = ; // TODO: replace with call to gps
    break;
  }
  case SEARCH: {
    communication.sendJoiningRequest();
    lastMessageSendAt = millis();
    break;
  }
  case RUNNING: {
    break;
  }
  }
}

void Game::loopInit() {

  if (buttonPressed(BUTTON_PIN_1)) {
    state = HOST;
  } else if (buttonPressed(BUTTON_PIN_2)) {
    state = SEARCH;
  }
  delay(300);
}

void Game::loopSearch() {
  while (communication.hasMessage()) {
    LoRaMessage message;
    if (communication.getNextMessage(message)) {
      switch (message.messageType) {
      case LoRaMessageType::JOINING_REQUEST_ACCPEPTANCE: {
        uint8_t macAddress[MAC_ADDRESS_SIZE];
        uint8_t macAddressReceived[MAC_ADDRESS_SIZE];
        HaS_Address assignedAddress;
        communication.parseJoiningRequestAcceptance(macAddress,
                                                    &assignedAddress);

        if (memcmp(macAddress, macAddressReceived, MAC_ADDRESS_SIZE)) {
          otherPlayers[0].player_address = message.senderAddress;
          otherPlayers[0].lastMessageReceivedAt = millis();
          otherPlayerCount = 1;
          communication.setLocalAddress(assignedAddress);
          communication.sendAcceptanceAcknoledgment(message.senderAddress);
        }
        break;
      }
      case LoRaMessageType::GAME_START: {
        if (otherPlayerCount &&
            message.senderAddress == otherPlayers[0].player_address) {
          setState(RUNNING);
        }
        break;
      }
      default:
        break;
      }
    }
  }

  if (foundGame &&
      millis() - otherPlayers[0].lastMessageReceivedAt > CANCEL_WAITING_FOR_START_AFTER) {
    setState(INIT);
  } else if (!foundGame && millis() - lastMessageSendAt > CANCEL_SEARCH_AFTER) {
    setState(INIT);
  }
  delay(1000);
}

void Game::loopHost() {

  // Hier Host sendet anfragen
  // postition ermitteln und senden
  // Faenger ermittlen
  // Wenn antworten kommen dann state = RUNNING
}

void Game::loopGame() {

  switch (state) { case INIT:; }
}

bool Game::buttonPressed(int pin) { return digitalRead(pin) == LOW; }
