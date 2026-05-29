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
	const uint8_t shipIndex = SHIPS - ships_left;
	const uint8_t length = shipLengthForIndex(shipIndex);
	if (btn)
	{
		placeConfirmedBoat(board, ships, ships_left, cursor_x, cursor_y, placing_horizontal);
	}
	else
	{
		Ship &ship = ships[SHIPS - ships_left];
		ship.x = cursor_x;
		ship.y = cursor_y;
		ship.length = length;
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
	const uint8_t shipIndex = SHIPS - ships_left;
	const uint8_t length = shipLengthForIndex(shipIndex);
	if (!canPlaceShip(board, length, cursor_x, cursor_y, placing_horizontal))
		return;

	Ship &ship = ships[SHIPS - ships_left];
	ship.x = cursor_x;
	ship.y = cursor_y;
	ship.length = length;
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

bool canPlaceShip(
	const Cell (&board)[BOARD_SIZE][BOARD_SIZE],
	uint8_t length,
	uint8_t x,
	uint8_t y,
	bool horizontal)
{
	if (length == 0)
		return false;

	if (horizontal)
	{
		if (x + length > BOARD_SIZE)
			return false;
	}
	else
	{
		if (y + length > BOARD_SIZE)
			return false;
	}

	for (uint8_t i = 0; i < length; ++i)
	{
		uint8_t cx = x + (horizontal ? i : 0);
		uint8_t cy = y + (horizontal ? 0 : i);
		if (board[cy][cx] == CELL_SHIP)
			return false;
	}

	return true;
}

uint8_t shipLengthForIndex(uint8_t index)
{
	uint8_t remaining = index;
	for (uint8_t i = 0; i < SHIP_TYPE_COUNT; ++i)
	{
		if (remaining < COUNTS[i])
		{
			return SIZES[i];
		}
		remaining -= COUNTS[i];
	}
	return SIZES[SHIP_TYPE_COUNT - 1];
}