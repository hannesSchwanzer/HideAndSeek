#pragma once

#include <Arduino.h>
#include <LoRa.h>
#include <SPI.h>
#include <cstdint>
#include "Pins.hpp"

#define PAYLOAD_SIZE 100
#define HEADER_SIZE 3

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
    void setup();
    void sendPairingRequest();
    void sendGPSData(int longitude, int latidute);
    void sendMessage(LoRaMessage message);

private:
    HaS_Address _localAddress;
    static Communication* _instance;

    void onReceive(int packetSize);
    LoRaMessage processMessage(int packetSize);
    bool isMessageValid(LoRaMessage);
    void processPairingRequest(byte sender);
    void processPairingResponse(byte sender);
    void printMessage(LoRaMessage message);

    static void onReceiveBridge(int packetSize) {
      if (_instance) {
        _instance->onReceive(packetSize);
      }
    }
};
