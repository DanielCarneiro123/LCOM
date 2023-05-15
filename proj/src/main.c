#include <lcom/lcf.h>
#include "controller/timer/timer.h"
#include "controller/video/graphics.h"
#include "controller/keyboard/keyboard.h"
#include "controller/mouse/mouse.h"
#include "controller/rtc/rtc.h"
#include "controller/serialport/sp.h"
#include "model/model.h"
#include "view/view.h"
#include "config.h"

extern SystemState systemState;

int (main)(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/g2/proj/src/debug/trace.txt");
  lcf_log_output("/home/lcom/labs/g2/proj/src/debug/output.txt");
  if (lcf_start(argc, argv)) return 1;
  lcf_cleanup();
  return 0;
}

int setup() {

  // Atualização da frequência
  if (timer_set_frequency(TIMER, GAME_FREQUENCY) != 0) return 1;

  // Inicialização dos buffers de vídeo (double buffering)
  if (set_frame_buffers(VIDEO_MODE) != 0) return 1;

  // Inicialização do modo gráfico
  if (set_graphic_mode(VIDEO_MODE) != 0) return 1;

  // Inicialização dos sprites
  setup_sprites();
  setup_positions();

  uint8_t timer_byte, keyboard_byte, mouse_byte, sp_byte;

  // Ativação das interrupções dos dispositivos
  if (timer_subscribe_ints(&timer_byte) != 0) return 1;
  if (keyboard_subscribe_interrupts(&keyboard_byte) != 0) return 1;
  if (mouse_subscribe_interrupts(&mouse_byte) != 0) return 1;
  if (rtc_subscribe_interrupts() != 0) return 1;
  if (sp_subscribe_interrupts(&sp_byte) != 0) return 1;

  // Ativar stream-mode e report de dados do rato
  if (mouse_write(ENABLE_STREAM) != 0) return 1;
  if (mouse_write(ENABLE_REMOTE) != 0) return 1;

  // Setup do Real Time Clock
  rtc_setup();
  if (sp_setup()) return 1;

  return 0;
}

int teardown() {

  // Volta ao modo de texto
  if (vg_exit() != 0) return 1;

  // Destruição dos sprites
  destroy_sprites();
  destroy_positions();

  // Desativa todas as interrupções
  if (timer_unsubscribe_ints() != 0) return 1;
  if (keyboard_unsubscribe_interrupts() != 0) return 1;
  if (mouse_unsubscribe_interrupts() != 0) return 1;
  if (rtc_unsubscribe_interrupts() != 0) return 1;
  if (sp_unsubscribe_interrupts() != 0) return 1;

  // Desativar o report de dados do rato
  if (mouse_write(DISABLE_STREAM) != 0) return 1;

  return 0;
}

int (proj_main_loop)(int argc, char *argv[]) {

  // Setup do Minix
  if (setup() != 0) return teardown();

  // Desenha a primeira frame
  draw_new_frame();

  // Tratamento das interrupções
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
          if (msg.m_notify.interrupts & BIT(6))        update_sp_state();
          if (msg.m_notify.interrupts & MOUSE_MASK)    update_mouse_state();
          if (msg.m_notify.interrupts & RTC_MASK)      update_rtc_state();
        }
    }
  }
  
  // Tear-down do Minix
  if (teardown() != 0) return 1;

  return 0;
}
