#include "Game.hpp"



void Game::initGame(){
    display.displaySetup();
    communication.setup();
    pinMode(BUTTON_PIN_1, INPUT_PULLUP);  // Mit Pull-Up-Widerstand
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);  // Mit Pull-Up-Widerstand
    randomSeed(millis());
    ownPlayer.player_address = random(256);
    state = INIT;
}



void Game::loopInit(){
    
    //Hier bis knopfdruck warten

    if (buttonPressed(BUTTON_PIN_1)){
        state = HOST;
    }
    else if (buttonPressed(BUTTON_PIN_2))
    {
        state = SEARCH;
    }
    delay(300);
}

void Game::loopSearch(){

    //Hier player suchen
    //Wenn gefunden dann state = RUNNING
}

void Game::loopHost(){
    
    //Hier Host sendet anfragen
    //postition ermitteln und senden
    //Faenger ermittlen 
    //Wenn antworten kommen dann state = RUNNING

}

void Game::loopGame(){
    
    switch(state){
        
        case INIT:
    }
}

bool Game::buttonPressed(int pin){
    return digitalRead(pin) == LOW;
}



