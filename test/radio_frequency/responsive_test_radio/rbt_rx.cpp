#include <Arduino.h>
#include <ros.h>
#include <SPI.h>
#include "RF24.h"

#define CE_PIN 4
#define CSN_PIN 5

byte address[][6] = {"1Node", "2Node"};

RF24 radio(CE_PIN, CSN_PIN);

bool radioNumber = 0;   //Identify the dispositive

int payload;

const uint8_t R_PWM =  25;
const uint8_t R_BACK = 12;
const uint8_t R_FORW = 13;

const uint8_t L_PWM = 33;
const uint8_t L_BACK = 27;
const uint8_t L_FORW = 14;

const uint8_t channel_L = 0;
const uint8_t channel_R = 1;

void pin_def(){

  const int freq = 5000;
  const int res = 8;
  
  pinMode(L_PWM,  OUTPUT);
  pinMode(L_FORW, OUTPUT);
  pinMode(L_BACK, OUTPUT);
  pinMode(R_PWM,  OUTPUT);
  pinMode(R_FORW, OUTPUT);
  pinMode(R_BACK, OUTPUT);
  
  ledcSetup(channel_R ,freq , res);
  ledcSetup(channel_L ,freq , res);

  ledcAttachPin(R_PWM,channel_R);
  ledcAttachPin(L_PWM,channel_L);

}

void speed ()
{
    ledcWrite(channel_R, 200);  
    ledcWrite(channel_L, 200);
}

void stop()
{
   
   ledcWrite(channel_R, 0);  
   ledcWrite(channel_L, 0);
}

void direction(uint8_t buffer){
  switch(buffer) {
    case 1:
      digitalWrite(L_FORW, 1);
      digitalWrite(R_FORW, 1);
      speed();
      break;
    case 2:
      digitalWrite(L_FORW, 0);
      digitalWrite(R_FORW, 0);
      speed();
      break;
    case 3:
      digitalWrite(L_FORW, 0);
      digitalWrite(R_FORW, 1);
      speed();
      break;
    case 4:
      digitalWrite(L_FORW, 1);
      digitalWrite(R_FORW, 0);
      speed();
      break;
    case 0:
      stop();
      digitalWrite(L_FORW, 0);
      digitalWrite(R_FORW, 0);
      break;
  }
}

void setup(){

  Serial.begin(115200);

  pin_def();
  stop();
  Serial.println("Get Ready");
  
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}
  }

  //Creating a pipe connection
  radio.openWritingPipe(address[radioNumber]);
  radio.openReadingPipe(1,address[!radioNumber]);

  radio.startListening(); // RX mode

}

void loop(){

  uint8_t pipe;
  uint8_t last_value;

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

  if (last_value != payload) {
    direction(payload);
    last_value = payload;
  }
}
