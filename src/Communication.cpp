#include "Communication.hpp"
#include <cstdint>
#include <cstring>
#include <functional>

Communication *Communication::_instance;
QueueHandle_t Communication::messageQueue;

Communication::Communication(byte localAddress) : _localAddress(localAddress) {
  _instance = this;
}

bool Communication::setup() {
  // Setup LoRa communication and set pins
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);

  if (!LoRa.begin(LORA_BAND)) {
    Serial.println("Starting LoRa failed!");
    return false;
  }
  Serial.println("Started LoRa successfully!");

  // Initialize the message queue
  messageQueue = xQueueCreate(QUEUE_SIZE, sizeof(LoRaMessage));
  if (messageQueue == nullptr) {
      Serial.println("Failed to create message queue!");
    return false;
  }

  // Register onReceive Callback
  LoRa.onReceive(onReceiveBridge);
  LoRa.receive();

  Serial.println("LoRa is in receive mode.");
  return true;
}

void Communication::sendPairingRequest() {
  // TODO: Implement
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

void Communication::sendMessage(LoRaMessage message) {
  Serial.println("Sending Message");

  LoRa.beginPacket();
  LoRa.write(message.senderAddress);                     // Send as binary
  LoRa.write(static_cast<uint8_t>(message.messageType)); // Send as binary
  LoRa.write(message.payloadLength);                    // Send as binary
  LoRa.write((uint8_t*)message.payload, message.payloadLength); // Send the payload
  LoRa.endPacket();

  // Switch back to receive mode
  LoRa.receive();
}

bool Communication::processMessage(int packetSize, LoRaMessage& message) {
    if (packetSize < HEADER_SIZE) {
        return false; // Not enough data for a header
    }

    message.senderAddress = (uint8_t)LoRa.read(); // Read as binary
    message.messageType = static_cast<LoRaMessageType>((uint8_t)LoRa.read());
    message.payloadLength = (uint8_t)LoRa.read();

    if (packetSize - HEADER_SIZE != message.payloadLength || message.payloadLength > PAYLOAD_SIZE) {
        return false; // Payload length mismatch or exceeds buffer size
    }

    for (int i = 0; i < message.payloadLength; i++) {
        message.payload[i] = (uint8_t)LoRa.read(); // Read as binary
    }

    return true;
}

void Communication::processPairingRequest(byte sender) {
  // TODO: Implement
}

void Communication::processPairingResponse(byte sender) {
  // TODO: Implement
}

void Communication::onReceive(int packetSize) {
  Serial.println("Received packet '");

  LoRaMessage message;
  bool processing_valid = processMessage(packetSize, message); // TODO: Handle Exceptions
  if (!processing_valid || !isMessageValid(message)) {
  Serial.println("Message invalid");
  Serial.println(processing_valid);
    return;
  }

  // Add message to the queue
  if (xQueueSend(messageQueue, &message, 0) != pdTRUE) {
      Serial.println("Queue full! Dropping message."); //TODO: 
  }
}

bool Communication::isMessageValid(LoRaMessage& message) {
  return true; // TODO:
}

void Communication::printMessage(LoRaMessage message) {
  Serial.println("Header:");
  Serial.print("\tSender Address: ");
  Serial.println(message.senderAddress);
  Serial.print("\tMessage Type: ");
  Serial.println(static_cast<uint8_t>(message.messageType));
  Serial.print("\tPayload Length: ");
  Serial.println(message.payloadLength);

  Serial.println("payload");
  for (int i = 0; i < message.payloadLength; i++) {
    Serial.print((byte)message.payload[i]);
  }
}

// Check if there are messages in the queue
bool Communication::hasMessage() {
    return uxQueueMessagesWaiting(messageQueue) > 0;
}

// Retrieve the next message from the queue
bool Communication::getNextMessage(LoRaMessage& message) {
    return xQueueReceive(messageQueue, &message, 0) == pdTRUE;
}
