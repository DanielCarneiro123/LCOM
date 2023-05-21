#ifndef _LCOM_MODEL_H_
#define _LCOM_MODEL_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "controller/keyboard/KBC.h"
#include "controller/mouse/mouse.h"
#include "controller/video/graphics.h"
#include "controller/rtc/rtc.h"
#include "controller/serialport/sp.h"
#include "controller/serialport/queue/queue.h"
#include "xpm/hand.xpm"
#include "xpm/mouse.xpm"
#include "xpm/cursorGimp.xpm"
#include "xpm/smile.xpm"
#include "xpm/masterminix.xpm"
#include "xpm/mastermind_boardV1.xpm"
#include "xpm/ball.xpm"
#include "xpm/small_ball.xpm"
#include "xpm/start.xpm"
#include "xpm/exit.xpm"
#include "xpm/um.xpm"
#include "xpm/dois.xpm"
#include "xpm/tres.xpm"
#include "xpm/quatro.xpm"
#include "xpm/cinco.xpm"

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
    uint32_t color;
} Position;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t color;
} PositionSmall;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint32_t color;
} PositionBallsBox;

void update_timer_state();
void update_keyboard_state();
void update_mouse_state();
void update_buttons_state();
void update_rtc_state();
void update_sp_state();
void setup_sprites();
void setup_positions();
void setup_box_balls_positions();
void setup_small_positions();
void destroy_positions();
void destroy_small_positions();
void destroy_sprites();
bool is_mouse_in_ball(uint8_t i);
bool is_mouse_in_small_ball(uint8_t i);
void update_menu_state(MenuState new_state);
void update_mouse_color(uint32_t color);
void place_ball();
void place_small_ball();
void remove_ball();
void remove_small_ball();
void finish_turn();
void pick_box_ball();
bool is_mouse_in_ball_box(uint8_t i);

#endif
