#include "protocol.h"
#include "roles/master.h"
#include "roles/slave.h"
#include <ESP8266WiFi.h>
#include <espnow.h>

extern User role;

//My adress: 0x24, 0x6F, 0x28, 0x11, 0x22, 0x33
uint8_t peerAddress[] = {0x24, 0x6F, 0x28, 0x11, 0x22, 0x33};

bool sendRaw(const uint8_t* data, uint8_t len) {
  return esp_now_send(peerAddress, const_cast<uint8_t*>(data), len) == 0;
}

// Called when data is received
void onReceive(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  if (len < sizeof(MessageHeader)) {
    return;
  }

  if (role == User::Master) {
    masterRecievedMessage(incomingData, len);
  }
  else {
    slaveRecievedMessage(incomingData, len);
  }
}

// Called when data is sent
void onSent(uint8_t *mac, uint8_t status) {
  Serial.print("Send status: ");
  Serial.println(status == 0 ? "OK" : "Fail");
}

void ESPNOW_setup() {
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_recv_cb(onReceive);
  esp_now_register_send_cb(onSent);

  esp_now_add_peer(peerAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
}

