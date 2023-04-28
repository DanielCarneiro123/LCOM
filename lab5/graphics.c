#include "graphics.h"

vbe_mode_info_t mode_info;
uint8_t bpp;
uint8_t* frame_buffer;

int set_graphic_mode(uint16_t submode) {
    reg86_t reg86;
    memset(&reg86, 0, sizeof(reg86)); 
    reg86.intno = BIOS_VIDEOCARD_SERV;                                
    reg86.ax = VBE_MODE_SET;             
    reg86.bx = submode | VBE_LINEAR_FB;     
    if (sys_int86(&reg86) != 0) {     
        printf("Set graphic mode failed\n");
        return 1;
    }
    return 0;
}

int set_text_mode() {
    reg86_t reg86;                       
    memset(&reg86, 0, sizeof(reg86));  
    reg86.intno = BIOS_VIDEOCARD_SERV;                 
    reg86.ah = BIOS_SET_VIDEOMODE;                    
    reg86.al = 0x03;                                                   
    if(sys_int86(&reg86) != 0) {        
        printf("Set text mode failed\n");
        return 1;
    }
    return 0;
}

int get_mode_info(uint16_t mode, vbe_mode_info_t *mode_info) {
    memset(mode_info, 0, sizeof(*mode_info));
    if(vbe_get_mode_info(mode, mode_info)) return 1;
    return 0;
}

int set_frame_buffer(uint16_t mode) {
    get_mode_info(mode, &mode_info);

    bpp = (mode_info.BitsPerPixel + 7) / 8;
    unsigned int frame_size = mode_info.XResolution * mode_info.YResolution * bpp;

    struct minix_mem_range physic_addresses;
    physic_addresses.mr_base = mode_info.PhysBasePtr;
    physic_addresses.mr_limit = physic_addresses.mr_base + frame_size;

    if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &physic_addresses)) return 1;

    frame_buffer = vm_map_phys(SELF, (void*) physic_addresses.mr_base, frame_size);
    if (frame_buffer == NULL) return 1;
    
    return 0;
}

uint32_t pixel_index(uint16_t x, uint16_t y) {
    return y * mode_info.XResolution + x;
}

uint32_t frame_buffer_index(uint16_t x, uint16_t y) {
    return pixel_index(x, y) * bpp;
}

int paint_pixel(uint16_t x, uint16_t y, uint32_t color) {
    if (x >= mode_info.XResolution || y >= mode_info.YResolution) return 1;
    
    uint32_t index = frame_buffer_index(x, y);

    if (memcpy(&frame_buffer[index], &color, bpp) == NULL) return 1;

    return 0;
}

int draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color) {
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

int fill_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (uint16_t i = x; i < x + width; i++) {
        for (uint16_t j = y; j < y + height; j++) {
            if (paint_pixel(i, j, color)) return 1;
        }
    }
    return 0;
}

int print_xpm(xpm_image_t img, uint16_t x, uint16_t y, const uint8_t* loaded_xpm) {
  if (loaded_xpm == NULL) return 1;
  uint16_t initial_x = x;
  uint16_t num_pixels = img.width * img.height;

  for (uint16_t i = 0; i < num_pixels; i++) {
    if (paint_pixel(x, y, loaded_xpm[i])) return 1;
    x = (x + 1) % (img.width + initial_x);
    if (x == 0) {
      y++;
      x = initial_x;
    }
  }

  return 0;
}

int (partial_print_xpm)(xpm_image_t img, uint16_t x, uint16_t y, uint16_t xdraw, uint16_t ydraw, uint16_t width, uint16_t height, const uint8_t* loaded_xpm) {
  if (loaded_xpm == NULL) return 1;
  if (xdraw + width > img.width || ydraw + height > img.height) return 1; 

  uint16_t initial_x = xdraw;
  uint16_t num_pixels = width * height;
  uint16_t index = xdraw + ydraw * img.width;
  uint16_t initial_index = index;
  uint16_t initial_y = ydraw;

  for (uint16_t i = 0; i < num_pixels; i++) {

    if (paint_pixel(x + xdraw, y + ydraw, loaded_xpm[i])) return 1;
    xdraw = (xdraw + 1) % (initial_x + width);
    if (xdraw == 0) {
        ydraw++;
        xdraw = initial_x;
        index = initial_index + (img.width * (ydraw - initial_y));
    }
    else index++;
  }

  return 0;
}
