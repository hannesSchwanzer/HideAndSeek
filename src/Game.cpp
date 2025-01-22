#include "Game.hpp"
#include "Communication.hpp"
#include "Globals.hpp"
#include "Pins.hpp"
#include "configValues.hpp"
#include "esp32-hal.h"
#include <cstdint>
#include <cstring>

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"

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
  // Handle incoming messages
  while (communication.hasMessage()) {
    LoRaMessage message;
    if (communication.getNextMessage(message)) {
      switch (message.messageType) {
      case LoRaMessageType::JOINING_REQUEST_ACCPEPTANCE: {
        // Get own mac address
        uint8_t macAddress[MAC_ADDRESS_SIZE];
        esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, macAddress);
        if (ret != ESP_OK) {
          // TODO: Error?
        }

        // parse message
        uint8_t macAddressReceived[MAC_ADDRESS_SIZE];
        HaS_Address assignedAddress;
        communication.parseJoiningRequestAcceptance(macAddress,
                                                    &assignedAddress);

        // Check if message was sent to this device
        if (memcmp(macAddress, macAddressReceived, MAC_ADDRESS_SIZE)) {
          // Add host to other players
          otherPlayers[0] = {.player_address = message.senderAddress,
                             .position = {0, 0},
                             .is_hunter = false,
                             .lastMessageReceivedAt = millis(),
                             .hasAccepted = true};
          otherPlayerCount = 1;

          // Send acceptance and set local address
          communication.setLocalAddress(assignedAddress);
          communication.sendAcceptanceAcknoledgment(message.senderAddress);
        }
        break;
      }
      case LoRaMessageType::GAME_START: {
        // Check if already joined a game and if message came from host
        if (otherPlayerCount == 1 &&
            message.senderAddress == otherPlayers[0].player_address) {
          // TODO: parse message
          setState(RUNNING);
        }
        break;
      }
      default:
        break;
      }
    }
  }

  if (foundGame && millis() - otherPlayers[0].lastMessageReceivedAt >
                       CANCEL_WAITING_FOR_START_AFTER) {
    setState(INIT);
  } else if (!foundGame && millis() - lastMessageSendAt > CANCEL_SEARCH_AFTER) {
    setState(INIT);
  }
  delay(1000);
}

void Game::loopHost() {
  unsigned long now = millis();
  //
  // Handle incoming messages
  while (communication.hasMessage()) {
    LoRaMessage message;
    if (communication.getNextMessage(message)) {
      switch (message.messageType) {
      case LoRaMessageType::JOINING_REQUEST: {
        // Check if there is space for another player
        if (otherPlayerCount >= MAX_PLAYERS - 1)
          break;

        // Create new adress
        HaS_Address assignedAdress = createNewAdress();

        // Create new Player
        otherPlayers[otherPlayerCount] = {.player_address = assignedAdress,
                                          .position = {0, 0},
                                          .is_hunter = false,
                                          .lastMessageReceivedAt = now,
                                          .hasAccepted = false};

        // Extract max address
        uint8_t macAddress[MAC_ADDRESS_SIZE];
        communication.parseJoiningRequest(macAddress);

        // Send joining acceptance
        communication.sendJoiningRequestAcceptance(macAddress, assignedAdress);

        break;
      }
      case LoRaMessageType::ACCEPTANCE_ACKNOLEDGMENT: {
        // Check which player send the message
        int playerIdx = -1;
        for (int i = 0; i < otherPlayerCount; i++) {
          if (otherPlayers[0].player_address == message.senderAddress) {
            playerIdx = i;
            break;
          }
        }

        // Ingore message if no player was found
        if (playerIdx == -1)
          break;

        // Change player status;
        otherPlayers[playerIdx].hasAccepted = true;
        otherPlayers[playerIdx].lastMessageReceivedAt = now;

        break;
      }
      default:
        break;
      }
    }
  }

  // TODO:
  // detect button press and start game
  //  Send own position
  //  random select catcher

  // Sanity check
  // Remove players after certain amount of time, if not received an acceptance
  int i = 0;
  while (i < otherPlayerCount) {
    if (!otherPlayers[i].hasAccepted &&
        now - otherPlayers[i].lastMessageReceivedAt >=
            REMOVE_PLAYER_NOT_ACCEPTING_AFTER) {
      removePlayer(i);
    } else {
      i++;
    }
  }
}

void Game::loopRunning() {
  // TODO:
  // Handle incoming messages
  while (communication.hasMessage()) {
    LoRaMessage message;
    if (communication.getNextMessage(message)) {
      switch (message.messageType) {
      default:
        break;
      }
    }
  }

  // TODO:
  // detect button press and react to it

  // TODO:
  // Sanity check
  //  Mark players as dead from otherPlayers if they didn't answer after a fixed
  //  amount of time
}

void Game::loopGame() {

  switch (state) { case INIT:; }
}

bool Game::buttonPressed(int pin) { return digitalRead(pin) == LOW; }

void Game::removePlayer(int idx) {
  // Check if index is valid
  if (idx < 0 || idx >= otherPlayerCount) {
    return;
  }

  // Shift elements to the left
  for (int i = idx; i < otherPlayerCount - 1; i++) {
    otherPlayers[i] = otherPlayers[i + 1];
  }

  // Decrease the size
  otherPlayerCount--;
}
