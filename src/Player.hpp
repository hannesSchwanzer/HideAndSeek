#pragma once
#include "Globals.hpp"

struct Player
{
    HaS_Address player_address;
    Position position;
    bool is_hunter;
    unsigned long lastMessageReceivedAt;
};
