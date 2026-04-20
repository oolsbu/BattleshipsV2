#pragma once
#include <stdint.h>
#include "config.h"

constexpr uint8_t GRID  = BOARD_SIZE;
constexpr uint8_t SHIP_TYPE_COUNT = sizeof(SIZES) / sizeof(SIZES[0]);

constexpr uint8_t sumCounts(const uint8_t *counts, uint8_t n, uint8_t i = 0) {
    return (i >= n) ? 0 : counts[i] + sumCounts(counts, n, i + 1);
}
constexpr uint8_t SHIPS = sumCounts(COUNTS, SHIP_TYPE_COUNT);

enum Cell : uint8_t { CELL_EMPTY, CELL_SHIP, CELL_HIT, CELL_MISS, CELL_SUNK };

enum GamePhase : uint8_t {
    PHASE_PAIRING,
    PHASE_PLACING,
    PHASE_SHOOTING,
    PHASE_WAITING,
    PHASE_GAMEOVER
};

struct Ship {
    uint8_t x;
    uint8_t y;
    uint8_t length;
    bool horizontal;
    bool placed;
    uint8_t hits;

    bool isSunk() const { return hits >= length; }
};

struct GameState {
    Cell my_board[BOARD_SIZE][BOARD_SIZE] = {};
    Ship my_ships[SHIPS] = {};
    uint8_t my_ships_left = SHIPS;
    uint8_t my_ships_placed = 0;
    bool my_turn = false;
    uint8_t cursor_x = 0;
    uint8_t cursor_y = 0;
    uint8_t placing_idx = 0;
    bool placing_horiz = true;
};
