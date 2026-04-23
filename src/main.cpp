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
#include "game/controller.cpp"


GamePhase game_phase = PHASE_INIT;
int x, y, joyBtn, btn;

User role = User::Master;

void setup() {
  Serial.begin(115200);
  ledSetup();
  ESPNOW_setup();
  pinMode(2, INPUT);

}

void loop() {
  if (role == User::Master) {
    getReadings(x, y, joyBtn, btn);
    masterLoop(game_phase, x, y, joyBtn, btn);
  }
  else {
    // slaveLoop(game_phase, game_state);
  }
  
}
