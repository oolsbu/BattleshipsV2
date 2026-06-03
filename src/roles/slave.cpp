#include "slave.h"
#include "game/logic.h"
#include "display/led_matrix.h"
#include "comms/protocol.h"
#include "comms/connect.h"

static SlaveGameState game_state;
static bool placement_done_sent = false;
static bool start_received = false;
static bool has_last_aim = false;
static uint8_t last_aim_x = 0;
static uint8_t last_aim_y = 0;
static unsigned long last_ready_ms = 0;
static bool shot_pending = false; // waiting for game state response after sending MSG_SHOOT
static bool animation_done = false;

static bool sendAim(uint8_t x, uint8_t y)
{
    AimMessage msg = {};
    msg.header.type = MSG_AIM;
    msg.payload.x = x;
    msg.payload.y = y;
    return sendRaw(reinterpret_cast<const uint8_t *>(&msg), sizeof(msg));
}

static bool sendShoot(uint8_t x, uint8_t y)
{
    ShootMessage msg = {};
    msg.header.type = MSG_SHOOT;
    msg.payload.x = x;
    msg.payload.y = y;
    return sendRaw(reinterpret_cast<const uint8_t *>(&msg), sizeof(msg));
}

static uint8_t wrapIndex(int value, int max)
{
    while (value < 0)
    {
        value += max;
    }
    while (value >= max)
    {
        value = max - 1;
    }
    return static_cast<uint8_t>(value);
}

static uint8_t clampIndex(int value, int minValue, int maxValue)
{
    if (value < minValue)
        return static_cast<uint8_t>(minValue);
    if (value > maxValue)
        return static_cast<uint8_t>(maxValue);
    return static_cast<uint8_t>(value);
}

void slaveSetup()
{
    placement_done_sent = false;
    start_received = false;
    has_last_aim = false;
    last_ready_ms = 0;
    shot_pending = false;
    animation_done = false;
    for (uint8_t i = 0; i < SHIPS; ++i)
    {
        game_state.slave_ships[i] = {};
        game_state.slave_ships[i] = {};
    }

    for (uint8_t y = 0; y < BOARD_SIZE; ++y)
    {
        for (uint8_t x = 0; x < BOARD_SIZE; ++x)
        {
            game_state.slave_board[y][x] = CELL_EMPTY;
            game_state.slave_board[y][x] = CELL_EMPTY;
            game_state.master_board[y][x] = CELL_EMPTY;
        }
    }

    game_state.slave_ships_left = SHIPS;
    game_state.slave_ships_left = SHIPS;
    game_state.slave_ships_to_place = SHIPS;
    game_state.slave_ships_to_place = SHIPS;
    game_state.slave_cursor_x = 0;
    game_state.slave_cursor_y = 0;
    game_state.slave_cursor_x = 0;
    game_state.slave_cursor_y = 0;
    game_state.placing_horizontal = true;
}

void slaveLoop(GamePhase &phase, int dx, int dy, int joyBtn, int btn)
{
    switch (phase)
    {
    case PHASE_INIT:
        slaveSetup();
        phase = PHASE_PLACING;
        break;
    case PHASE_PLACING:
    {
        uint8_t previewLength = (game_state.slave_ships_to_place > 0)
                                    ? shipLengthForIndex(SHIPS - game_state.slave_ships_to_place)
                                    : 1;
        int maxX = BOARD_SIZE - (game_state.placing_horizontal ? previewLength : 1);
        int maxY = BOARD_SIZE - (game_state.placing_horizontal ? 1 : previewLength);

        game_state.slave_cursor_x = clampIndex(static_cast<int>(game_state.slave_cursor_x) + dx, 0, maxX);
        game_state.slave_cursor_y = clampIndex(static_cast<int>(game_state.slave_cursor_y) + dy, 0, maxY);
        uint8_t before = game_state.slave_ships_to_place;
        placeBoats(
            game_state.slave_board,
            game_state.slave_ships,
            game_state.slave_ships_to_place,
            game_state.slave_cursor_x,
            game_state.slave_cursor_y,
            joyBtn,
            btn,
            game_state.placing_horizontal);
        if (before != game_state.slave_ships_to_place)
        {
            uint8_t ship_index = SHIPS - before;
            const Ship &ship = game_state.slave_ships[ship_index];
            sendPlaceShip(ship_index, ship.x, ship.y, ship.horizontal);
        }
        {
            previewLength = (game_state.slave_ships_to_place > 0)
                                ? shipLengthForIndex(SHIPS - game_state.slave_ships_to_place)
                                : 0;
            maxX = BOARD_SIZE - (game_state.placing_horizontal ? previewLength : 1);
            maxY = BOARD_SIZE - (game_state.placing_horizontal ? 1 : previewLength);
            game_state.slave_cursor_x = clampIndex(game_state.slave_cursor_x, 0, maxX);
            game_state.slave_cursor_y = clampIndex(game_state.slave_cursor_y, 0, maxY);
            bool previewValid = canPlaceShip(game_state.slave_board, previewLength, game_state.slave_cursor_x, game_state.slave_cursor_y, game_state.placing_horizontal);
            showFrame(game_state.slave_board, true, false, game_state.slave_cursor_x, game_state.slave_cursor_y,
                      previewLength, game_state.placing_horizontal, previewValid);
        }
        if (game_state.slave_ships_to_place == 0)
        {
            if (!placement_done_sent)
            {
                sendPlacementDone();
                placement_done_sent = true;
                last_ready_ms = millis();
            }
            phase = PHASE_WAITING;
        }
    }
    break;
    case PHASE_WAITING:
        if (!start_received && millis() - last_ready_ms > 500)
        {
            sendPlacementDone();
            last_ready_ms = millis();
        }
        if (start_received)
        {
            phase = PHASE_SHOOTING;
        }
        showFrame(game_state.slave_board, true, false);
        break;
    case PHASE_SHOOTING:
        if (game_state.slave_ships_left == 0 || game_state.master_ships_left == 0)
        {
            phase = PHASE_WON;
            int winner = (game_state.slave_ships_left == 0) ? 1 : 0;
            break;
        }
        if (!game_state.master_turn)
        {
            game_state.slave_cursor_x = clampIndex(static_cast<int>(game_state.slave_cursor_x) + dx, 0, BOARD_SIZE - 1);
            game_state.slave_cursor_y = clampIndex(static_cast<int>(game_state.slave_cursor_y) + dy, 0, BOARD_SIZE - 1);
            if (SHOW_OPPONENT_AIM && (!has_last_aim || last_aim_x != game_state.slave_cursor_x || last_aim_y != game_state.slave_cursor_y))
            {
                sendAim(game_state.slave_cursor_x, game_state.slave_cursor_y);
                last_aim_x = game_state.slave_cursor_x;
                last_aim_y = game_state.slave_cursor_y;
                has_last_aim = true;
            }
            if (game_state.slave_ships_to_place != 0)
            {
                phase = PHASE_PLACING;
                return;
            }
            if (btn && !shot_pending)
            {
                sendShoot(game_state.slave_cursor_x, game_state.slave_cursor_y);
                shot_pending = true;
            }
            showFrame(game_state.master_board, false, true, game_state.slave_cursor_x, game_state.slave_cursor_y);
        }
        else
        {
            showFrame(game_state.slave_board, true, SHOW_OPPONENT_AIM, game_state.master_cursor_x, game_state.master_cursor_y);
        }
        break;
    case PHASE_WON:
        if (!animation_done)
        {
            animation_done = true;
            showEndAnimation(game_state.master_ships_left == 0, game_state.master_board);
        }
        break;
    }
}

void slaveRecievedMessage(uint8_t *incomingData, uint8_t len)
{
    if (len < static_cast<uint8_t>(sizeof(MessageHeader)))
    {
        return;
    }

    const MessageHeader *header = reinterpret_cast<const MessageHeader *>(incomingData);
    switch (header->type)
    {
    case MSG_AIM:
    {
        if (len < static_cast<uint8_t>(sizeof(AimMessage)))
            return;
        const AimMessage *msg = reinterpret_cast<const AimMessage *>(incomingData);
        if (msg->payload.x < BOARD_SIZE)
            game_state.master_cursor_x = msg->payload.x;
        if (msg->payload.y < BOARD_SIZE)
            game_state.master_cursor_y = msg->payload.y;
        break;
    }
    case MSG_GAME_STATE:
    {
        if (len < static_cast<uint8_t>(sizeof(GameStateMessage)))
            return;
        const GameStateMessage *msg = reinterpret_cast<const GameStateMessage *>(incomingData);
        game_state.master_turn = msg->payload.master_turn != 0;
        game_state.master_ships_left = msg->payload.master_ships_left;
        game_state.slave_ships_left = msg->payload.slave_ships_left;
        shot_pending = false;
        if (msg->payload.shot_result == NO_SHOT)
        {
            Serial.println("Start: master turn");
        }
        if (msg->payload.shot_result != NO_SHOT && msg->payload.shot_x < BOARD_SIZE && msg->payload.shot_y < BOARD_SIZE)
        {
            Cell result = static_cast<Cell>(msg->payload.shot_result);
            if (msg->payload.shooter == static_cast<uint8_t>(User::Master))
            {
                game_state.slave_board[msg->payload.shot_y][msg->payload.shot_x] = result;
                // Master shot slave — show slave's board so slave sees where they were hit
                showFrame(game_state.slave_board, true, false);
                delay(1000);
            }
            else
            {
                game_state.master_board[msg->payload.shot_y][msg->payload.shot_x] = result;
                // Slave shot master — show master's board so slave sees their shot result
                showFrame(game_state.master_board, false, false);
                delay(1000);
            }
        }
        start_received = true;
        break;
    }
    case MSG_SHIP_REVEAL:
    {
        if (len < static_cast<uint8_t>(sizeof(ShipRevealMessage)))
            return;
        const ShipRevealMessage *msg = reinterpret_cast<const ShipRevealMessage *>(incomingData);
        for (uint8_t i = 0; i < msg->count && i < MAX_SHIPS_IN_REVEAL; ++i)
        {
            const ShipRevealEntry &s = msg->ships[i];
            for (uint8_t j = 0; j < s.length; ++j)
            {
                uint8_t sx = s.x + (s.horizontal ? j : 0);
                uint8_t sy = s.y + (s.horizontal ? 0 : j);
                if (sx < BOARD_SIZE && sy < BOARD_SIZE && game_state.master_board[sy][sx] == CELL_EMPTY)
                    game_state.master_board[sy][sx] = CELL_SHIP;
            }
        }
        break;
    }
    default:
        break;
    }
}

bool sendPlaceShip(uint8_t ship_index, uint8_t x, uint8_t y, bool horizontal)
{
    PlaceShipMessage msg = {};
    msg.header.type = MSG_PLACE_SHIP;
    msg.payload.ship_index = ship_index;
    msg.payload.x = x;
    msg.payload.y = y;
    msg.payload.horizontal = horizontal ? 1 : 0;
    return sendRaw(reinterpret_cast<const uint8_t *>(&msg), sizeof(msg));
}

bool sendPlacementDone()
{
    PlacementDoneMessage msg = {};
    msg.header.type = MSG_PLACEMENT_DONE;
    return sendRaw(reinterpret_cast<const uint8_t *>(&msg), sizeof(msg));
}