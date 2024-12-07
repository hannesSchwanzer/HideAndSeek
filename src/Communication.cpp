#include "Communication.hpp"

Communication::Communication(byte localAddress)
  : _localAddress(localAddress) {}


void Communication::setup() {
  // Setup LoRa communication and set pins
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);

  if (!LoRa.begin(LORA_BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);  // TODO: Replace with exception
  }

  // Register onReceive Callback
  auto callbackFunction = [this](int packetSize) {
    this->onReceive(packetSize);
  };
  LoRa.onReceive(callbackFunction);
}


void Communication::sendPairingRequest() {
 // TODO: Implement
}


void Communication::sendGPSData(int longitude, int latidute) {
 // TODO: Implement
}


void Communication::sendMessage(LoRaMessage message) {
 // TODO: Implement
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
  
  // TODO: Message to queue?
}
