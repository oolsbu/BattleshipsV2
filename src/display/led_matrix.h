#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <FastLED.h>
#include "config.h"
#include "game/state.h"
#include "display/render.h"

#define LED_PIN     6
#define WIDTH       16
#define HEIGHT      16
#define NUM_LEDS    (WIDTH * HEIGHT)
#define COLOR_ORDER RGB

extern CRGB leds[NUM_LEDS];

inline void ledSetup() {
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(10);
  FastLED.clear();
}

inline int XY(int x, int y) {
  if (y % 2 == 0) {
    return y * WIDTH + x;
  } else {
    return y * WIDTH + (WIDTH - 1 - x);
  }
}



inline void showFrame(const Cell frame[BOARD_SIZE][BOARD_SIZE], bool myBoard, bool showCursor = false, uint8_t cursorX = 0, uint8_t cursorY = 0) {
  FastLED.clear();
  for (uint8_t y = 0; y < BOARD_SIZE; y++) {
    for (uint8_t x = 0; x < BOARD_SIZE; x++) {
      leds[XY(BOARD_OFFSET_X + x, BOARD_OFFSET_Y + y)] = cellColor(frame[x][y], myBoard);
    }
  }
  if (showCursor) {
    leds[XY(BOARD_OFFSET_X + cursorX, BOARD_OFFSET_Y + cursorY)] = CRGB::Yellow;
  }
  FastLED.show();
}

#endif