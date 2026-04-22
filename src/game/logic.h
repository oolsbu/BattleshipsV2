#pragma once
#include "roles/master.h"

void placeBoats(
    Cell (&board)[BOARD_SIZE][BOARD_SIZE],
    Ship (&ships)[SHIPS],
    uint8_t &ships_left,
    uint8_t &cursor_x,
    uint8_t &cursor_y,
    bool &placing_horizontal
);