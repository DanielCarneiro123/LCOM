#ifndef _LCOM_SPRITE_H_
#define _LCOM_SPRITE_H_

#include "driver/video/graphics.h"
#include "driver/mouse/mouse.h"

typedef struct {
    uint16_t height;
    uint16_t width;
    uint32_t *colors;
    uint32_t color;
    uint8_t pressed;
    int32_t x;
    int32_t y;
} Sprite; 

Sprite *create_sprite_xpm(xpm_map_t sprite);
void destroy_sprite(Sprite *sprite);
bool is_mouse_in_sprite(Sprite *sprite);

#endif
