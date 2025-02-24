#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <lcom/lcf.h>
#include "VBE.h"

vbe_mode_info_t mode_info;

int (set_graphic_mode)(uint16_t submode);
int (set_frame_buffer)(uint16_t mode, uint8_t** frame_buffer);
int (paint_pixel)(uint16_t x, uint16_t y, uint32_t color);
int (fill_rectangle)(int32_t x, int32_t y, uint16_t width, uint16_t height, uint32_t color);
int (get_mode_info)(uint16_t mode, vbe_mode_info_t *mode_info);
uint32_t (frame_buffer_index)(uint16_t x, uint16_t y);
uint32_t (pixel_index)(uint16_t x, uint16_t y);
int (draw_line)(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);

#endif
