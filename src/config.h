#ifndef CONFIG
#define CONFIG

#include <Arduino.h>

const uint8_t SIZES[] = {4, 3, 2};
const uint8_t COUNTS[] = {1, 2, 3};

uint8_t other[] = {0xC8, 0x2B, 0x96, 0x22, 0xCB, 0x00};

#define SHOW_OPPONENT_AIM 1

#define RANDOM_FIRST_SHOOTER 1

#define BOARD_SIZE 10

#define BOARD_OFFSET_X ((16 - BOARD_SIZE) / 2)
#define BOARD_OFFSET_Y ((16 - BOARD_SIZE) / 2)

#endif