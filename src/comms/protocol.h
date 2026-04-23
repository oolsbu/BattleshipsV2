#include <stdint.h>


enum MessageType : uint8_t {
    MSG_PLACE_SHIP,
    MSG_PLACEMENT_DONE,
    MSG_AIM,
    MSG_SHOOT,
    MSG_GAME_STATE
};

struct __attribute__((packed)) MessageHeader {
    uint8_t type;
};

struct __attribute__((packed)) PlaceShipPayload {
    uint8_t ship_index;
    uint8_t x;
    uint8_t y;
    uint8_t horizontal;
};

struct __attribute__((packed)) PlaceShipMessage {
    MessageHeader header;
    PlaceShipPayload payload;
};

struct __attribute__((packed)) PlacementDoneMessage {
    MessageHeader header;
};