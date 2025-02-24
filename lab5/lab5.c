// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "keyboard.h"
#include "i8042.h"
#include "graphics.h"
extern bool error;
extern uint8_t scancode;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (set_graphic_mode(mode)) return 1;

  sleep(delay);

  if (vg_exit()) return 1;

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  if (set_frame_buffer(mode)) return 1;
  if (set_graphic_mode(mode)) return 1;
  //if (draw_line(x, y, x + width, y, color)) return 1;
  //if (draw_line(x + width, y, x + width, y + height, color)) return 1;
  //if (draw_line(x + width, y + height, x, y + height, color)) return 1;
  //if (draw_line(x, y + height, x, y, color)) return 1;
  for (uint16_t i = x; i < x + width; i++) {
    for (uint16_t j = y; j < y + height; j++) {
      if (paint_pixel(i, j, color)) return 1;
    }
  }

  int ipc_status;
  uint8_t irq_set;
  message msg;

  if (keyboard_subscribe_interrupts(&irq_set) != 0)
    return 1;

  while(scancode != BREAK_ESC) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("Error");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)){
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();
            if (error) return 1;
          }
      }
    }
  }  

  if (keyboard_unsubscribe_interrupts())
    return 1;

  if (vg_exit()) return 1;

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if (set_frame_buffer(mode)) return 1;
  if (set_graphic_mode(mode)) return 1;
  
  vbe_mode_info_t mode_info;
  if (get_mode_info(mode, &mode_info)) return 1;

  uint16_t x = 0, y = 0;
  uint8_t row = 0, col = 0;
  uint16_t width = mode_info.XResolution / no_rectangles;
  uint16_t height = mode_info.YResolution / no_rectangles;

  for (uint8_t i = 0; i < no_rectangles; i++) {
    uint32_t color = 0;
    if (mode == VBE_768p_INDEXED) {
      color = (first + (row * no_rectangles + col) * step) % (1 << mode_info.BitsPerPixel);
    }
    else {
      color |= ((((1 << mode_info.RedMaskSize) - 1) & (first >> mode_info.RedFieldPosition)) + col * step) % (1 << mode_info.RedMaskSize);
      color |= ((((1 << mode_info.GreenMaskSize) - 1) & (first >> mode_info.GreenFieldPosition)) + col * step) % (1 << mode_info.GreenMaskSize);
      color |= ((((1 << mode_info.BlueMaskSize) - 1) & (first >> mode_info.BlueFieldPosition)) + col * step) % (1 << mode_info.BlueMaskSize);
    }
    fill_rectangle(x, y, width, height, color);
    x += width;
    if (x + width > mode_info.XResolution) {
      x = 0;
      y += height;
      row++;
      col++;
    }
  }
  
  int ipc_status;
  uint8_t irq_set;
  message msg;

  if (keyboard_subscribe_interrupts(&irq_set) != 0)
    return 1;

  while(scancode != BREAK_ESC) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("Error");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)){
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();
            if (error) return 1;
          }
      }
    }
  }  

  if (keyboard_unsubscribe_interrupts())
    return 1;

  if (vg_exit()) return 1;

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if (set_frame_buffer(VBE_768p_INDEXED)) return 1;
  if (set_graphic_mode(VBE_768p_INDEXED)) return 1;

  xpm_image_t img;
  uint8_t *loaded_xpm = xpm_load(xpm, XPM_INDEXED, &img);
  if (loaded_xpm == NULL) return 1;
  
  //print_xpm(img, x, y, loaded_xpm);
  partial_print_xpm(img, x, y, img.width/2, img.height/2, img.width/2, img.height/2, loaded_xpm);

  int ipc_status;
  uint8_t irq_set;
  message msg;

  if (keyboard_subscribe_interrupts(&irq_set) != 0)
    return 1;

  while(scancode != BREAK_ESC) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("Error");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)){
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();
            if (error) return 1;
          }
      }
    }
  }  

  if (keyboard_unsubscribe_interrupts())
    return 1;

  if (vg_exit()) return 1;

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
