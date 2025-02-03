#pragma once 
//Display
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 160

//Communication
#define PAYLOAD_SIZE 100
#define HEADER_SIZE 3
#define QUEUE_SIZE 10 // Maximum number of messages in the queue


//Game logic
#define MAX_DISTANCE 1000 // Maximale Distanz in Metern
#define MAX_PLAYERS 3
#define CANCEL_SEARCH_AFTER (10* 60 * 1000)
#define CANCEL_WAITING_FOR_START_AFTER (30 * 60 * 1000)
#define REMOVE_PLAYER_NOT_ACCEPTING_AFTER (10 * 1000)
#define REMOVE_PLAYER_NOT_ANSWERING_AFTER (5 * 60 * 1000)
#define SEND_POS_INTERVAL (60 * 1000)
#define GAME_DURATION (120 * 60 * 1000)

//Button
#define BUTTON_COOLDOWN 2000 //5sec nur zum testen sonnst 1000


