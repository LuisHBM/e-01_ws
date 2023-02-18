  #include <Arduino.h>
  #include <ros.h>
  #include <SPI.h>
  #include "RF24.h"

  #define CE_PIN 4
  #define CSN_PIN 5

  byte address[][6] = {"1Node", "2Node"};

  RF24 radio(CE_PIN, CSN_PIN);

  bool radioNumber = 1;   //Identify the dispositive

  uint8_t payload = 1; // 1 -> UP, 2 -> DOWN, 3 -> LEFT, 4 -> RIGHT
  int count = 0;
  int delayTX = 1000; //Delay for the transmissor in microsseconds (ms)

  void setup() {

    Serial.begin(115200);
    while(!Serial) {}

    if (!radio.begin()) {
      Serial.println(F("radio hardware is not responding!!"));
      while (1) {}
    }

    radio.openWritingPipe(address[radioNumber]);
    radio.openReadingPipe(1,address[!radioNumber]);

    radio.setPayloadSize(sizeof(payload));

    radio.stopListening(); // TX mode

  }

  void loop() {

    unsigned long initial_time = micros();
    bool post = radio.write(&payload, sizeof(uint8_t));
    unsigned long final_time = micros();

    if(post) {
      Serial.println(count);
      Serial.print("° connection successfull!");
      Serial.print(" Time: ");
      Serial.print(final_time - initial_time);
      Serial.print("us");
      count++;
    } else {
      Serial.println("Connection failed.");
    }

    delay(delayTX);
  }