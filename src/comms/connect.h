#pragma once
#include <stdint.h>

bool sendRaw(const uint8_t *data, uint8_t len);
void ESPNOW_setup();
