#pragma once
#include "game/state.h"

void masterSetup();
void masterLoop(GamePhase &phase, int dx, int dy, int joyBtn, int btn);
void masterRecievedMessage(uint8_t *incomingData, uint8_t len);
