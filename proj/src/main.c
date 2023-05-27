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
#include "config.h"

extern SystemState systemState;

int (main)(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/g2/proj/src/debug/trace.txt");
  lcf_log_output(strcat(strcat("/home/lcom/labs/g2/proj/src/debug/output", argv[1]), ".txt"));
  if (lcf_start(argc, argv)) return 1;
  lcf_cleanup();
  return 0;
}

int setup() {


  if (timer_set_frequency(TIMER, GAME_FREQUENCY) != 0) return 1;
  if (set_frame_buffers(VIDEO_MODE) != 0) return 1;
  if (set_graphic_mode(VIDEO_MODE) != 0) return 1;

  setup_sprites();
  setup_positions();
  setup_small_positions();
  setup_box_balls_positions();
  setup_box_small_balls_positions();

  uint8_t timer_byte, keyboard_byte, mouse_byte, sp_byte;

  if (timer_subscribe_ints(&timer_byte) != 0) return 1;
  if (keyboard_subscribe_interrupts(&keyboard_byte) != 0) return 1;
  if (mouse_subscribe_interrupts(&mouse_byte) != 0) return 1;
  if (sp_subscribe_interrupts(&sp_byte) != 0) return 1;

  if (mouse_write(ENABLE_STREAM) != 0) return 1;
  if (mouse_write(ENABLE_REMOTE) != 0) return 1;

  rtc_setup();
  if (sp_setup()) return 1;

  return 0;
}

int teardown() {

  if (vg_exit() != 0) return 1;

  destroy_sprites();
  destroy_positions();
  destroy_small_positions();
  destroy_code_positions();

  if (timer_unsubscribe_ints() != 0) return 1;
  if (keyboard_unsubscribe_interrupts() != 0) return 1;
  if (mouse_unsubscribe_interrupts() != 0) return 1;
  if (sp_unsubscribe_interrupts() != 0) return 1;

  if (mouse_write(DISABLE_STREAM) != 0) return 1;

  return 0;
}

int (proj_main_loop)(int argc, char *argv[]) {

  if (setup() != 0) return teardown();

  draw_new_frame();

  int ipc_status;
  message msg;
  while (systemState == RUNNING) {
    
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("Error");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & TIMER_MASK)    update_timer_state();
          if (msg.m_notify.interrupts & KEYBOARD_MASK) update_keyboard_state();
          if (msg.m_notify.interrupts & SP_MASK)        update_sp_state();
          if (msg.m_notify.interrupts & MOUSE_MASK)    update_mouse_state();
        }
    }
  }
  
  if (teardown() != 0) return 1;

  return 0;
}
