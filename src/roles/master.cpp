#include "master.h"
#include "game/logic.h"
#include "display/led_matrix.h"
#include "comms/protocol.h"
#include "comms/connect.h"

static MasterGameState game_state;
static bool slave_ready = false;
static bool start_sent = false;
static bool has_last_aim = false;
static bool animation_done = false;
static uint8_t last_aim_x = 0;
static uint8_t last_aim_y = 0;

static Cell shoot(User user, uint8_t y, uint8_t x);

static bool sendAim(uint8_t x, uint8_t y)
{
    AimMessage msg = {};
    msg.header.type = MSG_AIM;
    msg.payload.x = x;
    msg.payload.y = y;
    return sendRaw(reinterpret_cast<const uint8_t *>(&msg), sizeof(msg));
}

static void sendShipReveal()
{
    ShipRevealMessage msg = {};
    msg.header.type = MSG_SHIP_REVEAL;
    msg.count = SHIPS;
    for (uint8_t i = 0; i < SHIPS; ++i)
    {
        const Ship &s = game_state.master_ships[i];
        msg.ships[i].x = s.x;
        msg.ships[i].y = s.y;
        msg.ships[i].length = s.length;
        msg.ships[i].horizontal = s.horizontal ? 1 : 0;
    }
    sendRaw(reinterpret_cast<const uint8_t *>(&msg), sizeof(msg));
}

static void sendGameState(uint8_t shooter, uint8_t x, uint8_t y, uint8_t result)
{
    GameStateMessage msg = {};
    msg.header.type = MSG_GAME_STATE;
    msg.payload.master_turn = game_state.master_turn ? 1 : 0;
    msg.payload.master_ships_left = game_state.master_ships_left;
    msg.payload.slave_ships_left = game_state.slave_ships_left;
    msg.payload.shooter = shooter;
    msg.payload.shot_x = x;
    msg.payload.shot_y = y;
    msg.payload.shot_result = result;
    sendRaw(reinterpret_cast<const uint8_t *>(&msg), sizeof(msg));
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

void masterSetup()
{
    slave_ready = false;
    start_sent = false;
    has_last_aim = false;
    animation_done = false;
    for (uint8_t i = 0; i < SHIPS; ++i)
    {
        game_state.master_ships[i] = {};
        game_state.slave_ships[i] = {};
    }

    for (uint8_t y = 0; y < BOARD_SIZE; ++y)
    {
        for (uint8_t x = 0; x < BOARD_SIZE; ++x)
        {
            game_state.master_board[y][x] = CELL_EMPTY;
            game_state.slave_board[y][x] = CELL_EMPTY;
        }
    }

    game_state.master_ships_left = SHIPS;
    game_state.slave_ships_left = SHIPS;
    game_state.master_ships_to_place = SHIPS;
    game_state.slave_ships_to_place = SHIPS;
    game_state.master_cursor_x = 0;
    game_state.master_cursor_y = 0;
    game_state.slave_cursor_x = 0;
    game_state.slave_cursor_y = 0;
    game_state.placing_horizontal = true;
    game_state.master_turn = true;
}

void masterLoop(GamePhase &phase, int dx, int dy, int joyBtn, int btn)
{
    switch (phase)
    {
    case PHASE_INIT:
        masterSetup();
        phase = PHASE_PLACING;
        break;
    case PHASE_PLACING:
    {
        uint8_t previewLength = (game_state.master_ships_to_place > 0)
                                    ? shipLengthForIndex(SHIPS - game_state.master_ships_to_place)
                                    : 1;
        int maxX = BOARD_SIZE - (game_state.placing_horizontal ? previewLength : 1);
        int maxY = BOARD_SIZE - (game_state.placing_horizontal ? 1 : previewLength);

        game_state.master_cursor_x = clampIndex(static_cast<int>(game_state.master_cursor_x) + dx, 0, maxX);
        game_state.master_cursor_y = clampIndex(static_cast<int>(game_state.master_cursor_y) + dy, 0, maxY);

        placeBoats(
            game_state.master_board,
            game_state.master_ships,
            game_state.master_ships_to_place,
            game_state.master_cursor_x,
            game_state.master_cursor_y,
            joyBtn,
            btn,
            game_state.placing_horizontal);
        {
            previewLength = (game_state.master_ships_to_place > 0)
                                ? shipLengthForIndex(SHIPS - game_state.master_ships_to_place)
                                : 0;
            maxX = BOARD_SIZE - (game_state.placing_horizontal ? previewLength : 1);
            maxY = BOARD_SIZE - (game_state.placing_horizontal ? 1 : previewLength);
            game_state.master_cursor_x = clampIndex(game_state.master_cursor_x, 0, maxX);
            game_state.master_cursor_y = clampIndex(game_state.master_cursor_y, 0, maxY);
            bool previewValid = canPlaceShip(game_state.master_board, previewLength, game_state.master_cursor_x, game_state.master_cursor_y, game_state.placing_horizontal);
            showFrame(game_state.master_board, true, false, game_state.master_cursor_x, game_state.master_cursor_y,
                      previewLength, game_state.placing_horizontal, previewValid);
        }
        if (game_state.master_ships_to_place == 0)
        {
            if (slave_ready)
            {
                if (!start_sent)
                {
                    game_state.master_turn = true;
                    start_sent = true;
                    sendGameState(static_cast<uint8_t>(User::Master), 0, 0, NO_SHOT);
                    Serial.println("Start: master turn");
                }
                phase = PHASE_SHOOTING;
            }
            else
            {
                phase = PHASE_WAITING;
            }
        }
    }
    break;
    case PHASE_WAITING:
        if (slave_ready)
        {
            if (!start_sent)
            {
                game_state.master_turn = true;
                start_sent = true;
                sendGameState(static_cast<uint8_t>(User::Master), 0, 0, NO_SHOT);
                Serial.println("Start: master turn");
            }
            phase = PHASE_SHOOTING;
        }
        showFrame(game_state.master_board, true, false);
        break;
    case PHASE_SHOOTING:
        if (game_state.master_ships_left == 0 || game_state.slave_ships_left == 0)
        {
            phase = PHASE_WON;
            int winner = (game_state.master_ships_left == 0) ? 1 : 0;
            break;
        }
        if (game_state.master_turn)
        {
            game_state.master_cursor_x = clampIndex(static_cast<int>(game_state.master_cursor_x) + dx, 0, BOARD_SIZE - 1);
            game_state.master_cursor_y = clampIndex(static_cast<int>(game_state.master_cursor_y) + dy, 0, BOARD_SIZE - 1);
            if (SHOW_OPPONENT_AIM && (!has_last_aim || last_aim_x != game_state.master_cursor_x || last_aim_y != game_state.master_cursor_y))
            {
                sendAim(game_state.master_cursor_x, game_state.master_cursor_y);
                last_aim_x = game_state.master_cursor_x;
                last_aim_y = game_state.master_cursor_y;
                has_last_aim = true;
            }
            if (game_state.master_ships_to_place != 0)
            {
                phase = PHASE_PLACING;
                return;
            }
            if (btn)
            {
                Cell result = shoot(User::Master, game_state.master_cursor_y, game_state.master_cursor_x);
                if (result != CELL_ALREADY_TARGETED)
                {
                    game_state.slave_board[game_state.master_cursor_y][game_state.master_cursor_x] = result;
                    showFrame(game_state.slave_board, false, false); // show result without cursor
                    delay(1000);
                    game_state.master_turn = false;
                    sendGameState(static_cast<uint8_t>(User::Master), game_state.master_cursor_x, game_state.master_cursor_y, static_cast<uint8_t>(result));
                }
                Serial.println(result);
            }
            showFrame(game_state.slave_board, false, true, game_state.master_cursor_x, game_state.master_cursor_y);
        }
        else
        {
            showFrame(game_state.master_board, true, SHOW_OPPONENT_AIM, game_state.slave_cursor_x, game_state.slave_cursor_y);
        }
        break;
    case PHASE_WON:
        if (!animation_done)
        {
            animation_done = true;
            sendShipReveal();
            showEndAnimation(game_state.slave_ships_left == 0, game_state.slave_board);
        }
        break;
    }
}

void masterRecievedMessage(uint8_t *incomingData, uint8_t len)
{
    if (len < static_cast<uint8_t>(sizeof(MessageHeader)))
    {
        return;
    }

    const MessageHeader *header = reinterpret_cast<const MessageHeader *>(incomingData);
    switch (header->type)
    {
    case MSG_PLACE_SHIP:
    {
        if (len < static_cast<uint8_t>(sizeof(PlaceShipMessage)))
            return;
        const PlaceShipMessage *msg = reinterpret_cast<const PlaceShipMessage *>(incomingData);
        if (msg->payload.ship_index >= SHIPS)
            return;
        Ship &ship = game_state.slave_ships[msg->payload.ship_index];
        ship.x = msg->payload.x;
        ship.y = msg->payload.y;
        ship.length = shipLengthForIndex(msg->payload.ship_index);
        ship.horizontal = msg->payload.horizontal != 0;
        ship.placed = true;
        ship.hits = 0;
        for (uint8_t i = 0; i < ship.length; ++i)
        {
            uint8_t sx = ship.x + (ship.horizontal ? i : 0);
            uint8_t sy = ship.y + (ship.horizontal ? 0 : i);
            if (sx < BOARD_SIZE && sy < BOARD_SIZE)
                game_state.slave_board[sy][sx] = CELL_SHIP;
        }
        break;
    }
    case MSG_PLACEMENT_DONE:
        slave_ready = true;
        if (game_state.master_ships_to_place == 0 && !start_sent)
        {
            game_state.master_turn = true;
            start_sent = true;
            sendGameState(static_cast<uint8_t>(User::Master), 0, 0, NO_SHOT);
            Serial.println("Start: master turn");
        }
        break;
    case MSG_AIM:
    {
        if (len < static_cast<uint8_t>(sizeof(AimMessage)))
            return;
        const AimMessage *msg = reinterpret_cast<const AimMessage *>(incomingData);
        if (msg->payload.x < BOARD_SIZE)
            game_state.slave_cursor_x = msg->payload.x;
        if (msg->payload.y < BOARD_SIZE)
            game_state.slave_cursor_y = msg->payload.y;
        break;
    }
    case MSG_SHOOT:
    {
        if (len < static_cast<uint8_t>(sizeof(ShootMessage)))
            return;
        if (game_state.master_turn)
            return;
        const ShootMessage *msg = reinterpret_cast<const ShootMessage *>(incomingData);
        if (msg->payload.x >= BOARD_SIZE || msg->payload.y >= BOARD_SIZE)
            return;
        Cell result = shoot(User::Slave, msg->payload.y, msg->payload.x);
        if (result != CELL_ALREADY_TARGETED)
        {
            game_state.master_turn = true;
            sendGameState(static_cast<uint8_t>(User::Slave), msg->payload.x, msg->payload.y, static_cast<uint8_t>(result));
        }
        break;
    }
    default:
        break;
    }
}

static Cell shoot(User user, uint8_t y, uint8_t x)
{
    auto &board = (user == User::Master)
                      ? game_state.slave_board
                      : game_state.master_board;

    Cell &cell = board[y][x];
    Cell hit = (cell == CELL_SHIP) ? (cell = CELL_HIT, CELL_HIT) : ((cell == CELL_HIT || cell == CELL_MISS || cell == CELL_SUNK) ? CELL_ALREADY_TARGETED : (cell = CELL_MISS, CELL_MISS));
    if (hit == CELL_HIT)
    {

        // check if ship is sunk
        for (Ship &ship : (user == User::Master) ? game_state.slave_ships : game_state.master_ships)
        {
            if (ship.placed && ship.x <= x && x < ship.x + (ship.horizontal ? ship.length : 1) && ship.y <= y && y < ship.y + (ship.horizontal ? 1 : ship.length))
            {
                ship.hits++;
                if (ship.isSunk())
                {
                    (user == User::Master) ? game_state.slave_ships_left-- : game_state.master_ships_left--;
                    // mark as sunk
                    for (uint8_t i = 0; i < ship.length; ++i)
                    {
                        uint8_t sx = ship.x + (ship.horizontal ? i : 0);
                        uint8_t sy = ship.y + (ship.horizontal ? 0 : i);
                        board[sy][sx] = CELL_SUNK;
                    }
                    hit = CELL_SUNK;
                }
                break;
            }
        }
    }
    return hit;
}
