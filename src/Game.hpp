#pragma once

#include "Communication.hpp"
#include "Compass.hpp"
#include "Display.hpp"
#include "GPSHandler.hpp"
#include "Globals.hpp"
#include "Pins.hpp"
#include "Player.hpp"
#include "configValues.hpp"

enum gameState { INIT, HOST, SEARCH, RUNNING, DEAD, WON };

class Game {
public:
  Game();
  void initGame();
  void loopGame();

private:
  gameState state;
  Communication communication;
  Display display;
  GPSHandler gpsHandler;
  Compass compass;
  Player otherPlayers[MAX_PLAYERS - 1];
  Player ownPlayer;
  byte otherPlayerCount;
  Position startPosition;
  unsigned long lastMessageSendAt;
  bool foundGame = false;
  bool receivedRequestAcceptance = false;
  unsigned long startTime;

  void setState(gameState state);
  void loopInit();
  void loopSearch();
  void loopHost();
  void loopRunning();
  bool buttonPressed(int pin);
  HaS_Address createNewAdress();
  void removePlayer(int idx);
  bool checkButtonTimer();
  void testButtons();
  int getPlayerIdxFromAddress(HaS_Address address);
  void waitForRestet();
  bool doesAdressExist(HaS_Address adress);
  void resetVariables();
};
