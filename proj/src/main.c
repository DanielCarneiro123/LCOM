#include <lcom/lcf.h>
#include "driver/timer/timer.h"
#include "driver/video/graphics.h"
#include "driver/keyboard/keyboard.h"
#include "driver/mouse/mouse.h"
#include "driver/rtc/rtc.h"
#include "driver/serialport/sp.h"
#include "model/model.h"
#include "model/data_structures.h"
#include "view/view.h"
#include "model/game_constants.h"

extern uint8_t running;

int (main)(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/g2/proj/src/debug/trace.txt");
  lcf_log_output(strcat(strcat("/home/lcom/labs/g2/proj/src/debug/output", argv[1]), ".txt"));
  if (lcf_start(argc, argv)) return 1;
  lcf_cleanup();
  return 0;
}

/**
 * @brief Sets up the devices and objects \n 
 * Allocates memory for objects, subscribes to interrupts and sets graphic mode
 * @return 1 on failure, 0 otherwise
 */
int setup() {
  if (timer_set_frequency(TIMER, GAME_FREQUENCY) != 0) return 1;
  if (set_frame_buffers(0x115) != 0) return 1;
  if (set_graphic_mode(0x115) != 0) return 1;

  if (setup_sprites()) return 1;
  if (setup_positions()) return 1;
  if (setup_small_positions()) return 1;
  if (setup_box_balls_positions()) return 1;
  if (setup_box_small_balls_positions()) return 1;

  uint8_t timer_byte, keyboard_byte, mouse_byte, sp_byte;

  if (timer_subscribe_ints(&timer_byte) != 0) return 1;
  if (keyboard_subscribe_interrupts(&keyboard_byte) != 0) return 1;
  if (mouse_subscribe_interrupts(&mouse_byte) != 0) return 1;
  if (sp_subscribe_interrupts(&sp_byte) != 0) return 1;

  if (mouse_write(ENABLE_STREAM) != 0) return 1;
  if (mouse_write(ENABLE_REMOTE) != 0) return 1;

  if (rtc_setup()) return 1;
  if (sp_setup()) return 1;

  return 0;
}

/**
 * @brief Tears down the program \n 
 * Deallocates heap memory, unsubscribes from interrupts, returns to text mode
 * @return 1 on failure, 0 otherwise
 */
int teardown() {

  if (vg_exit() != 0) return 1;

  destroy_sprites();
  destroy_positions();
  destroy_small_positions();
  destroy_code_positions();

  if (timer_unsubscribe_ints()) return 1;
  if (keyboard_unsubscribe_interrupts()) return 1;
  if (mouse_unsubscribe_interrupts()) return 1;
  if (sp_unsubscribe_interrupts()) return 1;

  if (mouse_write(DISABLE_STREAM)) return 1;

  return 0;
}

/**
 * @brief Main loop \n 
 * Main loop of the game
 * @param argc Command line argument count
 * @param argv Command line arguments
 * @return 1 on failure, 0 otherwise
 */
int (proj_main_loop)(int argc, char *argv[]) {

  if (setup()) return teardown();

  draw_new_frame();

  int ipc_status;
  message msg;
  while (running) {
    
    if (driver_receive(ANY, &msg, &ipc_status)) {
      printf("Driver receive error");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & TIMER_MASK) update_timer_state();
          if (msg.m_notify.interrupts & KEYBOARD_MASK) update_keyboard_state();
          if (msg.m_notify.interrupts & SP_MASK) update_sp_state();
          if (msg.m_notify.interrupts & MOUSE_MASK) update_mouse_state();
        }
    }
  }
  
  if (teardown()) return 1;

  return 0;
}
