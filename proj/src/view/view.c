#include "view.h"

// Variáveis externas importantes à visualização do modelo e dos seus estados
uint8_t *main_frame_buffer;
uint8_t *secondary_frame_buffer;
uint8_t *drawing_frame_buffer;
uint32_t frame_buffer_size;
extern int timer_interrupts;
extern vbe_mode_info_t mode_info;
extern MouseInfo mouse_info;
extern rtc_info time_info;
extern MenuState menuState;
extern uint8_t balls;
extern Position* ball_positions;
extern Position* code_positions;
extern PositionSmall* small_ball_positions;
extern Position* ball_box_positions;
extern PositionSmall* small_ball_box_positions;
extern uint32_t color_table[8];
extern uint8_t player_no;
extern uint8_t player_one_won;
extern uint8_t player_two_cheating;
extern int8_t curr_turn;


// Objetos
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
extern Sprite *code_guessed;
extern Sprite *code_not_guessed;
extern Sprite *madeira;

bool firstFrame = true;
Sprite *background[5];
uint8_t bg_size;
uint32_t bg_color;

// Alocação de memória ao(s) buffer(s)
// Se houver só um buffer, esse é o principal
// Se houver double buffering, há um secundário a alocar a mesma quantidade de memória que serve
// exclusivamente para o desenho
int set_frame_buffers(uint16_t mode) {
    if (set_frame_buffer(mode, &main_frame_buffer)) return 1;
    frame_buffer_size = mode_info.XResolution * mode_info.YResolution * ((mode_info.BitsPerPixel + 7) / 8);
    if (DOUBLE_BUFFER) {
        secondary_frame_buffer = (uint8_t *) malloc(frame_buffer_size);
        drawing_frame_buffer = secondary_frame_buffer;
    } else {
        drawing_frame_buffer = main_frame_buffer;
    }
    return 0;
}

// Double buffering
// Cópia para o frame buffer principal do frame construído desde a última atualização
// Otimizaçṍes: 
// A) como o swap é uma operação muito frequente, é melhor não estar  a calcular frame_buffer_size sempre. 
// Assim opta-se por uma variável global, que é constante ao longo da execução e calculada 1 vez na linha 30.
// Poupa-se (frequência * (2 multiplicações + 1 soma + 1 divisão)) operações por cada segundo.
// B) só vale a pena dar display do RTC quando passa um segundo
void swap_buffers() {
    memcpy(main_frame_buffer, secondary_frame_buffer, frame_buffer_size);
}

// A construção de um novo frame é baseado:
// - no estado atual do modelo (menuState, mouse_info, mode_info, buttonX->pressed...);
// - no Algoritmo do Pintor - https://pt.wikipedia.org/wiki/Algoritmo_do_pintor
// A ideia é colocar no buffer primeiro o plano mais longe do observador (a cor do fundo) e só depois 
// os objetos em cima, no caso do cursor e / ou dos botões
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
                resetTable();
                draw_finish_menu();
                break;
        }
    }    
    clean_mouse();
    draw_balls();
    draw_code();
    draw_small_balls();
    draw_numbers_and_balls_in_box();
    draw_mouse();
}

// O menu inicial é apenas um retângulo com tamanho máximo, com um smile ao centro
void draw_initial_menu() {
    set_background_color();
    fill_rectangle(0, 0, mode_info.XResolution, mode_info.YResolution, bg_color, drawing_frame_buffer);
    draw_sprite_xpm(masterminix, mode_info.XResolution/2 - 200, mode_info.YResolution/2 - 180);
    draw_sprite_xpm(start, mode_info.XResolution/2 - 60, mode_info.YResolution/2 - 40);
    draw_sprite_xpm(exit_menu, mode_info.XResolution/2 - 37, mode_info.YResolution/2 + 40);
    bg_size = 3;
    background[0] = masterminix;
    background[1] = start;
    background[2] = exit_menu;
}

// O menu do jogo é constituído por quatro botões
void draw_game_menu() {
    set_background_color();
    fill_rectangle(0, 0, mode_info.XResolution, mode_info.YResolution, bg_color, drawing_frame_buffer);
    //draw_sprite_button(button2, mode_info.XResolution/2, 0);
    //draw_sprite_button(button3, 0, mode_info.YResolution/2);
    //draw_sprite_button(button4, mode_info.XResolution/2, mode_info.YResolution/2);
    Sprite* drawing_board = player_no == 1 ? board : board2;
    draw_sprite_xpm(drawing_board, mode_info.XResolution/2 - board->width/2, 0);
    bg_size = 1;
    background[0] = drawing_board;
}

// O menu final é apenas um retângulo com tamanho máximo, com um smile ao centro
void draw_finish_menu() {
    set_background_color();
    fill_rectangle(0, 0, mode_info.XResolution, mode_info.YResolution, bg_color, drawing_frame_buffer);
    if (player_one_won == 1){
        draw_sprite_xpm(code_guessed, mode_info.XResolution/2 - 190, mode_info.YResolution/2 - 27);
        bg_size = 1;
        background[0] = code_guessed;
    }
    else if (curr_turn == 8){
        draw_sprite_xpm(code_not_guessed, mode_info.XResolution/2 - 282, mode_info.YResolution/2 - 27);
        bg_size = 1;
        background[0] = code_not_guessed;
    }
    else{
        draw_sprite_xpm(madeira, mode_info.XResolution/2 - 100, mode_info.YResolution/2 - 100);
        bg_size = 1;
        background[0] = madeira;
    }
    

}

/**
 * @brief Draws the code
 * Only applies to player 2, when in the game menu
 */
void draw_code() {  
    if (code_positions == NULL || menuState != GAME) return;
    for (uint8_t i = 0; i < 4; i++) {
        draw_ball(ball, code_positions[i].x, code_positions[i].y, code_positions[i].color);
    }
}

void toggle_code_view(uint8_t hide_code){
    if(hide_code){
        fill_rectangle(186, 0, 230, 59, GREEN, drawing_frame_buffer);
    }

    else {
        fill_rectangle(186, 0, 230, 59, bg_color, drawing_frame_buffer);
        }
}

/**
 * @brief Draws the balls that the player can pick up
 * If the player is not in the game menu, this function returns without doing anything
 */
void draw_numbers_and_balls_in_box(){

    if(menuState != GAME) return;
    Sprite *numbers[8] = {um,dois,tres,quatro,cinco,seis,sete,oito};

    int color[] = {LIGHTBLUE, GREEN, YELLOW, DARKBLUE, RED, PINK, ORANGE, PURPLE};

    for (int j = 0; j < 8; j++) {
        draw_sprite_xpm(numbers[j], 570, 84 + (j % 8) * 66);
        draw_ball(ball, ball_box_positions[j].x, ball_box_positions[j].y, color[j]);
    } 
    for( int j= 0; j < 2; j++){
        draw_ball(small_ball, small_ball_box_positions[j].x, small_ball_box_positions[j].y, color_table[j+1]);
    }
 
}

/**
 * @brief Draws the large balls
 * Draws the balls from the ball_positions array
 */
void draw_balls() {
    if (menuState != GAME) return;
    for (uint8_t i = 0; i < 9 * 4; i++) {
        draw_ball(ball, ball_positions[i].x, ball_positions[i].y, ball_positions[i].color);
    }
}

/**
 * @brief Draws the small balls
 * Draws the small balls from the small_ball_positions array
 */
void draw_small_balls() {
    if (menuState != GAME) return;
    for (uint8_t i = 0; i < 9 * 4; i++) {
        draw_ball(small_ball, small_ball_positions[i].x, small_ball_positions[i].y, small_ball_positions[i].color);
    }
}

/**
 * @brief Draws a ball
 * Puts a ball in the frame buffer, it can be any color
 * @param sprite Sprite to draw (ball or small_ball)
 * @param x x coordinate to draw ball in
 * @param y y coordinate to draw ball in
 * @param color Color to be used (for small balls, 0 is transparent, 1 is black, 2 is white)
 * @return int 1 on failure, 0 otherwise
 */
int draw_ball(Sprite *sprite, int x, int y, uint32_t color) {
    if (color == TRANSPARENT || (sprite == small_ball && color == 0)) return 0;
    if (sprite == small_ball && color == 1) color = 0;
    if (sprite == small_ball && color == 2) color = 0xFFFFFF;
    uint16_t height = sprite->height;
    uint16_t width = sprite->width;
    sprite->x = x;
    sprite->y = y;
    uint32_t current_color;
    for (int h = 0 ; h < height ; h++) {
      for (int w = 0 ; w < width ; w++) {
        current_color = sprite->colors[w + h*width];
        if (current_color == TRANSPARENT) continue;
        if (x + w < 0 || y + h < 0) continue;
        if (paint_pixel(x + w, y + h, color, drawing_frame_buffer) != 0) return 1;
      }
    }
    return 0; 
}



/**
 * @brief Draws the mouse
 * Draws a cursor, and a ball behind it in case the player is holding one
 */
void draw_mouse() {
    switch (menuState) {
        case START: case END:
            draw_sprite_xpm(mouse, mouse_info.x, mouse_info.y);
            break;
        case GAME:
            if (mouse_info.ball_color > 2) draw_ball(ball, mouse_info.x - ball->width/2, mouse_info.y - ball->height/2, mouse_info.ball_color);
            else if (mouse_info.ball_color <= 2 && mouse_info.ball_color > 0) draw_ball(small_ball, mouse_info.x - ball->width/2, mouse_info.y - ball->height/2, mouse_info.ball_color);
            draw_sprite_xpm(mouse, mouse_info.x, mouse_info.y);
            break;
    }
}

/**
 * @brief Cleans the previous position the mouse was in
 * Redraws the background, layer by layer, in the mouse's position during the previous frame
 */
void clean_mouse() {
    switch (menuState) {
        case START: case END:
            fill_rectangle(mouse->x, mouse->y, mouse->width, mouse->height, bg_color, drawing_frame_buffer);
            for (int i = 0; i < bg_size; i++) draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, mouse->x - background[i]->x, mouse->y - background[i]->y, mouse->height, mouse->width);
            break;
       case GAME:
            if (mouse_info.ball_color == 0) {
                fill_rectangle(mouse->x, mouse->y, mouse->width, mouse->height, bg_color, drawing_frame_buffer);
                for (int i = 0; i < bg_size; i++) draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, mouse->x - background[i]->x, mouse->y - background[i]->y, mouse->height, mouse->width);
            }
            else {
                if (mouse_info.ball_color <= 2 && mouse_info.ball_color > 0) {
                    fill_rectangle(small_ball->x, small_ball->y, ball->width, ball->height, bg_color, drawing_frame_buffer);
                    for (int i = 0; i < bg_size; i++) draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, small_ball->x - background[i]->x, small_ball->y - background[i]->y, ball->height, ball->width);
                }
                else {
                    fill_rectangle(ball->x, ball->y, ball->width, ball->height, bg_color, drawing_frame_buffer);
                    for (int i = 0; i < bg_size; i++) draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, ball->x - background[i]->x, ball->y - background[i]->y, ball->height, ball->width);
                }
            }
            break;
    }
}

/**
 * @brief Draws a sprite
 * Draws the entirety of a sprite object's colors on the frame buffer
 * @param sprite Sprite to be drawn
 * @param x x coordinate to draw in
 * @param y y coordinate to draw in
 * @return int 
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
        if (paint_pixel(x, y, current_color, drawing_frame_buffer)) return 1;
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
 * @brief Draws a sprite partially
 * This function is used when redrawing the background behind moving/cleaned objects, thus avoiding the need to redraw the entire frame buffer per frame
 * @param sprite Sprite to be partially drawn
 * @param x x coordinate in which to draw sprite
 * @param y y coordinate in which to draw sprite
 * @param xdraw x coordinate from which to start paiting pixels (relative to the sprite, that is, 0 equates to the parameter passed for "x")
 * @param ydraw y coordinate from which to start painting pixels (relative to the sprite, that is, 0 equates to the parameter passed for "y")
 * @param height Height of fragment to be drawn
 * @param width Width of fragment to be drawn
 * @return int 1 on failure, 0 otherwise
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
            if (paint_pixel(x + xdraw, y + ydraw, current_color, drawing_frame_buffer)) return 1;
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
 * @brief Cleans a ball from the frame buffer after it is removed
 * Redraws the background behind the removed ball
 * @param k Index of ball to be cleaned
 * @param sprite Sprite of ball (ball or small_ball)
 * @param positions Position array to check in (irrelevant for small balls)
 */
void clean_ball(uint8_t k, Sprite* sprite, Position* positions) {
    if (sprite == ball) {
        fill_rectangle(positions[k].x, positions[k].y, sprite->width, sprite->height, bg_color, drawing_frame_buffer);
        for (int i = 0; i < bg_size; i++) {
            draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, positions[k].x - background[i]->x, positions[k].y - background[i]->y, sprite->height, sprite->width);
        }
    }
    else if (sprite == small_ball) {
        fill_rectangle(small_ball_positions[k].x, small_ball_positions[k].y, sprite->width, sprite->height, bg_color, drawing_frame_buffer);
        for (int i = 0; i < bg_size; i++) {
            draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, small_ball_positions[k].x - background[i]->x, small_ball_positions[k].y - background[i]->y, sprite->height, sprite->width);
        }
    }
}

/**
 * @brief Set the background color according to current hour
 */
void set_background_color() {
    if (time_info.hours == 0) {
        bg_color = COLOR_TIME1;
    }else if (time_info.hours == 1) {
        bg_color = COLOR_TIME2;
    } 
    else if (time_info.hours == 2) {
        bg_color = COLOR_TIME3;
    }
    else if (time_info.hours == 3) {
        bg_color = COLOR_TIME4;
    }
    else if (time_info.hours == 4) {
        bg_color = COLOR_TIME5;
    }
    else if (time_info.hours == 5) {
        bg_color = COLOR_TIME6;
    }
    else if (time_info.hours == 6) {
        bg_color = COLOR_TIME7;
    }
    else if (time_info.hours == 7) {
        bg_color = COLOR_TIME8;
    }
    else if (time_info.hours == 8) {
        bg_color = COLOR_TIME9;
    }
    else if (time_info.hours == 9) {
        bg_color = COLOR_TIME10;
    }
    else if (time_info.hours == 10) {
        bg_color = COLOR_TIME11;
    }
    else if (time_info.hours == 11) {
        bg_color = COLOR_TIME12;
    }
    else if (time_info.hours == 12) {
        bg_color = COLOR_TIME13;
    }
    else if (time_info.hours == 13) {
        bg_color = COLOR_TIME14;
    }
    else if (time_info.hours == 14) {
        bg_color = COLOR_TIME15;
    }
    else if (time_info.hours == 15) {
        bg_color = COLOR_TIME16;
    }
    else if (time_info.hours == 16) {
        bg_color = COLOR_TIME17;
    }
    else if (time_info.hours == 17) {
        bg_color = COLOR_TIME18;
    }
    else if (time_info.hours == 18) {
        bg_color = COLOR_TIME19;
    }
    else if (time_info.hours == 19) {
        bg_color = COLOR_TIME20;
    }
    else if (time_info.hours == 20) {
        bg_color = COLOR_TIME21;
    }
    else if (time_info.hours == 21) {
        bg_color = COLOR_TIME22;
    }
    else if (time_info.hours == 22) {
        bg_color = COLOR_TIME23;
    }
    else {
        bg_color = COLOR_TIME24;
    } 
    printf("\n\n\nTIME %d \n\n\n", time_info.hours);
    printf("\n\n\nBACKGROUND COLOR %d \n\n\n", bg_color);
}

