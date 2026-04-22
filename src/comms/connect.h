#include <ESP8266WiFi.h>
#include <espnow.h>


//My adress: 0x24, 0x6F, 0x28, 0x11, 0x22, 0x33
uint8_t peerAddress[] = {0x24, 0x6F, 0x28, 0x11, 0x22, 0x33};

typedef struct {
  uint8_t value;
} Message;

Message msg;

// Called when data is received
void onReceive(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&msg, incomingData, sizeof(msg));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Value: ");
    Serial.println(msg.value);
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

void sendMessage(uint8_t value) {
  msg.value = value;
  esp_now_send(peerAddress, (uint8_t *) &msg, sizeof(msg));
}