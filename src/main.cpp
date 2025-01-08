#include <Arduino.h>
#include "display.hpp"
#include "Communication.hpp"

// 0: Receive; 1: send
#define MODE 0


byte address = 0x01;
Communication communication(address);

void setup() {
  Serial.begin(115200);
  // displaySetup();
  communication.setup();
}

#if MODE == 0
void loop() {
  while (communication.hasMessage()) {
    LoRaMessage message;
    if (communication.getNextMessage(message)) {
      communication.printMessage(message);
    } else {
      Serial.println("Error retrieving message from queue");
    }
  }

  delay(1000);
}
#elif MODE == 1
void loop() {
  communication.sendGPSData(1,2);
  delay(5000);
}
#endif
