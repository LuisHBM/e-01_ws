#include "WiFi.h"
#include <ros.h>
#include <geometry_msgs/Twist.h>
#include <SPI.h>
#include "RF24.h"

#define CE_PIN 4
#define CSN_PIN 5

const uint8_t R_PWM =  12;
const uint8_t R_BACK = 14;
const uint8_t R_FORW = 27;

const uint8_t L_PWM =  33;
const uint8_t L_BACK = 25;
const uint8_t L_FORW = 26;

const uint8_t channel_L =0;
const uint8_t channel_R= 1;

float left_wheel;
float right_wheel;


IPAddress server(192, 168, 100, 7);
uint16_t serverPort = 11411;
const char*  ssid = "";
const char*  password = "";


void cmdVel_to_pwm_cb( const geometry_msgs::Twist& velocity_msg);
ros::NodeHandle  nh;
ros::Subscriber<geometry_msgs::Twist> sub("/cmd_vel", &cmdVel_to_pwm_cb );


byte address[][6] = {"1Node", "2Node"};

RF24 radio(CE_PIN, CSN_PIN);

bool radioNumber = 1;   //Identify the dispositive

uint8_t payload = 0; // 1 -> UP, 2 -> DOWN, 3 -> LEFT, 4 -> RIGHT
int count = 0;

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

void setupWiFi()
{  
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) { delay(500);Serial.print("."); }
   Serial.print("SSID: ");
   Serial.println(WiFi.SSID());
   Serial.print("IP:   ");
   Serial.println(WiFi.localIP());

}

void direction(){
    digitalWrite(L_FORW, left_wheel >0 );
    digitalWrite(L_BACK,left_wheel < 0);
    digitalWrite(R_FORW,right_wheel > 0 );
    digitalWrite(R_BACK,right_wheel < 0);
}

void speed (){
    ledcWrite(channel_R, 200);  
    ledcWrite(channel_L, 200);
}

void stop()
{  
   ledcWrite(channel_R, 0);  
   ledcWrite(channel_L, 0);
}

void cmdVel_to_pwm_cb( const geometry_msgs::Twist& velocity_msg){
    right_wheel = (velocity_msg.linear.x + velocity_msg.angular.z ) / 2 ;
    left_wheel = (velocity_msg.linear.x - velocity_msg.angular.z ) /2 ;
    direction();
    speed();

    if ( right_wheel > 0 && left_wheel > 0)
       payload = 1;
    else if ( right_wheel < 0 && left_wheel < 0 )
      payload = 2;
    else if ( right_wheel > 0 && left_wheel < 0 )
      payload = 3;
    else if ( right_wheel < 0 && left_wheel > 0 )
      payload = 4;
    else
      payload = 0;

    if ( velocity_msg.linear.x ==0.0 & velocity_msg.angular.z ==0.0){
        stop();
    }
    Serial.print(left_wheel);Serial.print(" / ");Serial.println(right_wheel);

}

void setup() {

  Serial.begin(115200);
  setupWiFi();
  nh.getHardware()->setConnection(server, serverPort);
  nh.initNode();
  nh.subscribe(sub);

  pin_def();
  stop();

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

  nh.spinOnce();

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

}