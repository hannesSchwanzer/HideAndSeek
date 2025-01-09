#pragma once

#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include <cstdint>
#include <queue>
#include "Pins.hpp"

#define PAYLOAD_SIZE 100
#define HEADER_SIZE 3
#define QUEUE_SIZE 10 // Maximum number of messages in the queue

typedef uint8_t HaS_Address;

enum class LoRaMessageType : uint8_t {
    PAIRING_REQUEST = 1,
    GPS_DATA,
    GAME_START,
    PLAYER_ADRESSES,
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
    Communication(byte localAddress);
    bool setup();
    void sendPairingRequest();
    void sendGPSData(int longitude, int latidute);
    void sendMessage(LoRaMessage message);

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
    void processPairingRequest(byte sender);
    void processPairingResponse(byte sender);

    static void onReceiveBridge(int packetSize) {
      if (_instance) {
        _instance->onReceive(packetSize);
      }
    }

    // FreeRTOS Queue
    static QueueHandle_t messageQueue;
};
