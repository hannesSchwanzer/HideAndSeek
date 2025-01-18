#pragma once

#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include <cstdint>
#include <queue>
#include "Globals.hpp"

enum class LoRaMessageType : uint8_t {
    PAIRING_REQUEST = 1,
    GPS_DATA,
    GAME_START,
    PLAYER_ADRESSES,
    JOINING_REQUEST,
    JOINING_REQUEST_ACCPEPTANCE,
    ACCEPTANCE_ACKNOLEDGMENT,
    OTHER,
};

struct LoRaMessage {
    HaS_Address senderAddress;
    LoRaMessageType messageType;
    uint8_t payloadLength;
    char payload[100];
};

class Communication {
public:
    Communication();
    bool setup();
    void setLocalAddress(HaS_Address localAddress);
    void sendJoiningRequest();
    void sendJoiningRequestAcceptance(uint8_t* macAddress, HaS_Address assignedAddress);
    void sendAcceptanceAcknoledgment(HaS_Address receiverAddress);
    void sendGPSData(int longitude, int latidute);
    void sendMessage(LoRaMessage& message);
    void parseJoiningRequestAcceptance(uint8_t* macAddress, HaS_Address* assignedAddress);

    void printMessage(LoRaMessage message);
    static void setInstance(Communication* instance) {
        _instance = instance;
    }

    // New methods for external message handling
    bool hasMessage(); // Check if there are messages in the queue
    bool getNextMessage(LoRaMessage& message); // Retrieve the next message from the queue

private:
    HaS_Address _localAddress;
    static Communication* _instance;

    void onReceive(int packetSize);
    bool processMessage(int packetSize, LoRaMessage& message);
    bool isMessageValid(LoRaMessage& message);

    static void onReceiveBridge(int packetSize) {
      if (_instance) {
        _instance->onReceive(packetSize);
      }
    }

    // FreeRTOS Queue
    static QueueHandle_t messageQueue;
};
