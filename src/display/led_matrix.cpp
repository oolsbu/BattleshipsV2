#include "display/led_matrix.h"
#include <cstring>

CRGB leds[NUM_LEDS];

void ledSetup()
{
    FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(10);
    FastLED.setDither(false);
    FastLED.clear();
    FastLED.show();
}

int XY(int x, int y)
{
    if (y % 2 == 0)
    {
        return y * WIDTH + x;
    }
    return y * WIDTH + (WIDTH - 1 - x);
}

void showFrame(const Cell frame[BOARD_SIZE][BOARD_SIZE], bool myBoard, bool showCursor, uint8_t cursorX, uint8_t cursorY,
               uint8_t previewLength, bool previewHorizontal, bool previewValid)
{
    static CRGB lastFrame[NUM_LEDS];
    static bool hasLastFrame = false;

    fill_solid(leds, NUM_LEDS, CRGB::Black);

    const CRGB borderColor = CRGB(40, 40, 40);
    const int borderMinX = BOARD_OFFSET_X - 1;
    const int borderMaxX = BOARD_OFFSET_X + BOARD_SIZE;
    const int borderMinY = BOARD_OFFSET_Y - 1;
    const int borderMaxY = BOARD_OFFSET_Y + BOARD_SIZE;
    for (int x = borderMinX; x <= borderMaxX; x++)
    {
        if (x >= 0 && x < WIDTH)
        {
            if (borderMinY >= 0 && borderMinY < HEIGHT)
                leds[XY(x, borderMinY)] = borderColor;
            if (borderMaxY >= 0 && borderMaxY < HEIGHT)
                leds[XY(x, borderMaxY)] = borderColor;
        }
    }
    for (int y = borderMinY; y <= borderMaxY; y++)
    {
        if (y >= 0 && y < HEIGHT)
        {
            if (borderMinX >= 0 && borderMinX < WIDTH)
                leds[XY(borderMinX, y)] = borderColor;
            if (borderMaxX >= 0 && borderMaxX < WIDTH)
                leds[XY(borderMaxX, y)] = borderColor;
        }
    }
    for (uint8_t y = 0; y < BOARD_SIZE; y++)
    {
        for (uint8_t x = 0; x < BOARD_SIZE; x++)
        {
            leds[XY(BOARD_OFFSET_X + x, BOARD_OFFSET_Y + y)] = cellColor(frame[y][x], myBoard);
        }
    }

    if (previewLength > 0)
    {
        const CRGB previewColor = previewValid ? CRGB(0, 70, 180) : CRGB(120, 0, 0);
        for (uint8_t i = 0; i < previewLength; i++)
        {
            uint8_t px = cursorX + (previewHorizontal ? i : 0);
            uint8_t py = cursorY + (previewHorizontal ? 0 : i);
            if (px < BOARD_SIZE && py < BOARD_SIZE)
            {
                leds[XY(BOARD_OFFSET_X + px, BOARD_OFFSET_Y + py)] = previewColor;
            }
        }
    }
    if (showCursor)
    {
        leds[XY(BOARD_OFFSET_X + cursorX, BOARD_OFFSET_Y + cursorY)] = CRGB::Yellow;
    }

    bool changed = !hasLastFrame;
    if (!changed)
    {
        for (uint16_t i = 0; i < NUM_LEDS; i++)
        {
            if (leds[i] != lastFrame[i])
            {
                changed = true;
                break;
            }
        }
    }

    if (changed)
    {
        FastLED.show();
        memcpy(lastFrame, leds, sizeof(lastFrame));
        hasLastFrame = true;
    }
}
