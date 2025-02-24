#include "board_logic.h"
#include "model/data_structures.h"

extern uint8_t curr_turn;
extern Position* ball_positions;
extern PositionSmall* small_ball_positions;
extern Position* code_positions;
uint8_t player_one_won = 0;
uint8_t player_two_cheating = 0;


/**
 * @brief Checks if player one's guess is correct \n 
 * This function compares the colors in the code to the colors in the position array, according to the latest turn \n 
 * If the guess is correct, a flag is set
 */
void check_guess() {
    uint8_t guesses = 0;
    for (uint8_t i = 0; i < 4; i++) {
        if (ball_positions[curr_turn * 4 + i].color == code_positions[i].color) guesses++;
    }

    if (guesses == 4) {
        player_one_won = 1;
        printf("\n\nPLAYER ONE WON\n\n");
    }
    
    if (curr_turn == 8){
        printf("\n\nPLAYER ONE LOST\n\n");
    }
}

/**
 * @brief Checks if player two gave wrong instructions to player 1 \n 
 * This function compares the instructions that player 2 to the last turn that player 1 completed \n 
 * If player 2 made a mistake, a flag is set
 */
void check_player_two_cheating() {
    int8_t right_place = 0, wrong_place = 0;
    uint8_t used[4];
    uint8_t code_used[4];
    memset(used, 0, 4);

    for (uint8_t i = 0; i < 4; i++) {
        if (ball_positions[curr_turn * 4 + i].color == code_positions[i].color) {
            right_place++;
            used[i] = 1;
            code_used[i] = 1;
        }
    }

    for (uint8_t i = 0; i < 4; i++) {
        if (used[i]) continue;
        uint32_t curr_color = ball_positions[curr_turn * 4 + i].color;

        for (uint8_t j = 0; j < 4; j++) {
            if (code_used[j] == 1 || i == j) continue;
            if (curr_color == code_positions[j].color) {
                wrong_place++;
                code_used[j] = 1;
                break;
            }
        }
    }

    for (uint8_t i = 0; i < 4; i++) {
        if (small_ball_positions[curr_turn * 4 + i].color == 1) right_place--;
        if (small_ball_positions[curr_turn * 4 + i].color == 2) wrong_place--;
    }

    if ((right_place != 0) || (wrong_place != 0)) {
        player_two_cheating = 1;
    }
}
