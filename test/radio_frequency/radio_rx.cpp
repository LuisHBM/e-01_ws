#include <Arduino.h>
#include <ros.h>
#include <SPI.h>
#include "RF24.h"

#define CE_PIN 4
#define CSN_PIN 5

byte address[][6] = {"1Node", "2Node"};

RF24 radio(CE_PIN, CSN_PIN);

bool radioNumber = 0;   //Identify the dispositive

uint8_t payload;

void setup() {

  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}
  }

  radio.setPayloadSize(sizeof(payload));

  //Creating a pipe connection
  radio.openWritingPipe(address[radioNumber]);
  radio.openReadingPipe(1,address[!radioNumber]);

  radio.startListening(); // RX mode
}

void loop() {
  uint8_t pipe;

  if (radio.available(&pipe)) {
    uint8_t bytes = radio.getPayloadSize();  // get the size of the payload
    radio.read(&payload, bytes);             // fetch payload from FIFO
    Serial.print(F("Received "));
    Serial.print(bytes);  // print the size of the payload
    Serial.print(F(" bytes on pipe "));
    Serial.print(pipe);  // print the pipe number
    Serial.print(F(": "));
    Serial.println(payload);  // print the payload's value
  }
}