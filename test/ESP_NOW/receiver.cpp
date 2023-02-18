#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

//=================================
uint8_t dados = 0;

struct person_info {
  int age;
  bool man;
  unsigned int count = 0;
};

person_info myInfo;
//==================================

void OnDataRecv( const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myInfo, incomingData, sizeof(myInfo));
  Serial.print("Dados recebidos:");
  Serial.println(len);
  Serial.print("Idade:");
  Serial.println(myInfo.age);
  Serial.print("Sexo:");
  Serial.println(myInfo.man ? "Homem" : "Mulher");
  Serial.print("Contador:");
  Serial.println(myInfo.count);
  Serial.println();
}

void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  //Inicializando ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.print("Erro ao inicializar ESP-NOW.");
    while(true){}
  }

  //Registrando func callback
  esp_now_register_recv_cb(OnDataRecv);

}

void loop() {
  
}