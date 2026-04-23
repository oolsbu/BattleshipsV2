#include "master.h"
#include "game/logic.h"
#include "display/led_matrix.h"
#include "comms/protocol.h"

MasterGameState game_state;


void masterSetup() {
    for (uint8_t y = 0; y < BOARD_SIZE; ++y) {
        for (uint8_t x = 0; x < BOARD_SIZE; ++x) {
            game_state.master_board[y][x] = CELL_EMPTY;
            game_state.slave_board[y][x] = CELL_EMPTY;
        }
    }
}

void masterLoop(GamePhase& phase, int dx, int dy, int joyBtn, int btn) {
    game_state.master_cursor_x = (game_state.master_cursor_x + dx) % BOARD_SIZE;
    game_state.master_cursor_y = (game_state.master_cursor_y + dy) % BOARD_SIZE;
    switch (phase) {
        case PHASE_INIT:
            masterSetup();
            phase = PHASE_PLACING;
            break;
        case PHASE_PLACING:
            placeBoats(
                game_state.master_board,
                game_state.master_ships,
                game_state.master_ships_left,
                game_state.master_cursor_x,
                game_state.master_cursor_y,
                joyBtn,
                btn,
                game_state.placing_horizontal
            );
            showFrame(game_state.master_board, true, SHOW_OPPONENT_AIM, game_state.master_cursor_x, game_state.master_cursor_y);
            if (game_state.master_ships_left == 0) {
                phase = PHASE_SHOOTING;
            }
            break;
        case PHASE_SHOOTING:
        if(game_state.master_ships_left != 0 || game_state.slave_ships_left != 0) {
            // implement waiting animation or something here
            return;
        }
            break;



}
}


void masterRecievedMessage(uint8_t *incomingData, uint8_t len) {
    // Handle incoming messages from the slave
    // This function should update game_state based on the message type and content
}
Cell shoot(User user, uint8_t x, uint8_t y) {
    auto& board = (user == User::Master)
        ? game_state.slave_board
        : game_state.master_board;

    Cell& cell = board[y][x];

    return (cell == CELL_SHIP) ? (cell = CELL_HIT, CELL_HIT)
         : ((cell == CELL_HIT || cell == CELL_MISS) ? CELL_ALREADY_TARGETED
         : (cell = CELL_MISS, CELL_MISS));
}



