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

unsigned long lastTimeButtonPressed = 0;

void Game::initGame() {
  display.displaySetup();
  communication.setup();
  gpsHandler.setup();
  pinMode(BUTTON_PIN_1, INPUT_PULLUP); // Mit Pull-Up-Widerstand
  pinMode(BUTTON_PIN_2, INPUT_PULLUP); // Mit Pull-Up-Widerstand
  randomSeed(millis());
  ownPlayer.player_address = random(256);
  setState(INIT);
}

void Game::setState(gameState state) {
  this->state = state;
  display.resetDisplay();
  switch (state) {
  case INIT: {
    display.drawStartScreen();
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
    startTime = millis();
    break;
  }

  case WON: {

    display.resetDisplay();
    display.drawWinningScreen();
    break;
  }
  case DEAD: {
    
    display.resetDisplay();
    display.drawLoosingScreen();
    break;
  }
  }
}

void Game::loopInit() {
  if (buttonPressed(BUTTON_PIN_1)) {
    setState(HOST);
    display.resetDisplay();
  } else if (buttonPressed(BUTTON_PIN_2)) {
    setState(SEARCH);
    display.resetDisplay();
  }
  delay(500);
}

void Game::loopSearch() {
  display.drawWaitingScreen(false, otherPlayerCount);
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
  display.drawWaitingScreen(true, otherPlayerCount);

  unsigned long now = millis();

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
        otherPlayerCount++;

        // Extract max address
        uint8_t macAddress[MAC_ADDRESS_SIZE];
        communication.parseJoiningRequest(macAddress);

        // Send joining acceptance
        communication.sendJoiningRequestAcceptance(macAddress, assignedAdress);

        break;
      }
      case LoRaMessageType::ACCEPTANCE_ACKNOLEDGMENT: {
        // Check which player send the message
        int playerIdx = getPlayerIdxFromAddress(message.senderAddress);


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

  if (buttonPressed(BUTTON_PIN_1)) {
    state = RUNNING;
    display.resetDisplay();
  }
  delay(500);
  // TODO:
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
  unsigned long now = millis();

  while (communication.hasMessage()) {
    LoRaMessage message;
    if (communication.getNextMessage(message)) {
      switch (message.messageType) {
      case LoRaMessageType::GPS_DATA: {
        Position pos; // TODO: Extract from message
        int playeridx = getPlayerIdxFromAddress(message.senderAddress);
        if (playeridx == -1) break;
        otherPlayers[playeridx].position = pos;

        break;
      }
      case LoRaMessageType::PLAYER_DEAD: {
        int playeridx = getPlayerIdxFromAddress(message.senderAddress);
        if (playeridx == -1) break;
        removePlayer(playeridx);
        if (otherPlayerCount == 0) {
          // TODO: Searcher hat gewonnen. Implement
          }
        break;
      }
      default:
        break;
      }
    }
  }

  // Send position every n minutes
  if (now - lastMessageSendAt >= SEND_POS_INTERVAL) {
    // TODO: send position
  }

  // LostGame
  if (!ownPlayer.is_hunter && (buttonPressed(BUTTON_PIN_1) || buttonPressed(BUTTON_PIN_2))) {
    setState(DEAD);
    // TODO: Send message, that you lost
  }

  // Sanity check
  // Lose / Win game after certain amount of time
  if (now - startTime >= GAME_DURATION) {
    if (ownPlayer.is_hunter) {
      setState(DEAD);
    } else {
      setState(WON);
    }
  }

  // Remove player if not answering
  int i = 0;
  while (i < otherPlayerCount) {
    if (now - otherPlayers[i].lastMessageReceivedAt >= REMOVE_PLAYER_NOT_ANSWERING_AFTER) {
      removePlayer(i);
    } else {
      i++;
    }
  }

  // Update pos and azimuth
  bool succesfull = gpsHandler.readLocation(ownPlayer.position);
  if (!succesfull) {
    // TODO: 
    DEBUG_PRINTF("Couldn't read location");
  }
  // TODO: Read compass

  // Update Display
  display.drawMap(otherPlayers, ownPlayer, otherPlayerCount, 90); //TODO: Remove placeholder 90

  delay(1000);
}

void Game::loopGame() {
  switch (state) {
  case INIT:
    DEBUG_PRINTLN("State: INIT");
    loopInit();
    break;
  case HOST:
    DEBUG_PRINTLN("State: HOST");
    loopHost();
    break;
  case SEARCH:
    DEBUG_PRINTLN("State: SEARCH");
    loopSearch();
    break;
  case RUNNING:
    DEBUG_PRINTLN("State: RUNNING");
    loopRunning();
    break;
  case DEAD:
    DEBUG_PRINTLN("State: DEAD");
    waitForRestet();
    break;
  case WON:
    DEBUG_PRINTLN("State: WON");
    waitForRestet();
    break;
  default:
    break;
  }
}

void Game::testButtons() {
  if (buttonPressed(BUTTON_PIN_1)) {
    DEBUG_PRINTLN("Button 1 pressed");
  }
  if (buttonPressed(BUTTON_PIN_2)) {
    DEBUG_PRINTLN("Button 2 pressed");
  }
  delay(500);
}

bool Game::buttonPressed(int pin) {
  if (digitalRead(pin) != LOW) {
    return checkButtonTimer();
  }
  return false;
}

bool Game::checkButtonTimer() {
  unsigned long currentTime = millis();
  if ((currentTime - lastTimeButtonPressed) >= BUTTON_COOLDOWN) {
    lastTimeButtonPressed = currentTime;
    return true;
  } else {
    DEBUG_PRINTLN("Button Blocked");

    return false;
  }
};

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

HaS_Address Game::createNewAdress() {
  return 0; // TODO: Implement
}

void Game::waitForRestet() {
    
  if (buttonPressed(BUTTON_PIN_1) || buttonPressed(BUTTON_PIN_2)) {
    setState(INIT);
  }
  delay(1000);

}

int Game::getPlayerIdxFromAddress(HaS_Address address) {
  int playerIdx = -1;
  for (int i = 0; i < otherPlayerCount; i++) {
    if (otherPlayers[i].player_address == address) {
      playerIdx = i;
      break;
    }
  }

  return playerIdx;
}

