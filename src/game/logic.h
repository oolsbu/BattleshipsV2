#pragma once
#include "game/state.h"

void placeBoats(
    Cell (&board)[BOARD_SIZE][BOARD_SIZE],
    Ship (&ships)[SHIPS],
    uint8_t &ships_left,
    uint8_t &cursor_x,
    uint8_t &cursor_y,
    int &joyBtn,
    int &btn,
    bool &placing_horizontal);

void placeConfirmedBoat(
    Cell (&board)[BOARD_SIZE][BOARD_SIZE],
    Ship (&ships)[SHIPS],
    uint8_t &ships_left,
    uint8_t &cursor_x,
    uint8_t &cursor_y,
    bool &placing_horizontal);

bool canPlaceShip(
    const Cell (&board)[BOARD_SIZE][BOARD_SIZE],
    uint8_t length,
    uint8_t x,
    uint8_t y,
    bool horizontal);

uint8_t shipLengthForIndex(uint8_t index);