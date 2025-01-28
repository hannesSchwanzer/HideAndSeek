#include "Communication.hpp"
#include "Globals.hpp"
#include "Pins.hpp"
#include "configValues.hpp"
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
  if (ret != ESP_OK) {
    // TODO: Error?
  }

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

void Communication::sendGPSData(int longitude, int latidute) {
  LoRaMessage message;
  message.messageType = LoRaMessageType::GPS_DATA;
  message.senderAddress = this->_localAddress;
  message.payloadLength = sizeof(longitude) + sizeof(latidute);

  std::memcpy(message.payload, &longitude, sizeof(longitude));
  std::memcpy(message.payload + sizeof(longitude), &latidute, sizeof(latidute));

  sendMessage(message);
}

void Communication::sendMessage(LoRaMessage &message) {
  DEBUG_PRINTLN("Sending Message");

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
      processMessage(packetSize, message); // TODO: Handle Exceptions
  if (!processing_valid || !isMessageValid(message)) {
    DEBUG_PRINTLN("Message invalid");
    return;
  }

  // Add message to the queue
  if (xQueueSend(messageQueue, &message, 0) != pdTRUE) {
    DEBUG_PRINTLN("Queue full! Dropping message."); // TODO:
  }
}

bool Communication::isMessageValid(LoRaMessage &message) {
  return true; // TODO:Check for specif message sizes
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
}

void Communication::parseJoiningRequestAcceptance(
    uint8_t *macAddress, HaS_Address *assignedAddress) {
  for (int i = 0; i < MAC_ADDRESS_SIZE; i++) {
    macAddress[i] = (uint8_t)LoRa.read();
  }
  *assignedAddress = (uint8_t)LoRa.read();
}

void Communication::parseJoiningRequest(uint8_t *macAddress) {
  for (int i = 0; i < MAC_ADDRESS_SIZE; i++) {
    macAddress[i] = (uint8_t)LoRa.read();
  }
}

// Check if there are messages in the queue
bool Communication::hasMessage() {
  return uxQueueMessagesWaiting(messageQueue) > 0;
}

// Retrieve the next message from the queue
bool Communication::getNextMessage(LoRaMessage &message) {
  return xQueueReceive(messageQueue, &message, 0) == pdTRUE;
}
