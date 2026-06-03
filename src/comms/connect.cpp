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

// Circular receive buffer — written by WiFi task, read by main task
#define RECV_BUF_SLOTS 4
static uint8_t recv_bufs[RECV_BUF_SLOTS][250];
static uint8_t recv_lens[RECV_BUF_SLOTS];
static volatile uint8_t recv_write = 0;
static volatile uint8_t recv_read = 0;

// Send confirmation flags — written by WiFi task, polled by main task
static volatile bool send_done = false;
static volatile bool send_ok = false;

// Buffer incoming data; dispatch happens in main task via processPendingMessages()
static void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    if (len < 1 || len > 250)
        return;
    uint8_t next = (recv_write + 1) % RECV_BUF_SLOTS;
    if (next == recv_read)
        return; // buffer full, drop packet
    memcpy(recv_bufs[recv_write], incomingData, (size_t)len);
    recv_lens[recv_write] = (uint8_t)len;
    recv_write = next;
}

static void onSent(const uint8_t *mac, esp_now_send_status_t status)
{
    send_ok = (status == ESP_NOW_SEND_SUCCESS);
    send_done = true;
}

// Must be called from the main task only.
// Blocks until link-layer ACK received, retrying up to MAX_RETRIES times.
bool sendRaw(const uint8_t *data, uint8_t len)
{
    const int MAX_RETRIES = 3;
    const unsigned long TIMEOUT_MS = 300;
    const unsigned long RETRY_DELAY_MS = 100;

    for (int attempt = 0; attempt < MAX_RETRIES; attempt++)
    {
        send_done = false;
        send_ok = false;

        if (esp_now_send(other, data, len) != ESP_OK)
        {
            delay(RETRY_DELAY_MS);
            continue;
        }

        unsigned long start = millis();
        while (!send_done && millis() - start < TIMEOUT_MS)
        {
            delay(1); // yield so WiFi task can fire onSent
        }

        if (send_ok)
            return true;

        Serial.printf("Send failed (attempt %d/%d)\n", attempt + 1, MAX_RETRIES);
        if (attempt < MAX_RETRIES - 1)
            delay(RETRY_DELAY_MS);
    }

    Serial.println("Send failed after all retries");
    return false;
}

// Call from main loop to dispatch any buffered incoming messages.
void processPendingMessages()
{
    while (recv_read != recv_write)
    {
        uint8_t data[250];
        uint8_t len = recv_lens[recv_read];
        memcpy(data, recv_bufs[recv_read], len);
        recv_read = (recv_read + 1) % RECV_BUF_SLOTS;

        if (len < (uint8_t)sizeof(MessageHeader))
            continue;

#if ROLE_IS_MASTER
        masterRecievedMessage(data, len);
#else
        slaveRecievedMessage(data, len);
#endif
    }
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
