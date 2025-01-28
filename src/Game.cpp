#include "Game.hpp"
#include "Globals.hpp"
#include "Pins.hpp"
#include "configValues.hpp"
#include "esp32-hal.h"
#include <cstdint>
#include <cstring>

unsigned long lastTimeButtonPressed = 0;

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
  display.drawStartScreen();
  if (buttonPressed(BUTTON_PIN_1)) {
    state = HOST;
    display.resetDisplay();
  } 
  // if (buttonPressed(BUTTON_PIN_2)) {
  //   state = SEARCH;
  //   display.resetDisplay();
  // }
  delay(500);
}

void Game::loopSearch() {
  
  display.drawWaitingScreen(false, otherPlayerCount);
  
  // while (communication.hasMessage()) {
  //   LoRaMessage message;
  //   if (communication.getNextMessage(message)) {
  //     switch (message.messageType) {
  //     case LoRaMessageType::JOINING_REQUEST_ACCPEPTANCE: {
  //       uint8_t macAddress[MAC_ADDRESS_SIZE];
  //       uint8_t macAddressReceived[MAC_ADDRESS_SIZE];
  //       HaS_Address assignedAddress;
  //       communication.parseJoiningRequestAcceptance(macAddress,
  //                                                   &assignedAddress);

  //       if (memcmp(macAddress, macAddressReceived, MAC_ADDRESS_SIZE)) {
  //         otherPlayers[0].player_address = message.senderAddress;
  //         otherPlayers[0].lastMessageReceivedAt = millis();
  //         otherPlayerCount = 1;
  //         communication.setLocalAddress(assignedAddress);
  //         communication.sendAcceptanceAcknoledgment(message.senderAddress);
  //       }
  //       break;
  //     }
  //     case LoRaMessageType::GAME_START: {
  //       if (otherPlayerCount &&
  //           message.senderAddress == otherPlayers[0].player_address) {
  //         setState(RUNNING);
  //       }
  //       break;
  //     }
  //     default:
  //       break;
  //     }
  //   }
  //}

  // if (foundGame &&
  //     millis() - otherPlayers[0].lastMessageReceivedAt > CANCEL_WAITING_FOR_START_AFTER) {
  //   setState(INIT);
  // } else if (!foundGame && millis() - lastMessageSendAt > CANCEL_SEARCH_AFTER) {
  //   setState(INIT);
  // }


  if (buttonPressed(BUTTON_PIN_1)) {
    state = INIT;
    display.resetDisplay();

  }
  delay(500);
}

void Game::loopHost() {
  display.drawWaitingScreen(true, otherPlayerCount);
  // Hier Host sendet anfragen
  // postition ermitteln und senden
  // Faenger ermittlen
  // Wenn antworten kommen dann state = RUNNING
  if (buttonPressed(BUTTON_PIN_1)) {
    state = RUNNING;
    display.resetDisplay();
  }
  delay(500);

}
void Game::loopRunning(){
  delay(500);

};

void Game::loopGame() {
  switch (state) { 
    case INIT:
      Serial.println("State: INTI");
      loopInit();
      break;
    case HOST:
      Serial.println("State: HOST");
      loopHost();
      break;
    case SEARCH:
      Serial.println("State: SEARCH");
      loopSearch();
      break;
    case RUNNING:
      Serial.println("State: RUNNING");
      loopRunning();
      break;
    default:
      break;
    }
}


void Game::testButtons(){
  if (buttonPressed(BUTTON_PIN_1)) {
    Serial.println("Button 1 pressed");
  }
  if (buttonPressed(BUTTON_PIN_2)) {
    Serial.println("Button 2 pressed");
  }
  delay(500);
}
bool Game::buttonPressed(int pin) {   
  if (digitalRead(pin) != LOW){
    return checkButtonTimer();
  }
  return false;
}

bool Game::checkButtonTimer(){
unsigned long currentTime = millis();
if ((currentTime - lastTimeButtonPressed) >= BUTTON_COOLDOWN) {
    lastTimeButtonPressed = currentTime;
    return true;
}else{
  Serial.println("Button Blocked");

  return false;
}
};