#include "logic.h"

void placeBoats(
	Cell (&board)[BOARD_SIZE][BOARD_SIZE],
	Ship (&ships)[SHIPS],
	uint8_t &ships_left,
	uint8_t &cursor_x,
	uint8_t &cursor_y,
	int &joyBtn,
	int &btn,
	bool &placing_horizontal)
{
	if (ships_left == 0)
		return;
	if (btn)
	{
		placeConfirmedBoat(board, ships, ships_left, cursor_x, cursor_y, placing_horizontal);
	}
	else
	{
		Ship &ship = ships[SHIPS - ships_left];
		ship.x = cursor_x;
		ship.y = cursor_y;
		ship.length = SIZES[SHIPS - ships_left];
		ship.horizontal = placing_horizontal;
		ship.placed = false;
	}
	if (joyBtn)
	{
		placing_horizontal = !placing_horizontal;
	}
}
void placeConfirmedBoat(
	Cell (&board)[BOARD_SIZE][BOARD_SIZE],
	Ship (&ships)[SHIPS],
	uint8_t &ships_left,
	uint8_t &cursor_x,
	uint8_t &cursor_y,
	bool &placing_horizontal)
{
	Ship &ship = ships[SHIPS - ships_left];
	ship.x = cursor_x;
	ship.y = cursor_y;
	ship.length = SIZES[SHIPS - ships_left];
	ship.horizontal = placing_horizontal;
	ship.placed = true;

	for (uint8_t i = 0; i < ship.length; ++i)
	{
		uint8_t x = ship.x + (ship.horizontal ? i : 0);
		uint8_t y = ship.y + (ship.horizontal ? 0 : i);
		board[y][x] = CELL_SHIP;
	}

	--ships_left;
}