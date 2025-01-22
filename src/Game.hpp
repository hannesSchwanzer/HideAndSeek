#pragma once 

#include "Communication.hpp"
#include "Display.hpp"
#include "configValues.hpp"
#include "Player.hpp"
#include "Globals.hpp"


enum gameState{
  INIT,
  HOST,
  SEARCH,
  RUNNING
};

class Game{
public:
    void initGame();
    void loopGame();

private:
    gameState state;
    Communication communication;
    Display display;
    Player otherPlayers[MAX_PLAYERS-1];
    Player ownPlayer;
    byte otherPlayerCount;
    Position startPosition;
    unsigned long lastMessageSendAt;
    bool foundGame = false;
    bool receivedRequestAcceptance = false;
    

    void setState(gameState state);
    void loopInit();
    void loopSearch();
    void loopHost();
    void loopRunning();
    bool buttonPressed(int pin);
    HaS_Address createNewAdress();
    void removePlayer(int idx);
};
