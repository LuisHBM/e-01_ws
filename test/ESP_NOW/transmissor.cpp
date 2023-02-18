#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

//=================================

struct person_info {
  int age;
  bool man;
  unsigned int count = 0;
};

person_info myInfo;
//==================================

//MAC Address de envio;
uint8_t broadcast_address[] = {0x40,0x22,0xD8,0x57,0xF4,0x8C}; //40:22:D8:57:F4:8C

esp_now_peer_info_t peerInfo;

//Callback func
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\nStatus do ultimo pacote enviado:");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? " Enviado com sucesso!\n" : " Falha ao enviar.\n");
}

void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  //Inicializando ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.print("Erro ao inicializar ESP-NOW.");
    while(true){}
  }

  //Registrando callback
  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcast_address, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  //Adicionando elemento
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.print("Falha ao adicionar o peer.");
    while(true) {}
  }
}

void loop() {
  myInfo.man = true;
  myInfo.age = 20;
  myInfo.count++;

  esp_err_t result = esp_now_send(broadcast_address, (uint8_t*) &myInfo, sizeof(myInfo));
  Serial.print(result == ESP_OK ? "Preparado para envio..." : "Falha no preparo de envio.");

  delay(2000);
}