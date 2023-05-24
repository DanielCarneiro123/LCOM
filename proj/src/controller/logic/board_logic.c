#include "board_logic.h"

extern uint8_t curr_turn;
extern Position* ball_positions;
extern PositionSmall* small_ball_positions;
extern Position* code_positions;

bool check_guess() {
    uint8_t guesses = 0;
    for (uint8_t i = 0; i < 4; i++) {
        if (ball_positions[curr_turn * 4 + i].color == code_positions[i].color) guesses++;
    }

    printf("\n\nTHERE ARE %d CORRECT GUESSES\n\n", guesses);
    return guesses == 4;
}

bool check_player_two_cheating() {
    int8_t right_place = 0, wrong_place = 0;
    uint8_t used[4];
    memset(used, 0, 4);

    for (uint8_t i = 0; i < 4; i++) {
        if (ball_positions[curr_turn * 4 + i].color == code_positions[i].color) {
            right_place++;
            used[i] = 1;
        }
    }

    for (uint8_t i = 0; i < 4; i++) {
        if (used[i]) continue;
        uint32_t curr_color = ball_positions[curr_turn * 4 + i].color;

        for (uint8_t j = 0; j < 4; i++) {
            if (used[j] || i == j) continue;
            if (curr_color == code_positions[j].color) {
                wrong_place++;
                break;
            }
        }
    }

    for (uint8_t i = 0; i < 4; i++) {
        if (small_ball_positions[i].color == 1) right_place--;
        if (small_ball_positions[i].color == 2) wrong_place--;
    }

    return (right_place == 0) && (wrong_place == 0);
}
