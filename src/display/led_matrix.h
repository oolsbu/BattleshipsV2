#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <FastLED.h>
#include "config.h"
#include "game/state.h"
#include "display/render.h"

#define WIDTH 16
#define HEIGHT 16
#define NUM_LEDS (WIDTH * HEIGHT)
#define COLOR_ORDER RGB

extern CRGB leds[NUM_LEDS];

void ledSetup();
int XY(int x, int y);
void showFrame(const Cell frame[BOARD_SIZE][BOARD_SIZE], bool myBoard, bool showCursor = false, uint8_t cursorX = 0, uint8_t cursorY = 0,
               uint8_t previewLength = 0, bool previewHorizontal = true, bool previewValid = true);
void showEndAnimation(bool won, const Cell opponentBoard[BOARD_SIZE][BOARD_SIZE]);

#endif