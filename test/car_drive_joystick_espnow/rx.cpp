#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

//Right motor
int enableRightMotor=26; 
int rightMotorPin1=13;
int rightMotorPin2=12;
//Left motor
int enableLeftMotor=25;
int leftMotorPin1=14;
int leftMotorPin2=27;

#define MAX_MOTOR_SPEED 200

const int PWMFreq = 1000; //Hz
const int PWMResolution = 8;
const int rightMotorPWMSpeedChannel = 4;
const int leftMotorPWMSpeedChannel = 5;

#define SIGNAL_TIMEOUT 1000  // This is signal timeout in milli seconds. We will reset the data if no signal
unsigned long lastRecvTime = 0;

struct PacketData
{
  bool verticalMov; // true = UP, false = DOWN
  bool horizontalMov; // true = right, false = left
  bool verticalStop;
  bool horizontalStop;
};
PacketData receiverData;

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  if (rightMotorSpeed < 0)
  {
    digitalWrite(rightMotorPin1,LOW);
    digitalWrite(rightMotorPin2,HIGH);    
  }
  else if (rightMotorSpeed > 0)
  {
    digitalWrite(rightMotorPin1,HIGH);
    digitalWrite(rightMotorPin2,LOW);      
  }
  else
  {
    digitalWrite(rightMotorPin1,LOW);
    digitalWrite(rightMotorPin2,LOW);      
  }
  
  if (leftMotorSpeed < 0)
  {
    digitalWrite(leftMotorPin1,LOW);
    digitalWrite(leftMotorPin2,HIGH);    
  }
  else if (leftMotorSpeed > 0)
  {
    digitalWrite(leftMotorPin1,HIGH);
    digitalWrite(leftMotorPin2,LOW);      
  }
  else
  {
    digitalWrite(leftMotorPin1,LOW);
    digitalWrite(leftMotorPin2,LOW);      
  } 

  ledcWrite(rightMotorPWMSpeedChannel, abs(rightMotorSpeed));
  ledcWrite(leftMotorPWMSpeedChannel, abs(leftMotorSpeed));    
}

void simpleMovements()
{
  bool stop = true;
  
  if (!receiverData.verticalStop) {
    stop = false;
    if (receiverData.verticalMov)
      rotateMotor(MAX_MOTOR_SPEED, MAX_MOTOR_SPEED); //foward
    else
      rotateMotor(-MAX_MOTOR_SPEED, -MAX_MOTOR_SPEED); //backward
  }

  if (!receiverData.horizontalStop) {
    stop = false;
    if (receiverData.horizontalMov)       
      rotateMotor(-MAX_MOTOR_SPEED, MAX_MOTOR_SPEED); //right
    else
      rotateMotor(MAX_MOTOR_SPEED, -MAX_MOTOR_SPEED); //left
  }

  if (stop)
    rotateMotor(0,0); //stop
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  if (len == 0)
  {
    return;
  }
  memcpy(&receiverData, incomingData, sizeof(receiverData));
  
  simpleMovements();
  
  lastRecvTime = millis();   
}

void setUpPinModes()
{
  pinMode(enableRightMotor,OUTPUT);
  pinMode(rightMotorPin1,OUTPUT);
  pinMode(rightMotorPin2,OUTPUT);
  
  pinMode(enableLeftMotor,OUTPUT);
  pinMode(leftMotorPin1,OUTPUT);
  pinMode(leftMotorPin2,OUTPUT);

  //Set up PWM for motor speed
  ledcSetup(rightMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcSetup(leftMotorPWMSpeedChannel, PWMFreq, PWMResolution);  
  ledcAttachPin(enableRightMotor, rightMotorPWMSpeedChannel);
  ledcAttachPin(enableLeftMotor, leftMotorPWMSpeedChannel); 
  
  rotateMotor(0, 0);
}

void setup() 
{
  setUpPinModes();
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() 
{
  //Check Signal lost.
  unsigned long now = millis();
  if ( now - lastRecvTime > SIGNAL_TIMEOUT ) 
  {
    rotateMotor(0, 0);
  }
}