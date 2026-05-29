#ifndef CONFIG
#define CONFIG

#include <Arduino.h>

// Compile-time role selection: set to 1 for master, 0 for slave.
#define ROLE_IS_MASTER 1

const uint8_t SIZES[] = {4, 3, 2};
const uint8_t COUNTS[] = {1, 2, 3};

extern uint8_t other[6];

#define SHOW_OPPONENT_AIM 1

#define RANDOM_FIRST_SHOOTER 0

#define ESPNOW_CHANNEL 1

#define BOARD_SIZE 10

#define BOARD_OFFSET_X ((16 - BOARD_SIZE) / 2)
#define BOARD_OFFSET_Y ((16 - BOARD_SIZE) / 2)

#define LED_PIN 5
#define JOY_X_PIN 36
#define JOY_Y_PIN 39
#define JOY_SW_PIN 4
#define BTN_PIN 2
#define ADC_MAX 4095
#define JOY_LOW_THRESHOLD 1000
#define JOY_HIGH_THRESHOLD 3000
#define JOY_BTN_THRESHOLD 3500
#endif