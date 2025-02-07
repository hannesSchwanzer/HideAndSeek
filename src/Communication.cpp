#include "Communication.hpp"
#include "Globals.hpp"
#include "Pins.hpp"
#include "configValues.hpp"
#include "esp32-hal.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include <cstring>
#include <functional>

Communication *Communication::_instance;
QueueHandle_t Communication::messageQueue;

Communication::Communication() {
  _localAddress = 0;
  _instance = this;
}

bool Communication::setup() {
  // Setup LoRa communication and set pins
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);

  if (!LoRa.begin(LORA_BAND)) {
    DEBUG_PRINTLN("Starting LoRa failed!");

    return false;
  }
  DEBUG_PRINTLN("Started LoRa successfully!");

  // Initialize the message queue
  messageQueue = xQueueCreate(QUEUE_SIZE, sizeof(LoRaMessage));
  if (messageQueue == nullptr) {
    DEBUG_PRINTLN("Failed to create message queue!");

    return false;
  }

  // Register onReceive Callback
  LoRa.onReceive(onReceiveBridge);
  LoRa.receive();

  DEBUG_PRINTLN("LoRa is in receive mode.");
  return true;
}

void Communication::setLocalAddress(HaS_Address localAddress) {
  this->_localAddress = localAddress;
}

void Communication::sendJoiningRequest() {
  uint8_t macAddress[MAC_ADDRESS_SIZE];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, macAddress);

  LoRaMessage message;
  message.messageType = LoRaMessageType::JOINING_REQUEST;
  message.senderAddress = 0;
  message.payloadLength = MAC_ADDRESS_SIZE;
  std::memcpy(message.payload, macAddress, MAC_ADDRESS_SIZE);

  sendMessage(message);
}

void Communication::sendJoiningRequestAcceptance(uint8_t *macAddress,
                                                 HaS_Address assignedAddress) {
  LoRaMessage message;
  message.messageType = LoRaMessageType::JOINING_REQUEST_ACCPEPTANCE;
  message.senderAddress = _localAddress;
  message.payloadLength = MAC_ADDRESS_SIZE + sizeof(HaS_Address);
  std::memcpy(message.payload, macAddress, MAC_ADDRESS_SIZE);
  std::memcpy(message.payload + MAC_ADDRESS_SIZE, &assignedAddress,
              sizeof(HaS_Address));

  sendMessage(message);
}

void Communication::sendAcceptanceAcknoledgment(HaS_Address receiverAddress) {
  LoRaMessage message;
  message.messageType = LoRaMessageType::ACCEPTANCE_ACKNOLEDGMENT;
  message.senderAddress = _localAddress;
  message.payloadLength = sizeof(HaS_Address);
  std::memcpy(message.payload, &receiverAddress, sizeof(HaS_Address));

  sendMessage(message);
}

void Communication::sendGPSData(Position& position) {
  LoRaMessage message;
  message.messageType = LoRaMessageType::GPS_DATA;
  message.senderAddress = this->_localAddress;
  message.payloadLength = sizeof(Position);

  std::memcpy(message.payload, &position, sizeof(Position));
  DEBUG_PRINTF("Sending: - Lat: %f, Long: %f\n", position.lat, position.lon);

  sendMessage(message);
}


void Communication::sendPlayerDead() {
  LoRaMessage message;
  message.messageType = LoRaMessageType::PLAYER_DEAD;
  message.senderAddress = this->_localAddress;
  message.payloadLength = 0;

  sendMessage(message);
}


void Communication::sendGameStart(Position& startPosition, Player& ownPlayer, Player otherPlayer[], byte otherPlayerCount) {
  LoRaMessage message;
  message.messageType = LoRaMessageType::GAME_START;
  message.senderAddress = this->_localAddress;
  message.payloadLength = sizeof(startPosition) + sizeof(PlayerCommunicationData) * (1 + otherPlayerCount) + sizeof(otherPlayerCount);

  byte playCountAll = otherPlayerCount + 1;
  
  char* payloadPos = message.payload;
  std::memcpy(payloadPos, &startPosition, sizeof(startPosition));
  payloadPos += sizeof(startPosition);
  std::memcpy(payloadPos, &playCountAll, sizeof(playCountAll));
  payloadPos += sizeof(playCountAll);


  PlayerCommunicationData ownPlayerData = preparePlayer(ownPlayer);
  std::memcpy(payloadPos, &ownPlayerData, sizeof(PlayerCommunicationData));
  payloadPos += sizeof(PlayerCommunicationData);

  for (int i = 0; i < otherPlayerCount; i++) {
    PlayerCommunicationData playerData = preparePlayer(otherPlayer[i]);
    std::memcpy(payloadPos, &playerData, sizeof(PlayerCommunicationData));
    payloadPos += sizeof(PlayerCommunicationData);
  }

  sendMessage(message);
}


void Communication::sendMessage(LoRaMessage &message) {
  DEBUG_PRINTLN("Sending Message");
  printMessage(message);

  LoRa.beginPacket();
  LoRa.write(message.senderAddress);                     // Send as binary
  LoRa.write(static_cast<uint8_t>(message.messageType)); // Send as binary
  LoRa.write(message.payloadLength);                     // Send as binary
  LoRa.write((uint8_t *)message.payload,
             message.payloadLength); // Send the payload
  LoRa.endPacket();

  // Switch back to receive mode
  LoRa.receive();
}

bool Communication::processMessage(int packetSize, LoRaMessage &message) {
  if (packetSize < HEADER_SIZE) {
    return false; // Not enough data for a header
  }

  message.senderAddress = (uint8_t)LoRa.read(); // Read as binary
  message.messageType = static_cast<LoRaMessageType>((uint8_t)LoRa.read());
  message.payloadLength = (uint8_t)LoRa.read();

  if (packetSize - HEADER_SIZE != message.payloadLength ||
      message.payloadLength > PAYLOAD_SIZE) {
    return false; // Payload length mismatch or exceeds buffer size
  }

  for (int i = 0; i < message.payloadLength; i++) {
    message.payload[i] = (uint8_t)LoRa.read(); // Read as binary
  }

  return true;
}

void Communication::onReceive(int packetSize) {
  DEBUG_PRINTLN("Received packet '");

  LoRaMessage message;
  bool processing_valid =
      processMessage(packetSize, message);
  if (!processing_valid) {
    DEBUG_PRINTLN("Message invalid");
    return;
  }

  // Add message to the queue
  if (xQueueSend(messageQueue, &message, 0) != pdTRUE) {
    DEBUG_PRINTLN("Queue full! Dropping message.");
  }
}

void Communication::printMessage(LoRaMessage message) {
  DEBUG_PRINTLN("Header:");
  DEBUG_PRINT("\tSender Address: ");
  DEBUG_PRINTLN(message.senderAddress);
  DEBUG_PRINT("\tMessage Type: ");
  DEBUG_PRINTLN(static_cast<uint8_t>(message.messageType));
  DEBUG_PRINT("\tPayload Length: ");
  DEBUG_PRINTLN(message.payloadLength);

  DEBUG_PRINTLN("payload");
  for (int i = 0; i < message.payloadLength; i++) {
    DEBUG_PRINT((byte)message.payload[i]);
  }
  DEBUG_PRINT("\n");
}

void Communication::parseJoiningRequestAcceptance(
    LoRaMessage& message, uint8_t *macAddress, HaS_Address *assignedAddress) {
  char* payloadPos = message.payload;

  for (int i = 0; i < MAC_ADDRESS_SIZE; i++) {
    macAddress[i] = (uint8_t) *payloadPos;
    payloadPos += 1;
  }
  *assignedAddress = (uint8_t) *payloadPos;
}

void Communication::parseJoiningRequest(LoRaMessage& message, uint8_t *macAddress) {
  for (int i = 0; i < MAC_ADDRESS_SIZE; i++) {
    macAddress[i] = (uint8_t) message.payload[i];
  }
}

void Communication::parseGameStart(LoRaMessage& message, Position& startPosition, Player& ownPlayer, Player* otherPlayer, byte& otherPlayerCount) {
  DEBUG_PRINTLN("Parsing Game Start");
  char* payloadPos = message.payload;

  std::memcpy(&startPosition, payloadPos, sizeof(startPosition));
  payloadPos += sizeof(startPosition);
  DEBUG_PRINTF("Startpos: - Lat: %f, Long: %f\n", startPosition.lat, startPosition.lon);

  byte playCountAll;
  std::memcpy(&playCountAll, payloadPos, sizeof(playCountAll));
  payloadPos += sizeof(playCountAll);
  otherPlayerCount = playCountAll - 1;
  DEBUG_PRINTF("Otherplayercount: - Lat: %d\n", otherPlayerCount);

  byte currentPlayer = 0;
  for (int i = 0; i < playCountAll; i++) {
    PlayerCommunicationData playerData;
    std::memcpy(&playerData, payloadPos, sizeof(PlayerCommunicationData));
    payloadPos += sizeof(PlayerCommunicationData);
    DEBUG_PRINTF("Player %d: - Address: %d, isHunter: %d\n", i, playerData.player_address, playerData.is_hunter);

    if (playerData.player_address == _localAddress) {
      ownPlayer = {.player_address = _localAddress,
        .position = {0, 0},
        .is_hunter = playerData.is_hunter,
        .lastMessageReceivedAt = millis(),
        .hasAccepted = true};
    } else {
      otherPlayer[currentPlayer++] = {.player_address = playerData.player_address,
        .position = {0, 0},
        .is_hunter = playerData.is_hunter,
        .lastMessageReceivedAt = millis(),
        .hasAccepted = true};
    }
  }
}

void Communication::parseGpsData(LoRaMessage& message, Position& position) {
  std::memcpy(&position, message.payload, sizeof(Position));
  DEBUG_PRINTF("Parsing: - Lat: %f, Long: %f\n", position.lat, position.lon);
}



// Check if there are messages in the queue
bool Communication::hasMessage() {
  return uxQueueMessagesWaiting(messageQueue) > 0;
}

// Retrieve the next message from the queue
bool Communication::getNextMessage(LoRaMessage &message) {
  return xQueueReceive(messageQueue, &message, 0) == pdTRUE;
}


PlayerCommunicationData Communication::preparePlayer(Player& player) {
  PlayerCommunicationData p;
  p.is_hunter = player.is_hunter;
  p.player_address = player.player_address;

  return p;
}

