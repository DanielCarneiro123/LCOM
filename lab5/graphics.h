#include <lcom/lcf.h>
#include "VBE.h"

int (set_graphic_mode)(uint16_t submode);

int (set_text_mode)();

int (get_mode_info)(uint16_t mode, vbe_mode_info_t *mode_info);

int (set_frame_buffer)(uint16_t mode); 

uint32_t (pixel_index)(uint16_t x, uint16_t y);

uint32_t (frame_buffer_index)(uint16_t x, uint16_t y);

int (paint_pixel)(uint16_t x, uint16_t y, uint32_t color);

int (draw_line)(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color);

int (fill_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
