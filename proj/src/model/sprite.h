#ifndef _LCOM_SPRITE_H_
#define _LCOM_SPRITE_H_

#include "driver/video/graphics.h"
#include "driver/mouse/mouse.h"

typedef struct {
    uint16_t height; /** Height of sprite in pixels */
    uint16_t width; /** Width of sprite in pixels */
    uint32_t *colors; /** Color map of sprite */
    int32_t x; /** Current x coordinate of sprite */
    int32_t y; /** Current y coordinate of sprite */
} Sprite; 

Sprite *create_sprite(xpm_map_t sprite);
void destroy_sprite(Sprite *sprite);
bool is_mouse_in_sprite(Sprite *sprite);

#endif
