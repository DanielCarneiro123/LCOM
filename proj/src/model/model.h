#ifndef _LCOM_MODEL_H_
#define _LCOM_MODEL_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "controller/keyboard/KBC.h"
#include "controller/mouse/mouse.h"
#include "controller/video/graphics.h"
#include "controller/rtc/rtc.h"
#include "controller/serialport/sp.h"
#include "logic/queue/queue.h"
#include "logic/board_logic.h"
#include "xpm/hand.xpm"
#include "xpm/mouse.xpm"
#include "xpm/cursorGimp.xpm"
#include "xpm/smile.xpm"
#include "xpm/masterminix.xpm"
#include "xpm/mastermind_boardV1.xpm"
#include "xpm/mastermind_boardV2.xpm"
#include "xpm/ball.xpm"
#include "xpm/small_ball.xpm"
#include "xpm/start.xpm"
#include "xpm/exit.xpm"
#include "xpm/um.xpm"
#include "xpm/dois.xpm"
#include "xpm/tres.xpm"
#include "xpm/quatro.xpm"
#include "xpm/cinco.xpm"
#include "xpm/seis.xpm"
#include "xpm/sete.xpm"
#include "xpm/oito.xpm"
#include "xpm/w.xpm"
#include "xpm/r.xpm"
#include "xpm/toggle9.xpm"
#include "xpm/code_guessed.xpm"
#include "xpm/code_not_guessed.xpm"
#include "xpm/madeira.xpm"


typedef struct {
    uint16_t x;
    uint16_t y;
    uint32_t color;
} Position;

#include "view/view.h"
#include "model/sprite.h"
#include "config.h"

typedef enum {
    RUNNING,
    EXIT,
} SystemState;

typedef enum {
    START,
    GAME,
    END
} MenuState;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t color;
} PositionSmall;




void update_timer_state();
void update_keyboard_state();
void update_mouse_state();
void update_rtc_state();
void update_sp_state();
void setup_sprites();
void setup_positions();
void setup_box_balls_positions();
void setup_box_small_balls_positions();
void setup_code_positions();
void setup_small_positions();
void destroy_positions();
void destroy_code_positions();
void destroy_small_positions();
void destroy_sprites();
void push_code();
bool is_mouse_in_ball(uint8_t i, Position* positions);
bool is_mouse_in_small_ball(uint8_t i, PositionSmall* small_ball_positions);
bool is_mouse_in_start();
bool is_mouse_in_hide_code_button();
void update_menu_state(MenuState new_state);
void update_mouse_color(uint32_t color);
void place_ball(Position* positions, uint8_t n);
void place_small_ball(PositionSmall* small_ball_positions, uint8_t n);
void remove_ball(Position* positions, uint8_t n);
void remove_small_ball();
void finish_turn(Position* positions);
void pick_box_ball();
bool is_mouse_in_ball_box(uint8_t i);
void test_player_no();
void place_move();
void remove_move();
void menu_selection();
bool is_mouse_in_small_ball_box(uint8_t i);
void pick_small_ball();
void click_hide_code_button();
void resetTable();


#endif
