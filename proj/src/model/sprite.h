#ifndef _LCOM_SPRITE_H_
#define _LCOM_SPRITE_H_

#include "controller/video/graphics.h"

typedef struct {
    uint16_t height;
    uint16_t width;
    uint32_t *colors;
    uint32_t color;
    uint8_t pressed;
    uint16_t x;
    uint16_t y;
} Sprite; 

Sprite *create_sprite_xpm(xpm_map_t sprite);
void destroy_sprite(Sprite *sprite);

#endif
