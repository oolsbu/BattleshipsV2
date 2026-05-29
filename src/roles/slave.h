#pragma once
#include "game/state.h"

struct GameState
{
    Cell my_board[BOARD_SIZE][BOARD_SIZE] = {};
    Ship my_ships[SHIPS] = {};
    uint8_t my_ships_to_place = SHIPS;
    uint8_t my_ships_left = SHIPS;
    uint8_t master_ships_left = SHIPS;
    uint8_t my_ships_placed = 0;
    bool my_turn = false;
    uint8_t cursor_x = 0;
    uint8_t cursor_y = 0;
    uint8_t placing_idx = 0;
    bool placing_horizontal = true;
};

void slaveSetup();
void slaveLoop(GamePhase &phase, int dx, int dy, int joyBtn, int btn);
void slaveRecievedMessage(uint8_t *incomingData, uint8_t len);
bool sendPlaceShip(uint8_t ship_index, uint8_t x, uint8_t y, bool horizontal);
bool sendPlacementDone();