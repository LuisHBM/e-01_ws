#include <Arduino.h>

#define VRX_PIN  32
#define VRY_PIN  35

int xValue = 0; 
int yValue = 0; 

void setup() {
  Serial.begin(115200);
  pinMode(VRX_PIN, INPUT);
  pinMode(VRY_PIN, INPUT);
}

void loop() {
  // read analog X and Y analog values
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);

  // print data to Serial Monitor on Arduino IDE
  Serial.print("x = ");
  Serial.print(xValue);
  Serial.print(", y = ");
  Serial.println(yValue);
  delay(200);
}
