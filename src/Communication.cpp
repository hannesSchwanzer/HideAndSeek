#include "Communication.hpp"
#include <cstdint>
#include <cstring>
#include <functional>

Communication *Communication::_instance;

Communication::Communication(byte localAddress) : _localAddress(localAddress) {
  _instance = this;
}

void Communication::setup() {
  // Setup LoRa communication and set pins
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);

  if (!LoRa.begin(LORA_BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ; // TODO: Replace with exception
  }
  Serial.println("Started LoRa successfully!");

  // Register onReceive Callback
  LoRa.onReceive(onReceiveBridge);
  LoRa.receive();

  Serial.println("LoRa is in receive mode.");
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
}

void Communication::sendMessage(LoRaMessage message) {
  Serial.println("Sending Message");

  LoRa.beginPacket();
  LoRa.print(message.senderAddress);
  LoRa.print(static_cast<uint8_t>(message.messageType));
  LoRa.print(message.payloadLength);
  for (int i = 0; i < message.payloadLength; i++) {
    LoRa.print(message.payload[i]);
  }
  LoRa.endPacket();
}

LoRaMessage Communication::processMessage(int packetSize) {
  LoRaMessage message;
  // read packet
  if (packetSize < HEADER_SIZE || packetSize > HEADER_SIZE + PAYLOAD_SIZE) {
    // TODO: Throw Exception
  }

  message.senderAddress = LoRa.read();
  message.messageType = static_cast<LoRaMessageType>(LoRa.read());
  message.payloadLength = LoRa.read();

  if (packetSize - HEADER_SIZE != message.payloadLength) {
    // TODO: Throw exception
  }

  for (int i = 0; i < packetSize - HEADER_SIZE; i++) {
    message.payload[i] = LoRa.read();
  }

  return message;
}

void Communication::processPairingRequest(byte sender) {
  // TODO: Implement
}

void Communication::processPairingResponse(byte sender) {
  // TODO: Implement
}

void Communication::onReceive(int packetSize) {
  Serial.print("Received packet '");

  LoRaMessage message = processMessage(packetSize); // TODO: Handle Exceptions
  if (!isMessageValid(message)) {
    return;
  }

  printMessage(message);
  // TODO: Message to queue?
}

bool Communication::isMessageValid(LoRaMessage) {
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
