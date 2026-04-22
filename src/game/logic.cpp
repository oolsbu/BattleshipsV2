#include "logic.h"

void placeBoats(
	Cell (&board)[BOARD_SIZE][BOARD_SIZE],
	Ship (&ships)[SHIPS],
	uint8_t &ships_left,
	uint8_t &cursor_x,
	uint8_t &cursor_y,
	bool &placing_horizontal
) {
	if (ships_left == 0) return;

	uint8_t ship_i = 0;
	uint8_t y = 0;
	for (uint8_t t = 0; t < SHIP_TYPE_COUNT; ++t) {
		for (uint8_t c = 0; c < COUNTS[t] && ship_i < SHIPS; ++c) {
			const uint8_t len = SIZES[t];
			for (uint8_t x = 0; x < len; ++x) {
				board[y][x] = CELL_SHIP;
			}

			ships[ship_i] = {0, y, len, true, true, 0};
			++ship_i;
			y += 2;
		}
	}

	ships_left = 0;
	cursor_x = 0;
	cursor_y = 0;
	placing_horizontal = true;
}
