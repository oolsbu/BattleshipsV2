#include "comms/connect.h"
#include "comms/protocol.h"
#include "config.h"

#if ROLE_IS_MASTER
#include "roles/master.h"
#else
#include "roles/slave.h"
#endif

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <cstring>

bool sendRaw(const uint8_t *data, uint8_t len)
{
    return esp_now_send(other, data, len) == ESP_OK;
}

// Called when data is received
static void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    if (len < static_cast<int>(sizeof(MessageHeader)))
    {
        return;
    }

#if ROLE_IS_MASTER
    masterRecievedMessage(const_cast<uint8_t *>(incomingData), static_cast<uint8_t>(len));
#else
    slaveRecievedMessage(const_cast<uint8_t *>(incomingData), static_cast<uint8_t>(len));
#endif
}

// Called when data is sent
static void onSent(const uint8_t *mac, esp_now_send_status_t status)
{
    Serial.print("Send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "Fail");
}

void ESPNOW_setup()
{
    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);
    Serial.print("MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.print("ESP-NOW channel: ");
    Serial.println(ESPNOW_CHANNEL);
    Serial.printf("Peer: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  other[0], other[1], other[2], other[3], other[4], other[5]);

    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW init failed");
        return;
    }
    Serial.println("ESP-NOW init ok");

    esp_now_register_recv_cb(onReceive);
    esp_now_register_send_cb(onSent);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, other, 6);
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.channel = ESPNOW_CHANNEL;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Failed to add ESP-NOW peer");
    }
    else
    {
        Serial.println("ESP-NOW peer added");
    }
}
