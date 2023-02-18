#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

#define X_AXIS_PIN 32 // for the VRX port
#define Y_AXIS_PIN 33 // for the VRY port

#define DELAY_TIME 50 //Defines the loop delay
#define DEADZONE 200 //Defines the joystick deadzone value

uint8_t receiverMacAddress[] = {0x40,0x22,0xD8,0x57,0xF4,0x8C};  //40:22:D8:57:F4:8C

struct PacketData
{
  bool verticalMov; // true = UP, false = DOWN
  bool horizontalMov; // true = right, false = left
  bool verticalStop;
  bool horizontalStop;
};
PacketData data;

void trackingMovement(int value, bool direction) // value = 0-4095, direction = vertical(true) or horizontal(false)
{
  if (direction) {

    Serial.print("x = ");
    Serial.print(value);
    data.verticalStop = false;

    if (value >= (2000 + DEADZONE)) {
      data.verticalMov = true; // foward
    } else if (value < (1900 - DEADZONE)) {
      data.verticalMov = false; //backward
    } else {
      data.verticalStop = true; //x-axis = 0
    }
  } else {

    Serial.print(", y = ");
    Serial.println(value);
    data.horizontalStop = false;

    if (value >= (2000 + DEADZONE)) {
      data.horizontalMov = true; //right
    } else if (value < (1900 - DEADZONE)) {
      data.horizontalMov = false; //left
    } else {
      data.horizontalStop = true; //y-axis = 0
    }
  }
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t ");
  Serial.println(status);
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Message sent" : "Message failed");
}

void setup() 
{
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  else
  {
    Serial.println("Succes: Initialized ESP-NOW");
  }
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  else
  {
    Serial.println("Succes: Added peer");
  } 
  
  esp_now_register_send_cb(OnDataSent);   
}
 
void loop() 
{
  //Getting the info from the joystick
  trackingMovement(analogRead(X_AXIS_PIN), true);
  trackingMovement(analogRead(Y_AXIS_PIN), false);
  Serial.println("");
  
  //Sending data
  esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *) &data, sizeof(data));
  if (result == ESP_OK) 
  {
    Serial.println("Sent with success");
  }
  else 
  {
    Serial.println("Error sending the data");
  }
  Serial.print("x = ");
  Serial.print(data.verticalMov);
  Serial.print(", y = ");
  Serial.print(data.horizontalMov);
  Serial.print(", x(stop) = ");
  Serial.print(data.verticalStop);
  Serial.print(", y(stop) = ");
  Serial.print(data.horizontalStop);
  Serial.println("");
  
  //Delay for more clear Serial Monitor view. Note: DELAY_TIME must always be less then 1000ms/1sec.
  delay(DELAY_TIME);
}