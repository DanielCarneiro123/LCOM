#include <lcom/lcf.h>
#include "graphics.h"
#include <math.h>

uint8_t bpp;
extern uint8_t* back_buffer;

/**
 * @brief Sets the VM's graphic mode
 * Sets the VM's graphic mode through the VBE's 00h function
 * @param submode Mode to set
 * @return 1 on failure, 0 otherwise
 */
int (set_graphic_mode)(uint16_t submode) {
    reg86_t reg86;
    memset(&reg86, 0, sizeof(reg86)); 
    reg86.intno = BIOS_VIDEOCARD_SERV;                                
    reg86.ax = VBE_MODE_SET;             
    reg86.bx = submode | VBE_LINEAR_FB;     
    if (sys_int86(&reg86) != 0) {     
        return 1;
    }
    return 0;
}

/**
 * @brief Gets the information of a given VBE mode
 * @param mode Mode to get information on
 * @param mode_info Struct to be filled with the information
 * @return 1 on failure, 0 otherwise
 */
int get_mode_info(uint16_t mode, vbe_mode_info_t *mode_info) {
    memset(mode_info, 0, sizeof(*mode_info));
    if(vbe_get_mode_info(mode, mode_info)) return 1;
    return 0;
}

/**
 * @brief Maps the frame buffer to the process' address spac
 * @param mode Mode to use
 * @param frame_buffer Pointer to frame buffer to be used
 * @return 1 on failure, 0 otherwise
 */
int (set_frame_buffer)(uint16_t mode, uint8_t** frame_buffer) {
    get_mode_info(mode, &mode_info);

    bpp = (mode_info.BitsPerPixel + 7) / 8;
    unsigned int frame_size = mode_info.XResolution * mode_info.YResolution * bpp;

    struct minix_mem_range physic_addresses;
    physic_addresses.mr_base = mode_info.PhysBasePtr;
    physic_addresses.mr_limit = physic_addresses.mr_base + frame_size;

    if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &physic_addresses)) return 1;

    *frame_buffer = vm_map_phys(SELF, (void*) physic_addresses.mr_base, frame_size);
    if (frame_buffer == NULL) return 1;
    
    return 0;
}

/**
 * @brief Calculates the index of a pixel \n 
 * Calculates the index of a pixel, independently of the current mode
 * @param x x coordinate of pixel
 * @param y y coordinate of pxel
 * @return Index of pixel
 */
uint32_t pixel_index(uint16_t x, uint16_t y) {
    return y * mode_info.XResolution + x;
}

/**
 * @brief Calculates the offset of a pixel \n 
 * Calculates the offset of a pixel on the frame buffer
 * @param x x coordinate of pixel
 * @param y y coordinate of pixel
 * @return Offset of pixel
 */
uint32_t frame_buffer_index(uint16_t x, uint16_t y) {
    return pixel_index(x, y) * bpp;
}

/**
 * @brief Paints a pixel in the frame buffer
 * If the pixel is out of bounds, the function returns normally instead of with an error (this is useful for drawing sprites that are partially out of the screen)
 * @param x x coordinate of pixel
 * @param y y coordinate of pixel
 * @param color color of pixel
 * @return 1 on failure, 0 otherwise
 */
int (paint_pixel)(uint16_t x, uint16_t y, uint32_t color) {
    if (x >= mode_info.XResolution || y >= mode_info.YResolution) return 0;
    
    uint32_t index = frame_buffer_index(x, y);

    if (memcpy(&back_buffer[index], &color, bpp) == NULL) return 1;

    return 0;
}

/**
 * @brief Draws a horizontal or vertical line on the frame buffer
 * @param x1 x of first point
 * @param y1 y of first point
 * @param x2 x of second point
 * @param y2 y of second point
 * @param color Color of line
 * @return 1 on failure, 0 otherwise
 */
int (draw_line)(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color) {
    if (x1 == x2) {
        uint16_t min = new_min(y1, y2);
        uint16_t max = new_max(y1, y2);

        for (uint16_t i = min; i <= max; i++) {
            if (paint_pixel(x1, i, color)) return 1;
        }
        return 0;
    }
    else if (y1 == y2) {
        uint16_t min = new_min(x1, x2);
        uint16_t max = new_max(x1, x2);

        for (uint16_t i = min; i <= max; i++) {
            if (paint_pixel(i, y1, color)) return 1;
        }
        return 0;
    }

    return 1;
}

/**
 * @brief Fills a rectangle with a given color \n 
 * Fills a rectanglt with a given color on the frame buffer
 * @param x x of top-left corner
 * @param y y of top-right corner
 * @param width Width of rectangle
 * @param height Height of rectangle
 * @param color Color of rectangle
 * @return 1 on failure, 0 otherwise
 */
int fill_rectangle(int32_t x, int32_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (int32_t i = x; i < x + width; i++) {
        for (int32_t j = y; j < y + height; j++) {
            if (i < 0 || j < 0) continue;
            if (paint_pixel(i, j, color)) return 1;
        }
    }
    return 0;
}
