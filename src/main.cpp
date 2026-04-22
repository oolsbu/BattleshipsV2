#include <ESP8266WiFi.h>
#include <espnow.h>
#include <EEPROM.h>
#include <Arduino.h>
#include "config.h"
#include "game/state.h"
#include "display/led_matrix.h"
#include "roles/master.h"
#include "roles/slave.h"
#include "comms/connect.h"


GamePhase game_phase = PHASE_INIT;

User role = User::Master;

void setup() {
  Serial.begin(115200);
  ledSetup();
  ESPNOW_setup();
}

void loop() {
  if (role == User::Master) {
    masterLoop(game_phase);
  }
  else {
    // slaveLoop(game_phase, game_state);
  }
  
}
