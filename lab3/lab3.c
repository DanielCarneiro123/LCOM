#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include "i8042.h"
#include "keyboard.h"

extern uint32_t cnt;
extern bool error;
extern uint8_t scancode;
extern int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int(kbd_test_scan)() {
  int ipc_status;
  uint8_t irq_set;
  message msg;
  bool two_bytes = false;

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
            bool make = !(scancode & MAKE_CODE);
            if (scancode == TWO_BYTES) two_bytes = true;
            else if (two_bytes) {
                two_bytes = false;
                uint8_t bytes[2];
                bytes[0] = TWO_BYTES;
                bytes[1] = scancode;
                if (kbd_print_scancode(make, 2, bytes)) return 1;
            }
            else {
                uint8_t bytes[1];
                bytes[0] = scancode;
                if (kbd_print_scancode(make, 1, bytes)) return 1;
            }
          }
        }
      }
    }

  if (keyboard_unsubscribe_interrupts() != 0)
    return 1;
  if (kbd_print_no_sysinb(cnt) != 0)
    return 1;

  return 0;
}

int(kbd_test_poll)() {

  bool two_bytes = false;
  while (scancode != BREAK_ESC) {
    kbc_ih_poll();
    if (error) return 1;
    bool make = !(scancode & MAKE_CODE);
    if (scancode == TWO_BYTES) two_bytes = true;
    else if (two_bytes) {
         two_bytes = false;
        uint8_t bytes[2];
        bytes[0] = TWO_BYTES;
        bytes[1] = scancode;
        if (kbd_print_scancode(make, 2, bytes)) return 1;
    }
    else {
        uint8_t bytes[1];
         bytes[0] = scancode;
        if (kbd_print_scancode(make, 1, bytes)) return 1;
    }
  } 
  if (keyboard_restore()) return 1;
  if (kbd_print_no_sysinb(cnt) != 0)
    return 1;
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t idle = n;
  uint8_t curr_delay = 0;
  int ipc_status;
  uint8_t irq_set;
  uint8_t irq_timer;
  message msg;
  bool two_bytes = false;

  if (timer_subscribe_int(&irq_timer))
    return 1;
  if (keyboard_subscribe_interrupts(&irq_set) != 0)
    return 1;

  

  while((scancode != BREAK_ESC) && (idle != 0)) {
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
            bool make = !(scancode & MAKE_CODE);
            if (scancode == TWO_BYTES) two_bytes = true;
            else if (two_bytes) {
                two_bytes = false;
                uint8_t bytes[2];
                bytes[0] = TWO_BYTES;
                bytes[1] = scancode;
                if (kbd_print_scancode(make, 2, bytes)) return 1;
            }
            else {
                uint8_t bytes[1];
                bytes[0] = scancode;
                if (kbd_print_scancode(make, 1, bytes)) return 1;
            }
            idle = n;
            curr_delay = counter % 60;
          }
          if (msg.m_notify.interrupts & irq_timer) {
            timer_int_handler();
            if (counter % 60 == curr_delay) {
              idle--;
            }
          }
        }
      }
    }

  if (keyboard_unsubscribe_interrupts() != 0)
    return 1;
  if (timer_unsubscribe_int())
    return 1;

  return 0;
}
