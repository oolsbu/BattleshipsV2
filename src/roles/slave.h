#include "master.h"

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