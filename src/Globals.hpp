#pragma once
#include <cstdint>


#define MAC_ADDRESS_SIZE 6


typedef uint8_t HaS_Address;
struct Position {
    float lat;
    float lon;
};

#if DEBUG
    #define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
    #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
    #define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINTLN(...)
    #define DEBUG_PRINT(...)
    #define DEBUG_PRINTF(...)
#endif


