#include <FastLED.h>
#include "config.h"
#include "game/state.h"

inline CRGB cellColor(Cell cell) {
  switch (cell) {
    case CELL_EMPTY: return CRGB::Black;
    case CELL_SHIP:  return CRGB::Blue;
    case CELL_HIT:   return CRGB::Red;
    case CELL_MISS:  return CRGB::White;
    case CELL_SUNK:  return CRGB::Orange;
    default:         return CRGB::Black;
  }
}