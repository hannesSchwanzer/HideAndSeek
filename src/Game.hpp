#pragma once 

#include "Communication.hpp"
#include "display.hpp"
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
    private:
        gameState state;
        Communication communication;
        Display display;
        Player otherPlayers[MAX_PLAYERS-1];
        Player ownPlayer;
        byte otherPlayerCount;
        Position startPosition;
    
        void loopInit();
        void loopSearch();
        void loopHost();
        void loopRunning();
        bool buttonPressed(int pin);


    public:
        void initGame();
        void loopGame();

};
