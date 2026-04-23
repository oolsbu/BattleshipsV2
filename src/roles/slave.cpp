 #include "slave.h"
 #include "comms/protocol.h"
 #include "display/led_matrix.h"
 #include "game/logic.h"
 #include <espnow.h>

 extern uint8_t peerAddress[];
    GameState slave_game_state;

void slaveLoop(GamePhase& phase, int dx, int dy, int joyBtn, int btn) {
    slave_game_state.cursor_x = (slave_game_state.cursor_x + dx) % BOARD_SIZE;
    slave_game_state.cursor_y = (slave_game_state.cursor_y + dy) % BOARD_SIZE;
    switch (phase) {
        case PHASE_INIT:
            masterSetup();
            phase = PHASE_PLACING;
            break;
        case PHASE_PLACING:
            placeBoats(
                slave_game_state.my_board,
                slave_game_state.my_ships,
                slave_game_state.my_ships_left,
                slave_game_state.cursor_x,
                slave_game_state.cursor_y,
                joyBtn,
                btn,
                slave_game_state.placing_horizontal
            );
            showFrame(slave_game_state.my_board, true, SHOW_OPPONENT_AIM, slave_game_state.cursor_x, slave_game_state.cursor_y);
            if (slave_game_state.my_ships_left == 0) {
                phase = PHASE_SHOOTING;
            }
            break;
        case PHASE_SHOOTING:
        if(slave_game_state.my_ships_left != 0 || slave_game_state.master_ships_left != 0) {
            // implement waiting animation or something here
            return;
        }
            break;



}
}
 
 
 
 
 
 
 
 bool sendPlaceShip(uint8_t ship_index, uint8_t x, uint8_t y, bool horizontal) {
	 PlaceShipMessage msg = {
		 {MSG_PLACE_SHIP},
		 {ship_index, x, y, static_cast<uint8_t>(horizontal ? 1 : 0)}
	 };
	 return esp_now_send(peerAddress, reinterpret_cast<uint8_t*>(&msg), sizeof(msg)) == 0;
 }

 bool sendPlacementDone() {
	 PlacementDoneMessage msg = {
		 {MSG_PLACEMENT_DONE}
	 };
	 return esp_now_send(peerAddress, reinterpret_cast<uint8_t*>(&msg), sizeof(msg)) == 0;
 }
 
 void slaveRecievedMessage(uint8_t *incomingData, uint8_t len) {
	 if (len < sizeof(MessageHeader)) {
		 return;
	 }
 
	 const MessageHeader* header = reinterpret_cast<const MessageHeader*>(incomingData);
 
	 switch (static_cast<MessageType>(header->type)) {
		 case MSG_AIM:
		 case MSG_SHOOT:
		 case MSG_GAME_STATE:
		 case MSG_PLACE_SHIP:
		 case MSG_PLACEMENT_DONE:
		 default:
			 break;
	 }
 }
