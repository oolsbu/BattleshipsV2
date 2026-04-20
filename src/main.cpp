#include <ESP8266WiFi.h>
#include <espnow.h>
#include <EEPROM.h>
#include <Arduino.h>
#include "config.h"
#include "game/state.h"
#include "display/led_matrix.h"

GamePhase game_phase = PHASE_PLACING;

GameState game_state;

void setup() {
  Serial.begin(115200);
  ledSetup();
}

void loop() {
  if(game_phase == PHASE_PLACING) {
    if (game_state.my_ships_placed == SHIPS) {
      
      game_phase = PHASE_WAITING;
    }
  } else if(game_phase == PHASE_PAIRING) {
  }
  else if(game_phase == PHASE_WAITING) {
  }
  else if(game_phase == PHASE_SHOOTING) {
  }
  else if(game_phase == PHASE_GAMEOVER) {
  }
  
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}