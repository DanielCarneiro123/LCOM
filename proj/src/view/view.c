#include "view.h"

uint8_t *main_frame_buffer;
uint8_t *back_buffer;
uint32_t frame_buffer_size;
extern int timer_interrupts;
extern vbe_mode_info_t mode_info;
extern MouseInfo mouse_info;
extern RTCInfo time_info;
extern MenuState menuState;
extern uint8_t balls;
extern Position* ball_positions;
extern Position* code_positions;
extern PositionSmall* small_ball_positions;
extern Position* ball_box_positions;
extern PositionSmall* small_ball_box_positions;
extern uint32_t color_table[11];
extern uint8_t player_no;
extern uint8_t player_one_won;
extern uint8_t player_two_cheating;
extern int8_t curr_turn;
extern uint8_t hide_code;
extern uint32_t code_colors[4];

extern Sprite *mouse;
extern Sprite *hand;
extern Sprite *masterminix;
extern Sprite *start;
extern Sprite *button1;
extern Sprite *button2;
extern Sprite *button3;
extern Sprite *button4;
extern Sprite *exit_menu;
extern Sprite *board;
extern Sprite *board2;
extern Sprite *ball;
extern Sprite *small_ball;
extern Sprite *um;
extern Sprite *dois;
extern Sprite *tres;
extern Sprite *quatro;
extern Sprite *cinco;
extern Sprite *seis;
extern Sprite *sete;
extern Sprite *oito;
extern Sprite *w;
extern Sprite *r;
extern Sprite *toggle9;
extern Sprite *code_guessed;
extern Sprite *code_not_guessed;
extern Sprite *madeira;
extern Sprite *frase_menu;
extern Sprite *press_s;
extern Sprite *player_cheated;



bool firstFrame = true;
Sprite *background[6];
uint8_t bg_size;
uint32_t bg_color;
uint32_t time_colors[24] = {COLOR_TIME1, COLOR_TIME2, COLOR_TIME3, COLOR_TIME4, COLOR_TIME5, COLOR_TIME6, COLOR_TIME7, COLOR_TIME8, COLOR_TIME9, COLOR_TIME10, COLOR_TIME11, COLOR_TIME12, COLOR_TIME13, COLOR_TIME14, COLOR_TIME15, COLOR_TIME16, COLOR_TIME17, COLOR_TIME18, COLOR_TIME19, COLOR_TIME20, COLOR_TIME21, COLOR_TIME22, COLOR_TIME23, COLOR_TIME24};
 
/**
 * @brief Prepares the frame buffers \n 
 * Allocates the main frame buffer in VRAM and the secondary buffer in the process' address space
 * @param mode Mode to be used
 * @return 1 on failure, 0 otherwise
 */
int set_frame_buffers(uint16_t mode) {
    if (set_frame_buffer(mode, &main_frame_buffer)) return 1;
    frame_buffer_size = mode_info.XResolution * mode_info.YResolution * ((mode_info.BitsPerPixel + 7) / 8);
    back_buffer = (uint8_t *) malloc(frame_buffer_size);
    if (back_buffer == NULL) return 1;
    return 0;
}

/**
 * @brief Double buffering function \n 
 * Copies the contents of the secondary buffer to the main buffer
 */
void copy_buffer() {
    memcpy(main_frame_buffer, back_buffer, frame_buffer_size);
}

/**
 * @brief Draws a frame \n 
 * Draws the next frame on the frame buffer according to the game's state
 */
void draw_new_frame() {
    if (firstFrame) {
        firstFrame = false;
        switch (menuState) {
            case START:
                draw_initial_menu();
                break;
            case GAME:
                draw_game_menu();
                break;
            case END:
                draw_finish_menu();
                break;
        }
    }    
    clean_mouse();
    draw_balls();
    draw_code();
    draw_small_balls();
    draw_numbers_and_balls_in_box();
    draw_lid();
    draw_final_code();
    draw_mouse();
}

/**
 * @brief Draws the balls in the end menu 
 */
void draw_final_code() {
    if (menuState != END) return;
    for (uint i = 0; i < 4; i++) {
        uint32_t curr_color;
        if (player_no == 1) {
            curr_color = code_colors[i];
        }
        else {
            curr_color = code_positions[i].color;
        }
        draw_ball(ball, 300 + i*56, mode_info.YResolution/2 - 50, curr_color);
    }
}

/**
 * @brief Draws the initial menu \n 
 * Draws the initial menu and sets background
 */
void draw_initial_menu() {
    set_background_color();
    fill_rectangle(0, 0, mode_info.XResolution, mode_info.YResolution, bg_color);
    draw_sprite_xpm(masterminix, mode_info.XResolution/2 - 200, mode_info.YResolution/2 - 180);
    draw_sprite_xpm(start, mode_info.XResolution/2 - 60, mode_info.YResolution/2 - 40);
    draw_sprite_xpm(exit_menu, mode_info.XResolution/2 - 37, mode_info.YResolution/2 + 85);
    draw_sprite_xpm(frase_menu, mode_info.XResolution/2 - 325, mode_info.YResolution - 54);
    bg_size = 4;
    background[1] = masterminix;
    background[2] = start;
    background[3] = exit_menu;
    background[0] = frase_menu;
}

/**
 * @brief Draws the game menu \n 
 * Draws the game menu and sets background
 */
void draw_game_menu() {
    set_background_color();
    fill_rectangle(0, 0, mode_info.XResolution, mode_info.YResolution, bg_color);
    Sprite* drawing_board = player_no == 1 ? board : board2;
    draw_sprite_xpm(drawing_board, mode_info.XResolution/2 - board->width/2, 0);
    bg_size = 1;
    background[0] = drawing_board;
    if (player_no == 2) {
        draw_sprite_xpm(toggle9, 2, 5);
        bg_size = 2;
        background[1] = toggle9;
    }
}

/**
 * @brief Draws the final menu \n 
 * Draws the final menu and sets background
 */
void draw_finish_menu() {
    bg_size = 0;
    set_background_color();
    fill_rectangle(0, 0, mode_info.XResolution, mode_info.YResolution, bg_color);
    
    if (player_one_won == 1){
        
        draw_sprite_xpm(code_guessed, mode_info.XResolution/2 - 190, mode_info.YResolution/4 - 27);
        background[bg_size] = code_guessed;
        bg_size ++;
    }
    if (player_one_won == 0){
        draw_sprite_xpm(code_not_guessed, mode_info.XResolution/2 - 282, mode_info.YResolution/4 - 27);
        background[bg_size] = code_not_guessed;
        bg_size ++;
    }
    if (player_two_cheating == 1){
        draw_sprite_xpm(player_cheated, mode_info.XResolution/2 - 467/2, mode_info.YResolution/2 + 35);
        background[bg_size] = player_cheated;
        bg_size ++;
    }

    for (uint i = 0; i < 4; i++) {
        uint32_t curr_color;
        if (player_no == 1) {
            curr_color = code_colors[i];
        }
        else {
            curr_color = code_positions[i].color;
            draw_ball(ball, 300 + i*56, mode_info.YResolution/2 - 50, code_positions[i].color);
        }
    }

    draw_sprite_xpm(start, mode_info.XResolution/4 - 120/2, mode_info.YResolution - 50);
        background[bg_size] = start;
        bg_size ++;

    if (player_no == 1) {
        draw_sprite_xpm(exit_menu, 3 * mode_info.XResolution/4 - 74/2, mode_info.YResolution - 50);
            background[bg_size] = exit_menu;
            bg_size ++;
    }        
}

/**
 * @brief Draws the code \n 
 * Only applies to player 2, when in the game menu
 */
void draw_code() {  
    if (code_positions == NULL || menuState != GAME) return;
    for (uint8_t i = 0; i < 4; i++) {
        draw_ball(ball, code_positions[i].x, code_positions[i].y, code_positions[i].color);
    }
}

/**
 * @brief Draws the lid \n 
 * Only applies to player 2, when in the game menu
 */
void draw_lid(){
    if (hide_code) {
        fill_rectangle(186, 0, 230, 59, GREEN);
    }
}

/**
 * @brief Cleans the lid \n 
 * Draws the background behind the now hidden lid
 */
void clean_lid(){
    fill_rectangle(186, 0, 230, 59, bg_color);
    for (int i = 0; i < bg_size; i++) draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, 186 - background[i]->x, 0 - background[i]->y, 59, 230);
                
}

/**
 * @brief Draws the balls that the player can pick up \n 
 * If the player is not in the game menu, this function returns without doing anything
 */
void draw_numbers_and_balls_in_box(){

    if(menuState != GAME) return;
    Sprite *numbers[8] = {um,dois,tres,quatro,cinco,seis,sete,oito};
    Sprite *numbers_small[2] = {r,w};

    int color[] = {LIGHTBLUE, GREEN, YELLOW, DARKBLUE, RED, PINK, ORANGE, PURPLE};

    for (int j = 0; j < 8; j++) {
        draw_sprite_xpm(numbers[j], 570, 84 + (j % 8) * 66);
        draw_ball(ball, ball_box_positions[j].x, ball_box_positions[j].y, color[j]);
    } 
    for( int j= 0; j < 2; j++){
        draw_sprite_xpm(numbers_small[j], 525 + (j % 2) * 36, 10);
        draw_ball(small_ball, small_ball_box_positions[j].x, small_ball_box_positions[j].y, color_table[j+1]);
    }
 
}

/**
 * @brief Draws the large balls \n 
 * Draws the balls from the ball_positions array
 */
void draw_balls() {
    if (menuState != GAME) return;
    for (uint8_t i = 0; i < 9 * 4; i++) {
        draw_ball(ball, ball_positions[i].x, ball_positions[i].y, ball_positions[i].color);
    }
}

/**
 * @brief Draws the small balls \n 
 * Draws the small balls from the small_ball_positions array
 */
void draw_small_balls() {
    if (menuState != GAME) return;
    for (uint8_t i = 0; i < 9 * 4; i++) {
        draw_ball(small_ball, small_ball_positions[i].x, small_ball_positions[i].y, small_ball_positions[i].color);
    }
}

/**
 * @brief Draws a ball \n 
 * Puts a ball in the frame buffer, it can be any color
 * @param sprite Sprite to draw (ball or small_ball)
 * @param x x coordinate to draw ball in
 * @param y y coordinate to draw ball in
 * @param color Color to be used (for small balls, 0 is transparent, 1 is black, 2 is white)
 * @return 1 on failure, 0 otherwise
 */
int draw_ball(Sprite *sprite, int x, int y, uint32_t color) {
    if (color == TRANSPARENT || (sprite == small_ball && color == 0)) return 0;
    if (sprite == small_ball && color == 1) color = 0;
    if (sprite == small_ball && color == 2) color = 0xFFFFFF;
    uint16_t height = sprite->height;
    uint16_t width = sprite->width;
    sprite->x = x;
    sprite->y = y;
    ball->x = x;
    ball->y = y;
    uint32_t current_color;
    for (int h = 0 ; h < height ; h++) {
      for (int w = 0 ; w < width ; w++) {
        current_color = sprite->colors[w + h*width];
        if (current_color == TRANSPARENT) continue;
        if (x + w < 0 || y + h < 0) continue;
        if (paint_pixel(x + w, y + h, color) != 0) return 1;
      }
    }
    return 0; 
}



/**
 * @brief Draws the mouse \n 
 * Draws a cursor, and a ball behind it in case the player is holding one
 */
void draw_mouse() {
    switch (menuState) {
        case START: case END:
            draw_sprite_xpm(mouse, mouse_info.x, mouse_info.y);
            break;
        case GAME:
            if (mouse_info.ball_color > 2) draw_ball(ball, mouse_info.x - ball->width/2, mouse_info.y - ball->height/2, mouse_info.ball_color);
            else if (mouse_info.ball_color <= 2 && mouse_info.ball_color > 0) {
                draw_ball(small_ball, mouse_info.x - small_ball->width/2, mouse_info.y - small_ball->height/2, mouse_info.ball_color);
            }
            draw_sprite_xpm(mouse, mouse_info.x, mouse_info.y);
            break;
    }
}

/**
 * @brief Cleans the previous position the mouse was in \n 
 * Redraws the background, layer by layer, in the mouse's position during the previous frame
 */
void clean_mouse() {
    switch (menuState) {
        case START:
            fill_rectangle(mouse->x, mouse->y, mouse->width, mouse->height, bg_color);
            for (int i = 0; i < bg_size; i++) draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, mouse->x - background[i]->x, mouse->y - background[i]->y, mouse->height, mouse->width);
            break;
       case GAME:
            if (mouse_info.ball_color == 0) {
                fill_rectangle(mouse->x, mouse->y, mouse->width, mouse->height, bg_color);
                for (int i = 0; i < bg_size; i++) draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, mouse->x - background[i]->x, mouse->y - background[i]->y, mouse->height, mouse->width);
            }
            else {
                if (mouse_info.ball_color <= 2 && mouse_info.ball_color > 0) {
                    fill_rectangle(ball->x, ball->y, ball->width, ball->height, bg_color);
                    for (int i = 0; i < bg_size; i++) draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, ball->x - background[i]->x, ball->y - background[i]->y, ball->height, ball->width);
                }
                else {
                    fill_rectangle(ball->x, ball->y, ball->width, ball->height, bg_color);
                    for (int i = 0; i < bg_size; i++) draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, ball->x - background[i]->x, ball->y - background[i]->y, ball->height, ball->width);
                }
            }
            if(player_no == 2 && hide_code){
                fill_rectangle(186, 0, 230, 59, GREEN);
            }
            break;
        case END:
            fill_rectangle(mouse->x, mouse->y, mouse->width, mouse->height, bg_color);
            for (int i = 0; i < bg_size; i++) draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, mouse->x - background[i]->x, mouse->y - background[i]->y, mouse->height, mouse->width);
            break;

    }
}

/**
 * @brief Draws a sprite \n 
 * Draws the entirety of a sprite object's colors on the frame buffer
 * @param sprite Sprite to be drawn
 * @param x x coordinate to draw in
 * @param y y coordinate to draw in
 * @return 1 on failure, 0 otherwise
 */
int draw_sprite_xpm(Sprite *sprite, int x, int y) { 
    uint16_t height = sprite->height;
    uint16_t width = sprite->width;

    uint16_t initial_x = x;
    uint32_t num_pixels = width * height;
    sprite->x = x;
    sprite->y = y;
    uint32_t current_color;

    for (uint32_t i = 0; i < num_pixels; i++) {
      current_color = sprite->colors[i];
      if (current_color != TRANSPARENT) {
        if (paint_pixel(x, y, current_color)) return 1;
      }
      x = (x + 1) % (width + initial_x);
      if (x == 0) {
        y++;
        x = initial_x;
      }
    }

    return 0;
}

/**
 * @brief Draws a sprite partially \n 
 * This function is used when redrawing the background behind moving/cleaned objects, thus avoiding the need to redraw the entire frame buffer per frame
 * @param sprite Sprite to be partially drawn
 * @param x x coordinate in which to draw sprite
 * @param y y coordinate in which to draw sprite
 * @param xdraw x coordinate from which to start paiting pixels (relative to the sprite, that is, 0 equates to the parameter passed for "x")
 * @param ydraw y coordinate from which to start painting pixels (relative to the sprite, that is, 0 equates to the parameter passed for "y")
 * @param height Height of fragment to be drawn
 * @param width Width of fragment to be drawn
 * @return 1 on failure, 0 otherwise
 */
int draw_partial_sprite_xpm(Sprite *sprite, int x, int y, int xdraw, int ydraw, int height, int width) {
  //if (xdraw + width < x || ydraw + height < y) return 0; 
  
  int initial_x = xdraw;
  uint16_t num_pixels = width * height;
  uint32_t current_color;

  for (uint16_t i = 0; i < num_pixels; i++) {
    if (xdraw < 0 || ydraw < 0 || xdraw >= sprite->width || ydraw >= sprite->height) {
        xdraw++;
        if (xdraw == initial_x + width) {
            ydraw++;
            xdraw = initial_x;
        }
    }
    else {
        current_color = sprite->colors[xdraw + ydraw * sprite->width];
        if (current_color != TRANSPARENT) 
            if (paint_pixel(x + xdraw, y + ydraw, current_color)) return 1;
        xdraw++;
        if (xdraw == initial_x + width) {
            ydraw++;
            xdraw = initial_x;
        }
    }    
  }

  return 0;

}

/**
 * @brief Cleans a ball from the frame buffer after it is removed \n 
 * Redraws the background behind the removed ball
 * @param k Index of ball to be cleaned
 * @param sprite Sprite of ball (ball or small_ball)
 * @param positions Position array to check in (irrelevant for small balls)
 */
void clean_ball(uint8_t k, Sprite* sprite, Position* positions) {
    if (sprite == ball) {
        fill_rectangle(positions[k].x, positions[k].y, sprite->width, sprite->height, bg_color);
        for (int i = 0; i < bg_size; i++) {
            draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, positions[k].x - background[i]->x, positions[k].y - background[i]->y, sprite->height, sprite->width);
        }
    }
    else if (sprite == small_ball) {
        fill_rectangle(small_ball_positions[k].x, small_ball_positions[k].y, sprite->width, sprite->height, bg_color);
        for (int i = 0; i < bg_size; i++) {
            draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, small_ball_positions[k].x - background[i]->x, small_ball_positions[k].y - background[i]->y, sprite->height, sprite->width);
        }
    }
}

/**
 * @brief Set the background color according to current hour
 */
void set_background_color() {
    bg_color = time_colors[time_info.hours];
}

