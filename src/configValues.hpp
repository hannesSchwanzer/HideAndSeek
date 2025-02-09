#pragma once 
//Display
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 160
#define VIEW_DISTANCE 100 // in meters
#define FIELD_RADIUS  300 // Radius des Spielfelds in Metern

//Communication
#define PAYLOAD_SIZE 100
#define HEADER_SIZE 3
#define QUEUE_SIZE 10 // Maximum number of messages in the queue


//Game logic
#define MAX_PLAYERS 3
#define CANCEL_SEARCH_AFTER (10* 60 * 1000)
#define RESEND_JOINING_MESSAGE_INTERVAL (30 * 1000)
#define REMOVE_PLAYER_NOT_ACCEPTING_AFTER (10 * 1000)
#define REMOVE_PLAYER_NOT_ANSWERING_AFTER (5 * 60 * 1000)
#define SEND_POS_INTERVAL (10 * 1000)
#define GAME_DURATION (120 * 60 * 1000)
#define HUNTER_REVEAL (1 * 60 * 1000)

//Button
#define BUTTON_COOLDOWN 1000 //5sec nur zum testen sonnst 1000

// GPS
#define WAIT_DURATION_GPS_INIT (10 * 60 * 1000)

