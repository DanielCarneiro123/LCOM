#include "model.h"

extern uint8_t scancode;
extern uint8_t byte_index;
uint8_t running = 1;
MenuState menuState = START;
extern MouseInfo mouse_info;
extern vbe_mode_info_t mode_info;
extern RTCInfo time_info;
extern bool firstFrame;
extern uint8_t sp_data;
extern bool new_data;
extern bool is_writing;
extern uint8_t player_one_won;
extern uint8_t player_two_cheating;

Sprite *mouse;
Sprite *hand;
Sprite *button1;
Sprite *button2;
Sprite *button3;
Sprite *button4;
Sprite *masterminix;
Sprite *start;
Sprite *exit_menu;
Sprite *board;
Sprite *board2;
Sprite *ball;
Sprite *small_ball;
Sprite *um;
Sprite *dois;
Sprite *tres;
Sprite *quatro;
Sprite *cinco;
Sprite *seis;
Sprite *sete;
Sprite *oito;
Sprite *w;
Sprite *r;
Sprite *toggle9;
Sprite *code_guessed;
Sprite *code_not_guessed;
Sprite *madeira;
Sprite *frase_menu;
Sprite *press_s;
Sprite *player_cheated;


Position* ball_positions;
PositionSmall* small_ball_positions;
Position* ball_box_positions;
PositionSmall* small_ball_box_positions;
Position* code_positions;


uint8_t balls;
bool activeTurn;
int colorArr[8] = {LIGHTBLUE, GREEN, YELLOW, DARKBLUE, RED, PINK, ORANGE, PURPLE};
uint32_t color_table[COLOR_AMOUNT] = {0, 1, 2, LIGHTBLUE, GREEN, YELLOW, DARKBLUE, RED, PINK, ORANGE, PURPLE};
uint32_t code_colors[4];
int8_t curr_turn = -1;
uint8_t player_no = 0;
uint8_t hide_code = 0;
int timer_interrupts = 0;

/**
 * @brief Updates the menu state \n 
 * This function is used instead of simply assigning the new state to avoid repeating code that needs to go along with updating said state
 * @param new_state State to transition to
 */
void update_menu_state(MenuState new_state) {
    firstFrame = true;
    if (new_state == GAME && menuState == END) resetTable();
    menuState = new_state;
}

/**
 * @brief Set up the box balls positions object \n 
 * Allocates memory for the balls that the player can pick up
 * @return 1 on failure, 0 otherwise
 */
int setup_box_balls_positions() {
    ball_box_positions = 0;
    ball_box_positions = malloc(sizeof(Position) * 8 );
    if (ball_box_positions == NULL) return 1;
    for (int j = 0; j < 8; j++) {
        ball_box_positions[j].x = 500;
        ball_box_positions[j].y = 77 + (j % 8) * 65;
        ball_box_positions[j].color = TRANSPARENT;
    }  
    return 0;
}

/**
 * @brief Set up the box small balls positions object \n 
 * Allocates memory for the small balls that the player can pick up
 * @return 1 on failure, 0 otherwise
 */
int setup_box_small_balls_positions() {
    small_ball_box_positions = 0;
    small_ball_box_positions = malloc(sizeof(PositionSmall) * 2 );
    if (small_ball_box_positions == NULL) return 1;
    for (int j = 0; j < 2; j++) {
        small_ball_box_positions[j].x = 500 + (j % 2) * 36;
        small_ball_box_positions[j].y = 20;
        small_ball_box_positions[j].color = 0;
    }  
    return 0;
}

/**
 * @brief Set up the positions object \n 
 * Allocates memory for the balls that the player will place
 * @return 1 on failure, 0 otherwise
 */
int setup_positions() {
    balls = 0;
    ball_positions = malloc(sizeof(Position) * 9 * 4);
    if (ball_positions == NULL) return 1;
    for (int i = 0; i < BALL_AMOUNT; i++) {
        ball_positions[i].x = 190 + (i % 4) * 56;
        ball_positions[i].y = 66 + (i / 4) * 60;
        ball_positions[i].color = TRANSPARENT;
    }
    return 0;
}

/**
 * @brief Set up the code positions object \n 
 * Allocates memory for the balls in the code
 * @return 1 on failure, 0 otherwise
 */
int setup_code_positions() {
    if (player_no == 1) {
        code_positions = NULL;
    }
    else {
        code_positions = malloc(sizeof(Position) * 4);
        if (code_positions == NULL) return 1;
        for (int i = 0; i < ROW_SIZE; i++) {
            code_positions[i].x = 190 + i * 56;
            code_positions[i].y = 6;
            code_positions[i].color = TRANSPARENT;
        }
    }
    return 0;
}

/**
 * @brief Set up the small positions object \n 
 * Allocates memory for the small balls that the player will place
 * @return 1 on failure, 0 otherwise
 */
int setup_small_positions() {
    small_ball_positions = malloc(sizeof(PositionSmall) * 36);
    if (small_ball_positions == NULL) return 1;
    for (int j = 0; j < ROW_NO; j++) {
        for (int i = 0; i < ROW_SIZE; i++) {
            small_ball_positions[j * 4 + i].x = 416 + (i % 2) * 26;
            small_ball_positions[j * 4 + i].y = (j * 61) + (60 + (i / 2) * 30);
            if (j > 5) small_ball_positions[j * 4 + i].y -= 2;
            small_ball_positions[j * 4 + i].color = 0;
        }
    }
    return 0;
}

/**
 * @brief Deallocates memory for placed balls
 */
void destroy_positions() {
    free(ball_positions);
}

/**
 * @brief Deallocates memory for placed small balls
 */
void destroy_small_positions() {
    free(small_ball_positions);
}

/**
 * @brief Deallocates memory for code balls
 */
void destroy_code_positions() {
    if (player_no == 2) free(code_positions);
}

/**
 * @brief Creates sprite objects \n 
 * Allocates memory for every sprite that the game will need
 * @return 1 on failure, 0 otherwise
 */
int setup_sprites() {
    mouse = create_sprite((xpm_map_t) mouse_xpm);
    if (mouse == NULL) return 1;
    board = create_sprite((xpm_map_t) mastermind_boardV1_xpm);
    if (board == NULL) return 1;
    board2 = create_sprite((xpm_map_t) mastermind_boardV2_xpm);
    if (board2 == NULL) return 1;
    ball = create_sprite((xpm_map_t) ball_xpm);
    if (ball == NULL) return 1;
    small_ball = create_sprite((xpm_map_t) small_ball_xpm);
    if (small_ball == NULL) return 1;
    hand = create_sprite((xpm_map_t) hand_xpm);
    if (hand == NULL) return 1;
    masterminix = create_sprite((xpm_map_t) masterminix_xpm);
    if (masterminix == NULL) return 1;
    start = create_sprite((xpm_map_t) start_xpm);
    if (start == NULL) return 1;
    exit_menu = create_sprite((xpm_map_t) exit_xpm);
    if (exit_menu == NULL) return 1;
    um = create_sprite((xpm_map_t) um_xpm);
    if (um == NULL) return 1;
    dois = create_sprite((xpm_map_t) dois_xpm);
    if (dois == NULL) return 1;
    tres = create_sprite((xpm_map_t) tres_xpm);
    if (tres == NULL) return 1;
    quatro = create_sprite((xpm_map_t) quatro_xpm);
    if (quatro == NULL) return 1;
    cinco = create_sprite((xpm_map_t) cinco_xpm);
    if (cinco == NULL) return 1;
    seis = create_sprite((xpm_map_t) seis_xpm);
    if (seis == NULL) return 1;
    sete = create_sprite((xpm_map_t) sete_xpm);
    if (sete == NULL) return 1;
    oito = create_sprite((xpm_map_t) oito_xpm);
    if (oito == NULL) return 1;
    w = create_sprite((xpm_map_t) w_xpm);
    if (w == NULL) return 1;
    r = create_sprite((xpm_map_t) r_xpm);
    if (r == NULL) return 1;
    toggle9 = create_sprite((xpm_map_t) toggle9_xpm);
    if (toggle9 == NULL) return 1;
    code_guessed = create_sprite((xpm_map_t) code_guessed_xpm);
    if (code_guessed == NULL) return 1;
    code_not_guessed = create_sprite((xpm_map_t) code_not_guessed_xpm);
    if (code_not_guessed == NULL) return 1;
    madeira = create_sprite((xpm_map_t) madeira_xpm);
    if (madeira == NULL) return 1;
    frase_menu = create_sprite((xpm_map_t) frase_menu_xpm);
    if (frase_menu == NULL) return 1;
    press_s = create_sprite((xpm_map_t) press_s_xpm);
    if (press_s == NULL) return 1;
    player_cheated = create_sprite((xpm_map_t) player_cheated_xpm);
    if (player_cheated == NULL) return 1;

    return 0;
}

/**
 * @brief Deallocates memory used for sprites
 */
void destroy_sprites() {
    destroy_sprite(mouse);
    destroy_sprite(board);
    destroy_sprite(board2);
    destroy_sprite(ball);
    destroy_sprite(small_ball);
    destroy_sprite(hand);
    destroy_sprite(masterminix);
    destroy_sprite(start);
    destroy_sprite(exit_menu);
    destroy_sprite(um);
    destroy_sprite(dois);
    destroy_sprite(tres);
    destroy_sprite(quatro);
    destroy_sprite(cinco);
    destroy_sprite(seis);
    destroy_sprite(sete);
    destroy_sprite(oito);
    destroy_sprite(w);
    destroy_sprite(r);
    destroy_sprite(toggle9);
    destroy_sprite(code_guessed);
    destroy_sprite(code_not_guessed);
    destroy_sprite(madeira);
    destroy_sprite(frase_menu);
    destroy_sprite(press_s);
    destroy_sprite(player_cheated);
}

/**
 * @brief Updates the state of the i8254 timer \n 
 * Beyond incrementing the internal counter, this function also sets the pace of queue updates, RTC updates, and buffer copying
 */
void update_timer_state() {
    copy_buffer();
    update_queue();
    update_rtc_state();
    timer_interrupts++;
}

/**
 * @brief Updates the state of the UART serial port \n 
 * This function runs the serial port's interrupt handler, then deals with any data that might have been recieved
 */
void update_sp_state() {
    sp_ih();
    if (new_data) {
        if ((sp_data & BIT(7)) && (sp_data & BIT(6))) {
            code_colors[3] = color_table[sp_data & 0xF];
        }
        else if (!(sp_data & BIT(7)) && (sp_data & BIT(6))) {
            code_colors[1] = color_table[sp_data & 0xF];
        }
        else if ((sp_data & BIT(7)) && !(sp_data & BIT(6))) {
            code_colors[2] = color_table[sp_data & 0xF];
        }
        else {
            code_colors[0] = color_table[sp_data & 0xF];
        }
        if (sp_data == SP_END_TURN) {
            activeTurn = true;
            curr_turn++;
            if (player_no == 2) {
                check_guess();
                if (player_one_won == 1 || curr_turn == 8){
                    push_code();
                    update_menu_state(END);
                }
            }
        }
        else if (sp_data == SP_NACK) {
            retry();
        }
        else if (sp_data == SP_ACK) {
            is_writing = false;
        }
        else if (menuState == END && player_no == 1) {

        }
        else if (sp_data == SP_TEST_PLAYER) {}
        else if (sp_data == SP_RESET_GAME) {
            resetTable();
            printf("\n\nI AM HERE\n\n");
            update_menu_state(GAME);
        }
        else if (sp_data & BIT(4)) {
            if (sp_data & BIT(0)) player_one_won = 1;
            if (sp_data & BIT(1)) player_two_cheating = 1;
            update_menu_state(END);
        }
        
        else if (player_no == 1) {
            uint8_t index = (curr_turn) * ROW_SIZE + (sp_data >> 6);
            if (sp_data & BIT(5)) {
                small_ball_positions[index].color = 0;
                clean_ball(index, small_ball, NULL);
            }
            else {
                small_ball_positions[index].color = color_table[sp_data & 0x1F];
            }
        }
        else if (player_no == 2) {
            uint8_t index = (curr_turn+1) * ROW_SIZE + (sp_data >> 6);
            if (sp_data & BIT(5)) {
                ball_positions[index].color = TRANSPARENT;
                clean_ball(index, ball, ball_positions);
            }
            else {
                ball_positions[index].color = color_table[sp_data & 0x1f];
            }
        }
    
    }

    new_data = false;
}

/**
 * @brief Function used when placing a ball \n 
 * This function contains all the logic tied to placing balls (differentiating players 1 from 2, etc) so that it can be called with no need for external conditions
 */
void place_move() {
    switch (player_no) {
        case 1:
            place_ball(ball_positions, BALL_AMOUNT);
            break;
        case 2:
            if (hide_code) return;
            if (curr_turn == -1) place_ball(code_positions, ROW_SIZE);
            else place_small_ball(small_ball_positions, BALL_AMOUNT);
            break;    
    } 
}

/**
 * @brief Function used when removing a ball \n 
 * This function contains all the logic tied to removing balls (differentiating players 1 from 2, etc) so that it can be called with no need for external conditions
 */
void remove_move() {
    switch (player_no) {
        case 1:
            remove_ball(ball_positions, BALL_AMOUNT);
            break;
        case 2:
            if (curr_turn == -1) remove_ball(code_positions, ROW_SIZE);
            else remove_small_ball();
            break;    
    } 
}



/**
 * @brief Updates the RTC time info
 */
void update_rtc_state() {
    if (timer_interrupts % GAME_FREQUENCY == 0) rtc_update_time();
}

/**
 * @brief Decides player numbers \n 
 * This function runs when the actual game begins. The first player to enter the game menu is player 1, the other is player 2.
 */
void test_player_no() {
    if (player_no == 1 || player_no == 2) return;
    if (sp_data == SP_TEST_PLAYER) {
        player_no = 2;
        activeTurn = true;
    }
    else {
        player_no = 1;
        activeTurn = false;
        push(SP_TEST_PLAYER);
    }
}

/**
 * @brief Updates the state of the keyboard \n 
 * This function runs the interrupt handler, and then runs code based on which key was pressed
 */
void update_keyboard_state() {
    (kbc_ih)();
    switch (scancode) {
        case Q_KEY:
            running = 0;
            break;
        case G_KEY:
            if (menuState == GAME) break;
            if (player_no == 2 && menuState == END) break;
            test_player_no();
            setup_code_positions();
            update_menu_state(GAME);
            break;
        case ONE_KEY:
            update_mouse_color(color_table[3]);
            break;
        case TWO_KEY:
            update_mouse_color(color_table[4]);
            break;
        case THREE_KEY:
            update_mouse_color(color_table[5]);
            break;
        case FOUR_KEY:
            update_mouse_color(color_table[6]);
            break;
        case FIVE_KEY:
            update_mouse_color(color_table[7]);
            break;   
        case SIX_KEY:
            update_mouse_color(color_table[8]);
            break;
        case SEVEN_KEY:
            update_mouse_color(color_table[9]);
            break;
        case EIGHT_KEY:
            update_mouse_color(color_table[10]);
            break;     
        case W_KEY:
            update_mouse_color(2);
            break;
        case R_KEY:
            update_mouse_color(1);
            break;
        case P_KEY:
            place_move();
            break;
        case O_KEY:
            remove_move(); 
            break;
        case T_KEY:
            if (player_no == 2){
                if(hide_code){
                    hide_code = 0;
                    clean_lid();
                }
                else{
                    hide_code = 1;
                }
            }
            break;
        case ENTER_KEY:
            if (player_no == 2 && curr_turn == 0) finish_turn(code_positions);
            else finish_turn(ball_positions);
            break;
        default:
            break;
    }
    draw_new_frame();
}

/**
 * @brief Updates the state of the mouse \n 
 * This function runs the interrupt handler, updates the mouse info at the end of each packet, and runs code according to the button the player is pressing
 */
void update_mouse_state() {
    (mouse_ih)();
    mouse_update_buffer();
    if (byte_index == 3) {
        mouse_sync_info();
        byte_index = 0;
        if (mouse_info.left_click){
            menu_selection();
            end_menu_selection();
            pick_box_ball();
            place_move(); 
            pick_small_ball();
            click_hide_code_button();
        }
        if (mouse_info.right_click){
            remove_move();  
        }
        draw_new_frame();        
    }
}


void click_hide_code_button() {
    if (is_mouse_in_sprite(toggle9) && player_no == 2) {
        if(hide_code){
            hide_code = 0;
            clean_lid();
        }
        else{
            hide_code = 1;
        }
    }
}

/**
 * @brief Updates the color of the ball being held \n 
 * This function is used instead of simply assigning the new color to avoid repeating the condition that this can only happen in the game menu
 * @param color new color to be used
 */
void update_mouse_color(uint32_t color) {
  if (menuState == GAME) {
        mouse_info.ball_color = color;
    }
}

/**
 * @brief Tries to mark a turn as finished \n 
 * This function checks if all balls for the current turn have been filled (irrelevant for small balls)
 * @param positions Array of balls to check (use ball_positions in the case of small balls)
 */
void finish_turn(Position* positions) {
    if (player_no == 1) {
        for (int i = curr_turn * ROW_SIZE; i < (curr_turn + 1) * ROW_SIZE; i++) {
            if (positions[i].color == TRANSPARENT) {
                return;
            }
        }
    }
    if (menuState == GAME && activeTurn) {
        activeTurn = false;
        push(SP_END_TURN);
        if (player_no == 2 && curr_turn != -1) {
            check_player_two_cheating();
        }
    }
}

/**
 * @brief Checks if the mouse is inside a given ball position
 * @param i Index to check
 * @param positions Position array to check in
 * @return true The mouse is inside the position
 * @return false The mouse is not inside the position
 */
bool is_mouse_in_ball(uint8_t i, Position* positions) {
    return mouse_info.x >= positions[i].x && mouse_info.x <= positions[i].x + ball->width && mouse_info.y >= positions[i].y && mouse_info.y <= positions[i].y + ball->height;
}

/**
 * @brief Checks if the mouse is inside a given small ball position
 * @param i Index to check
 * @param small_ball_positions Small position array to check in
 * @return true The mouse is inside the position
 * @return false The mouse is not inside the position
 */
bool is_mouse_in_small_ball(uint8_t i, PositionSmall* small_ball_positions) {
    return mouse_info.x >= small_ball_positions[i].x && mouse_info.x <= small_ball_positions[i].x + small_ball->width && mouse_info.y >= small_ball_positions[i].y && mouse_info.y <= small_ball_positions[i].y + small_ball->height;
}

bool is_mouse_in_ball_box(uint8_t i) {
    return mouse_info.x >= ball_box_positions[i].x && mouse_info.x <= ball_box_positions[i].x + ball->width && mouse_info.y >= ball_box_positions[i].y && mouse_info.y <= ball_box_positions[i].y + ball->height;
}

/**
 * @brief Places a ball \n 
 * This function returns before doing anything if the player's turn isn't active or if the player is not in the game menu
 * @param positions Position array to place on
 * @param Number of balls in array
 */
void place_ball(Position* positions, uint8_t n) {
    if (menuState != GAME) return;
    if (!activeTurn) return;
    if (mouse_info.ball_color < 3) return;
    int8_t turn_offset = curr_turn == -1 ? 0 : curr_turn;
    for (int i = turn_offset * ROW_SIZE; i < (turn_offset + 1) * ROW_SIZE; i++) {
        if (i >= n) return;
        if (is_mouse_in_ball(i, positions)) {
            positions[i].color = mouse_info.ball_color;
            uint8_t byte = prepare_move_byte(i%4, mouse_info.ball_color, 0);
            if (positions != code_positions) push(byte);
            return;
        }
    }
}

/**
 * @brief Selects menu options \n 
 * This function is run when on the initial menu and the player presses the left mouse button
 */
void menu_selection() {
    if (menuState != START) return;
    if (is_mouse_in_sprite(start)) {
        test_player_no();
        setup_code_positions();
        update_menu_state(GAME);
        return; 
    }

    if (is_mouse_in_sprite(exit_menu)){
        running = 0;
    }      
    
}

/**
 * @brief Selects menu options for final menu
 */
void end_menu_selection() {
    if (menuState != END) return;
    if (player_no == 1 && is_mouse_in_sprite(start)) {
        test_player_no();
        setup_code_positions();
        update_menu_state(GAME);
        return; 
    }

    if (is_mouse_in_sprite(exit_menu)){
        running = 0;
    }      
    
}

/**
 * @brief Picks a small ball from the box \n 
 * Picks a black/white small ball from the box
 */
void pick_small_ball() {
    if (menuState != GAME) return;
    if (!activeTurn) return;
    for (int i = 0; i < 2; i++) {
        if (is_mouse_in_small_ball(i, small_ball_box_positions)) {
            update_mouse_color(i+1);
            return;       
        }
    }
}

/**
 * @brief Picks a ball from the box \n 
 * Picks a large ball from the box
 */
void pick_box_ball() {
    if (menuState != GAME) return;
    if (!activeTurn) return;
    for (int i = 0; i < 8; i++) {
        if (is_mouse_in_ball(i, ball_box_positions)) {
            update_mouse_color(colorArr[i]);
            return;       
        }
    }


}

/**
 * @brief Places a small ball \n 
 * This function returns before doing anything if the player's turn isn't active or if the player is not in the game menu
 * @param positions Small position array to place on
 * @param n Number of balls in array
 */
void place_small_ball(PositionSmall* small_ball_positions, uint8_t n) {
    if (menuState != GAME) return;
    if (!activeTurn) return;
    if (!(mouse_info.ball_color == 1 || mouse_info.ball_color == 2)) return;
    int8_t turn_offset = curr_turn == -1 ? 0 : curr_turn;
    for (int i = turn_offset * ROW_SIZE; i < (turn_offset + 1) * ROW_SIZE; i++) {
        if (i >= n) return;
        if (is_mouse_in_small_ball(i, small_ball_positions)) {
            small_ball_positions[i].color = mouse_info.ball_color;
            uint8_t byte = prepare_move_byte(i%4, mouse_info.ball_color, 0);
            push(byte);
            return;
        }
    }
}

/**
 * @brief Removes a ball from the board if the mouse is inside it \n 
 * Checks every ball in the array until finding one that can be removed
 * @param positions Array to check in
 * @param n Number of balls in array
 */
void remove_ball(Position* positions, uint8_t n) {
    if (menuState != GAME) return;
    if (!activeTurn) return;
    int8_t turn_offset = curr_turn == -1 ? 0 : curr_turn;
    for (int i = turn_offset * ROW_SIZE; i < (turn_offset + 1) * ROW_SIZE; i++) {
        if (i >= n) return;
        if (is_mouse_in_ball(i, positions)) {
            positions[i].color = TRANSPARENT;
            uint8_t byte = prepare_move_byte(i%4, mouse_info.ball_color, 1);
            if (positions != code_positions) push(byte);
            clean_ball(i, ball, positions);
            break;
        }
    }
}

/**
 * @brief Removes a small ball from the board if the mouse is inside it \n 
 * Checks every ball in the array until finding one that can be removed \n 
 * This function doesn't need any arguments because, unlike for large balls, there is only one case in which small balls can be removed
 */
void remove_small_ball() {
    if (menuState != GAME) return;
    if (!activeTurn) return;
    for (int i = 0; i < BALL_AMOUNT; i++) {
        if (is_mouse_in_small_ball(i, small_ball_positions)) {
            small_ball_positions[i].color = 0;
            uint8_t byte = prepare_move_byte(i%4, mouse_info.ball_color, 1);
            push(byte);
            clean_ball(i, small_ball, NULL);
            break;
        }
    }
}

/**
 * @brief Resets the game state \n 
 * Resets global variables so that the game may be replayed
 */
void resetTable(){
    curr_turn = -1;
    hide_code = 0;
    player_one_won = 0;
    player_two_cheating = 0;
    if (player_no == 1){
        activeTurn = false;
    }
    else if (player_no == 2){
        activeTurn = true;
    }
    for (int i = 0; i < BALL_AMOUNT; i++){
        ball_positions[i].color = TRANSPARENT;
    }
    for (int i = 0; i < BALL_AMOUNT; i++){
        small_ball_positions[i].color = 0;
    }
    if (player_no == 2) {
        for (int i = 0; i < ROW_SIZE; i++){
            code_positions[i].color = TRANSPARENT;
        }
    }    
    if (sp_data != SP_RESET_GAME) push(SP_RESET_GAME);
}

/**
 * @brief Pushes endgame data \n 
 * Pushes all relevant data to player 1 when the game ends
 */
void push_code() {
    uint8_t end_byte = BIT(4);
    if (player_one_won == 1) end_byte |= BIT(0);
    if (player_two_cheating == 1) end_byte |= BIT(1);
    push(end_byte);
    for (int i = 0; i < ROW_SIZE; i++) {
        uint8_t byte = prepare_move_byte(i, code_positions[i].color, 0);
        push(byte);
    }
}
