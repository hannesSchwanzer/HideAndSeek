#pragma once 

//Display // DO NOT CHANGE
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 160

#define VIEW_DISTANCE 350 // Sichtweite in Metern
#define FIELD_RADIUS  3000 // Radius des Spielfelds in Metern

//Communication // DO NOT CHANGE
#define PAYLOAD_SIZE 100 
#define HEADER_SIZE 3
#define QUEUE_SIZE 10 // Maximum number of messages in the queue


//Game logic
#define MAX_PLAYERS 3   // Maximale Anzahl an Spielern
#define GAME_DURATION (120 * 60 * 1000) // Wie lange das Spiel dauern soll
#define HUNTER_REVEAL (1 * 60 * 1000) // Zeit nachdem der Jäger enttarnt wird
#define SEND_POS_INTERVAL (60 * 1000)  // Wie oft die Position gesendet wird


#define CANCEL_SEARCH_AFTER (10* 60 * 1000) // Wann die Suche abgebrochen wird
#define RESEND_JOINING_MESSAGE_INTERVAL (30 * 1000) // Interval, wann die Anfrage erneut gesendet wird
#define REMOVE_PLAYER_NOT_ACCEPTING_AFTER (10 * 1000) // Wann ein Spieler entfernt wird, wenn er die Anfrage nicht annimmt
#define REMOVE_PLAYER_NOT_ANSWERING_AFTER (5 * 60 * 1000) // Wann ein Spieler entfernt wird, wenn er nicht antwortet



// GPS
#define WAIT_DURATION_GPS_INIT (10 * 60 * 1000) //Wie lange auf GPS-Signal gewartet wird

//Button
#define BUTTON_COOLDOWN 1000 //DO NOT CHANGE