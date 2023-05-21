#include "model.h"

// Variáveis externas importantes à construção e manipulação do modelo
extern uint8_t scancode;
extern uint8_t byte_index;
SystemState systemState = RUNNING;
MenuState menuState = START;
extern MouseInfo mouse_info;
extern vbe_mode_info_t mode_info;
extern real_time_info time_info;
extern bool firstFrame;
extern uint8_t sp_data;

// Objetos a construir e manipular com a mudança de estados
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
Sprite *ball;
Sprite *small_ball;
Sprite *um;
Sprite *dois;
Sprite *tres;
Sprite *quatro;
Sprite *cinco;


Position* ball_positions;
PositionSmall* small_ball_positions;
PositionBallsBox* ball_box_positions;

uint8_t balls;
bool activeTurn = true;
int colorArr[5] = {RED, GREEN, DARKBLUE, YELLOW, BLUE};
uint32_t color_table[8] = {0, 1, 0xFFFFFF, RED, GREEN, DARKBLUE, YELLOW, BLUE};
uint8_t curr_turn = 0;

// Contador de interrupções do timer
int timer_interrupts = 0;

void update_menu_state(MenuState new_state) {
    firstFrame = true;
    menuState = new_state;
}

void setup_box_balls_positions() {
    ball_box_positions = 0;
    ball_box_positions = malloc(sizeof(PositionBallsBox) * 5 );
    for (int j = 0; j < 5; j++) {
        ball_box_positions[j].x = 500;
        ball_box_positions[j].y = 120 + (j % 5) * 80;
        ball_box_positions[j].color = TRANSPARENT;
    }  
}

void setup_positions() {
    balls = 0;
    ball_positions = malloc(sizeof(Position) * 9 * 4);
    for (int i = 0; i < 9 * 4; i++) {
        ball_positions[i].x = 190 + (i % 4) * 56;
        ball_positions[i].y = 66 + (i / 4) * 60;
        ball_positions[i].color = TRANSPARENT;
    }
}


void setup_small_positions() {
    small_ball_positions = malloc(sizeof(PositionSmall) * 36);
    for (int j = 0; j < 9; j++) {
            for (int i = 0; i < 4; i++) {
                small_ball_positions[j*4+i].x = 416 + (i % 2) * 26;
                small_ball_positions[j*4+i].y = (j*61) + (60 + (i / 2) * 30);
                if (j > 5) small_ball_positions[j*4+i].y -= 2;
                small_ball_positions[j*4+i].color = 0;
            }
        }  
}

void destroy_positions() {
    free(ball_positions);
}

void destroy_small_positions() {
    free(small_ball_positions);
}

// Criação dos objetos via XPM e via comum
void setup_sprites() {
    mouse = create_sprite_xpm((xpm_map_t) mouse_xpm);
    board = create_sprite_xpm((xpm_map_t) mastermind_boardV1_xpm);
    ball = create_sprite_xpm((xpm_map_t) ball_xpm);
    small_ball = create_sprite_xpm((xpm_map_t) small_ball_xpm);

    hand = create_sprite_xpm((xpm_map_t) hand_xpm);
    //smile = create_sprite_xpm((xpm_map_t) smile_xpm);
    masterminix = create_sprite_xpm((xpm_map_t) masterminix_xpm);
    start = create_sprite_xpm((xpm_map_t) start_xpm);
    exit_menu = create_sprite_xpm((xpm_map_t) exit_xpm);
    button1 = create_sprite_button(mode_info.XResolution/2, mode_info.YResolution/2, ORANGE);
    button2 = create_sprite_button(mode_info.XResolution/2, mode_info.YResolution/2, BLUE);
    button3 = create_sprite_button(mode_info.XResolution/2, mode_info.YResolution/2, GREEN);
    button4 = create_sprite_button(mode_info.XResolution/2, mode_info.YResolution/2, YELLOW);
    um = create_sprite_xpm((xpm_map_t) um_xpm);
    dois = create_sprite_xpm((xpm_map_t) dois_xpm);
    tres = create_sprite_xpm((xpm_map_t) tres_xpm);
    quatro = create_sprite_xpm((xpm_map_t) quatro_xpm);
    cinco = create_sprite_xpm((xpm_map_t) cinco_xpm);

}

// É boa prática antes de acabar o programa libertar a memória alocada
void destroy_sprites() {
    destroy_sprite(mouse);
    destroy_sprite(board);
    destroy_sprite(ball);
    destroy_sprite(small_ball);
    destroy_sprite(hand);
    destroy_sprite(masterminix);
    destroy_sprite(start);
    destroy_sprite(exit_menu);
    destroy_sprite(button1);
    destroy_sprite(button2);
    destroy_sprite(button3);
    destroy_sprite(button4);
    //destroy_sprite(masterminix);
    destroy_sprite(um);
    destroy_sprite(dois);
    destroy_sprite(tres);
    destroy_sprite(quatro);
    destroy_sprite(cinco);

}

// Na altura da interrupção há troca dos buffers e incremento do contador
void update_timer_state() {
    if (DOUBLE_BUFFER) swap_buffers();
    update_queue();
    timer_interrupts++;
}

void update_sp_state() {
    sp_ih();
}

// Como o Real Time Clock é um módulo mais pesado, 
// devemos só atualizar os valores quando passa um segundo
void update_rtc_state() {
    if (timer_interrupts % GAME_FREQUENCY == 0) rtc_update_time();
}

// Sempre que uma nova tecla é pressionada há avaliação do scancode.
// No caso do Template o teclado influencia:
// - o systemState: se Q for pressionado, leva ao fim do programa
// - o menuState: se S, G, E forem pressionados, leva a um dos menus (start, game, end) disponíveis
void update_keyboard_state() {
    (kbc_ih)();
    switch (scancode) {
        case Q_KEY:
            systemState = EXIT;
            break;
        case S_KEY:
            update_menu_state(START);
            break;
        case G_KEY:
            write_sp_data(143);
            update_menu_state(GAME);
            break;
        case E_KEY:
            read_sp_data();
            update_menu_state(END);
        case ONE_KEY:
            update_mouse_color(0xFF0000);
            break;
        case TWO_KEY:
            update_mouse_color(0x00FF00);
            break;
        case THREE_KEY:
            update_mouse_color(0x0000FF);
            break;
        case FOUR_KEY:
            update_mouse_color(0xFFFF00);
            break;
        case FIVE_KEY:
            update_mouse_color(0x00FFFF);
            break;        
        case ZERO_KEY:
            update_mouse_color(0);
            break;
        case W_KEY:
            update_mouse_color(0xFFFFFF);
            break;
        case P_KEY:
            place_small_ball();
            place_ball();    
            break;
        case O_KEY:
            remove_small_ball();
            remove_ball();
            break;    
        case ENTER_KEY:
            finish_turn();
            break;

        default:
            break;
    }
    draw_new_frame();
}

// Sempre que há um novo pacote completo do rato
// - muda o seu estado interno (x, y, left_pressed, right_pressed) - mouse_sync_info();
// - pode mudar o estado do botão por baixo dele - update_buttons_state();
void update_mouse_state() {
    (mouse_ih)();
    mouse_sync_bytes();
    if (byte_index == 3) {
        mouse_sync_info();
        update_buttons_state();
        draw_new_frame();
        byte_index = 0;
        if (mouse_info.left_click){
            pick_box_ball();
            place_small_ball();
            place_ball();    
        }
        if (mouse_info.right_click){
            remove_small_ball();
            remove_ball();    
        }
    }
}

// Se o rato tiver o botão esquerdo pressionado (mouse_info.left_click) então
// muda o estado do botão no mesmo quadrante
// Senão, todos os botões voltam a não estar pressionados (buttonX->pressed = 0;)
void update_buttons_state() {

    if (mouse_info.left_click) {

        if (mouse_info.x < mode_info.XResolution/2 && mouse_info.y < mode_info.YResolution/2)
            button1->pressed = 1;

        if (mouse_info.x >= mode_info.XResolution/2 && mouse_info.y <= mode_info.YResolution/2)
            button2->pressed = 1;

        if (mouse_info.x < mode_info.XResolution/2 && mouse_info.y >= mode_info.YResolution/2)
            button3->pressed = 1;

        if (mouse_info.x >= mode_info.XResolution/2 && mouse_info.y > mode_info.YResolution/2)
            button4->pressed = 1;

    } else {
        button1->pressed = 0;
        button2->pressed = 0;
        button3->pressed = 0;
        button4->pressed = 0;
    }
}

void update_mouse_color(uint32_t color) {
  if (menuState == GAME) {
        mouse_info.ball_color = color;
    }
}

void finish_turn() {
    if (menuState == GAME) {
        activeTurn = false;
        curr_turn++;
    }
}

bool is_mouse_in_ball(uint8_t i) {
    return mouse_info.x >= ball_positions[i].x && mouse_info.x <= ball_positions[i].x + ball->width && mouse_info.y >= ball_positions[i].y && mouse_info.y <= ball_positions[i].y + ball->height;
}

bool is_mouse_in_small_ball(uint8_t i) {
    return mouse_info.x >= small_ball_positions[i].x && mouse_info.x <= small_ball_positions[i].x + ball->width && mouse_info.y >= small_ball_positions[i].y && mouse_info.y <= small_ball_positions[i].y + ball->height;
}

bool is_mouse_in_ball_box(uint8_t i) {
    return mouse_info.x >= ball_box_positions[i].x && mouse_info.x <= ball_box_positions[i].x + ball->width && mouse_info.y >= ball_box_positions[i].y && mouse_info.y <= ball_box_positions[i].y + ball->height;
}

void place_ball() {
    if (menuState != GAME || balls >= 9*4) return;
    if (!activeTurn) return;
    for (int i = curr_turn * 4; i < (curr_turn + 1) * 4; i++) {
        if (is_mouse_in_ball(i)) {
            ball_positions[i].color = mouse_info.ball_color;
            uint8_t byte = prepare_move_byte(i/4, mouse_info.ball_color);
            printf("\n\n\nABOUT TO WRITE %d\n\n\n", byte);
            push(byte);
            return;
        }
    }
    //ball_positions[balls].x = mouse_info.x - ball->width/2;
    //ball_positions[balls].y = mouse_info.y - ball->height/2;
    //ball_positions[balls].color = mouse_info.ball_color;

    //balls++;
}

void pick_box_ball() {
    if (menuState != GAME) return;
    if (!activeTurn) return;
    for (int i = 0; i < 5; i++) {
        if (is_mouse_in_ball_box(i)) {
            update_mouse_color(colorArr[i]);
            return;       
        }
    }
    //ball_positions[balls].x = mouse_info.x - ball->width/2;
    //ball_positions[balls].y = mouse_info.y - ball->height/2;
    //ball_positions[balls].color = mouse_info.ball_color;

    //balls++;
}

void place_small_ball() {
    if (menuState != GAME || balls >= 9*4) return;
    if (!activeTurn) return;
    for (int i = 0; i < 9 * 4; i++) {
        if (is_mouse_in_small_ball(i)) {
            small_ball_positions[i].color = mouse_info.ball_color;
            return;
        }
    }
}

void remove_ball() {
    if (menuState != GAME) return;
    if (!activeTurn) return;
    //int16_t removed_index = -1;
    for (int i = 0; i < 9 * 4; i++) {
        if (is_mouse_in_ball(i)) {
            ball_positions[i].color = TRANSPARENT;
            clean_ball(i, ball);
            break;
        }
    }

    /*if (removed_index != -1) {
        for (int i = removed_index; i < balls - 1; i++) {
            ball_positions[i] = ball_positions[i + 1];
        }

        balls--;
    }*/
}

void remove_small_ball() {
    if (menuState != GAME) return;
    if (!activeTurn) return;
    for (int i = 0; i < 9 * 4; i++) {
        if (is_mouse_in_small_ball(i)) {
            small_ball_positions[i].color = 0;
            clean_ball(i, small_ball);
            break;
        }
    }
}
