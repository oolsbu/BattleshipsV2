#pragma once
#include <stdint.h>

enum MessageType : uint8_t
{
    MSG_PLACE_SHIP,
    MSG_PLACEMENT_DONE,
    MSG_AIM,
    MSG_SHOOT,
    MSG_GAME_STATE,
    MSG_SHIP_REVEAL,
    MSG_POWER_SHOOT,
    MSG_POWER_STATE
};

#define MAX_SHIPS_IN_REVEAL 8

constexpr uint8_t NO_SHOT = 0xFF;

struct __attribute__((packed)) MessageHeader
{
    uint8_t type;
};

struct __attribute__((packed)) PlaceShipPayload
{
    uint8_t ship_index;
    uint8_t x;
    uint8_t y;
    uint8_t horizontal;
};

struct __attribute__((packed)) PlaceShipMessage
{
    MessageHeader header;
    PlaceShipPayload payload;
};

struct __attribute__((packed)) PlacementDoneMessage
{
    MessageHeader header;
};

struct __attribute__((packed)) AimPayload
{
    uint8_t x;
    uint8_t y;
    uint8_t powerup_active;
};

struct __attribute__((packed)) AimMessage
{
    MessageHeader header;
    AimPayload payload;
};

struct __attribute__((packed)) ShootPayload
{
    uint8_t x;
    uint8_t y;
};

struct __attribute__((packed)) ShootMessage
{
    MessageHeader header;
    ShootPayload payload;
};

struct __attribute__((packed)) GameStatePayload
{
    uint8_t master_turn;
    uint8_t master_ships_left;
    uint8_t slave_ships_left;
    uint8_t shooter;
    uint8_t shot_x;
    uint8_t shot_y;
    uint8_t shot_result;
};

struct __attribute__((packed)) GameStateMessage
{
    MessageHeader header;
    GameStatePayload payload;
};

struct __attribute__((packed)) ShipRevealEntry
{
    uint8_t x;
    uint8_t y;
    uint8_t length;
    uint8_t horizontal;
};

struct __attribute__((packed)) ShipRevealMessage
{
    MessageHeader header;
    uint8_t count;
    ShipRevealEntry ships[MAX_SHIPS_IN_REVEAL];
};

struct __attribute__((packed)) PowerShootMessage
{
    MessageHeader header;
    uint8_t x;
    uint8_t y;
};

struct __attribute__((packed)) PowerStateEntry
{
    uint8_t x;
    uint8_t y;
    uint8_t result;
};

struct __attribute__((packed)) PowerStateMessage
{
    MessageHeader header;
    uint8_t shooter;
    uint8_t master_turn;
    uint8_t master_ships_left;
    uint8_t slave_ships_left;
    uint8_t count;
    PowerStateEntry results[9];
};