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
extern PositionBallsBox* ball_box_positions;

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
extern Sprite *ball;
extern Sprite *small_ball;
extern Sprite *um;
extern Sprite *dois;
extern Sprite *tres;
extern Sprite *quatro;
extern Sprite *cinco;

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
    if (timer_interrupts % GAME_FREQUENCY == 0) display_real_time();
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
    bg_color = RED;
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
    bg_color = GREEN;
    fill_rectangle(0, 0, mode_info.XResolution, mode_info.YResolution, bg_color, drawing_frame_buffer);
    //draw_sprite_button(button2, mode_info.XResolution/2, 0);
    //draw_sprite_button(button3, 0, mode_info.YResolution/2);
    //draw_sprite_button(button4, mode_info.XResolution/2, mode_info.YResolution/2);
    draw_sprite_xpm(board, mode_info.XResolution/2 - board->width/2, 0);
    bg_size = 1;
    background[0] = board;
}

// O menu final é apenas um retângulo com tamanho máximo, com um smile ao centro
void draw_finish_menu() {
    bg_color = DARKBLUE;
    fill_rectangle(0, 0, mode_info.XResolution, mode_info.YResolution, bg_color, drawing_frame_buffer);
    draw_sprite_xpm(masterminix, mode_info.XResolution/2 - 100, mode_info.YResolution/2 - 100);
    bg_size = 1;
    background[0] = masterminix;
}

void draw_code() {
    if (code_positions == NULL || menuState != GAME) return;
    for (uint8_t i = 0; i < 4; i++) {
        draw_ball(ball, code_positions[i].x, code_positions[i].y, code_positions[i].color);
    }
}

void draw_numbers_and_balls_in_box(){

    if(menuState != GAME) return;
    Sprite *numbers[5] = {um,dois,tres,quatro,cinco};

    int color[] = {RED, GREEN, DARKBLUE, YELLOW, BLUE};

    for (int j = 0; j < 5; j++) {
        draw_sprite_xpm(numbers[j], 570, 135 + (j % 5) * 80);
        draw_ball(ball, ball_box_positions[j].x, ball_box_positions[j].y, color[j]);
    }  
}

void draw_balls() {
    if (menuState != GAME) return;
    for (uint8_t i = 0; i < 9 * 4; i++) {
        draw_ball(ball, ball_positions[i].x, ball_positions[i].y, ball_positions[i].color);
    }
}

void draw_small_balls() {
    if (menuState != GAME) return;
    for (uint8_t i = 0; i < 9 * 4; i++) {
        draw_ball(small_ball, small_ball_positions[i].x, small_ball_positions[i].y, small_ball_positions[i].color);
    }
}

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


// O cursor mode ter dois estados:
// - "normal", quando está no menu de início ou de fim
// - "mão", quando está no menu com os botões
void draw_mouse() {
    switch (menuState) {
        case START: case END:
            draw_sprite_xpm(mouse, mouse_info.x, mouse_info.y);
            break;
        case GAME:
            if (mouse_info.ball_color > 2) draw_ball(ball, mouse_info.x - ball->width/2, mouse_info.y - ball->height/2, mouse_info.ball_color);
            else draw_ball(small_ball, mouse_info.x - small_ball->width/2, mouse_info.y - small_ball->height/2, mouse_info.ball_color);
            draw_sprite_xpm(mouse, mouse_info.x, mouse_info.y);
            break;
    }
}

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
                if (mouse_info.ball_color == 1 || mouse_info.ball_color == 0xFFFFFF) {
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

// A função recebe um objeto Sprite proveniente de um XPM e mostra-o nas coordenadas (x, y)
// Usa as cores dinamicamente alocadas na altura da construção
// A função ignora a cor transparente do XPM para não modificar o fundo quando não é preciso
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

// A função recebe um objeto Sprite de cor constante e mostra-o nas coordenadas (x, y)
// Usa apenas uma cor, alocada na altura da construção
// A função ignora a cor transparente do XPM para não modificar o fundo quando não é preciso
int draw_sprite_button(Sprite *sprite, int x, int y) { 
    uint16_t height = sprite->height;
    uint16_t width = sprite->width;
    uint32_t color = sprite->pressed ? PRESSED : sprite->color;
    for (int h = 0 ; h < height ; h++) {
      for (int w = 0 ; w < width ; w++) {
        if (paint_pixel(x + w, y + h, color, drawing_frame_buffer) != 0) return 1;
      }
    }
    return 0; 
}

// Faz o display do tempo real num formato amigável
// No caso do Template esta função apenas retorna uma string para o ficheiro output.txt
// Em projetos pode ser mudada para invocar sprites que coloquem no ecrã os respetivos dígitos
void display_real_time() {
    //printf("NOW: %d/%d/%d @%d:%d:%d\n", 2000 + time_info.year, time_info.month, time_info.day, time_info.hours, time_info.minutes, time_info.seconds);
}

void clean_ball(uint8_t k, Sprite* sprite) {
    if (sprite == ball) {
        fill_rectangle(ball_positions[k].x, ball_positions[k].y, sprite->width, sprite->height, bg_color, drawing_frame_buffer);
        for (int i = 0; i < bg_size; i++) {
            draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, ball_positions[k].x - background[i]->x, ball_positions[k].y - background[i]->y, sprite->height, sprite->width);
        }
    }
    else if (sprite == small_ball) {
        fill_rectangle(small_ball_positions[k].x, small_ball_positions[k].y, sprite->width, sprite->height, bg_color, drawing_frame_buffer);
        for (int i = 0; i < bg_size; i++) {
            draw_partial_sprite_xpm(background[i], background[i]->x, background[i]->y, small_ball_positions[k].x - background[i]->x, small_ball_positions[k].y - background[i]->y, sprite->height, sprite->width);
        }
    }
}

