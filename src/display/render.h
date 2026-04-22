#include <FastLED.h>
#include "config.h"
#include "game/state.h"

inline CRGB cellColor(Cell cell, bool Myboard) {
  switch (cell) {
    case CELL_EMPTY: return CRGB::Black;
    case CELL_SHIP:  return Myboard ? CRGB::Blue : CRGB::Black;
    case CELL_HIT:   return CRGB::Red;
    case CELL_MISS:  return CRGB::White;
    case CELL_SUNK:  return CRGB::Orange;
    default:         return CRGB::Black;
  }
}