
#include <Arduino.h>
#include "config.h"
#include "game/state.h"
#include "display/led_matrix.h"
#include "comms/connect.h"
#include "game/controller.h"

#if ROLE_IS_MASTER
#include "roles/master.h"
#else
#include "roles/slave.h"
#endif

GamePhase game_phase = PHASE_INIT;
int x, y, joyBtn, btn;

void setup()
{
  Serial.begin(115200);
#if defined(ESP32)
  analogReadResolution(12);
#endif
  ledSetup();
  ESPNOW_setup();
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(JOY_SW_PIN, INPUT_PULLUP);
}

void loop()
{
#if ROLE_IS_MASTER
  getReadings(x, y, joyBtn, btn);
  masterLoop(game_phase, x, y, joyBtn, btn);
  // Serial.printf("Master - Cursor: (%d, %d), JoyBtn: %d, Btn: %d, GamePhase: %d\n", x, y, joyBtn, btn, game_phase);
#else
  getReadings(x, y, joyBtn, btn);
  slaveLoop(game_phase, x, y, joyBtn, btn);
#endif
}
