#ifndef _LCOM_MENU_H_
#define _LCOM_MENU_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "model/game_constants.h"
#include "driver/video/graphics.h"
#include "driver/mouse/mouse.h"
#include "driver/rtc/rtc.h"
#include "model/sprite.h"
#include "model/data_structures.h"

void draw_new_frame();
void draw_initial_menu();
void draw_game_menu();
void draw_finish_menu();
void clean_mouse();
void draw_mouse();
void draw_numbers_and_balls_in_box();
void draw_balls();
void draw_lid();
void draw_code();
void clean_lid();
void draw_small_balls();
void copy_buffer();
int draw_sprite_xpm(Sprite *sprite, int x, int y);
int draw_partial_sprite_xpm(Sprite *sprite, int x, int y, int xdraw, int ydraw, int height, int width);
int set_frame_buffers(uint16_t mode);
int draw_ball(Sprite *sprite, int x, int y, uint32_t color);
void clean_ball(uint8_t k, Sprite* sprite, Position* positions);
void set_background_color();
void draw_final_code();

#endif
