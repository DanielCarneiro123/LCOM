// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

#include "mouse.h"

extern bool error;
extern uint8_t mouse_byte;
extern int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

//SEPARAR MOUSE SYNC
int (mouse_test_packet)(uint32_t cnt) {
  //if (write_to_mouse(ENABLE_STREAM)) return 1;
  if (mouse_enable_data_reporting()) return 1; //FAZER A MINHA PRÓPRIA FUNÇÃO QUE FAZ ISTO
  uint8_t mask;
  if (mouse_subscribe_interrupts(&mask)) return 1;
  
  int ipc_status;
  message msg;
  int r;
  //uint8_t bytes[3];
  uint8_t curr = 0;
  struct packet mouse_packet;
  int16_t delta_x, delta_y;

  while(cnt > 0) { /* You may want to use a different condition */
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & mask) { /* subscribed interrupt */
                  mouse_ih();
                  if ((curr == 0) && !(BIT(3) & mouse_byte)) return 1;
                  mouse_packet.bytes[curr] = mouse_byte;
                  if (curr == 0) {
                    mouse_packet.lb = mouse_byte & BIT(0);
                    mouse_packet.rb = mouse_byte & BIT(1);
                    mouse_packet.mb = mouse_byte & BIT(2);
                    mouse_packet.x_ov = mouse_byte & BIT(6);
                    mouse_packet.y_ov = mouse_byte & BIT(7);
                    if (mouse_byte & BIT(4)) delta_x = 0xFF00;
                    else delta_x = 0;
                    if (mouse_byte & BIT(5)) delta_y = 0xFF00;
                    else delta_y = 0;
                  }
                  else if (curr == 1) {
                    delta_x |= mouse_byte;
                    mouse_packet.delta_x = delta_x;
                  }
                  else if (curr == 2) {
                    delta_y |= mouse_byte;
                    mouse_packet.delta_y = delta_y;
                    mouse_print_packet(&mouse_packet);
                    cnt--;
                  }
                  curr = (curr+1) % 3;
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */	
        }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
 }

  if (mouse_unsubscribe_interrupts()) return 1;
  if (mouse_restore()) return 1; 
  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    //if (write_to_mouse(ENABLE_STREAM)) return 1;
  if (mouse_enable_data_reporting()) return 1; //FAZER A MINHA PRÓPRIA FUNÇÃO QUE FAZ ISTO
  uint8_t mask, mask_timer;
  if (mouse_subscribe_interrupts(&mask)) return 1;
  if (timer_subscribe_int(&mask_timer)) return 1;
  uint32_t hz = sys_hz();
  
  int ipc_status;
  message msg;
  int r;
  uint8_t curr_seconds = 0;
  uint8_t curr = 0;
  uint8_t curr_delay = 0;
  struct packet mouse_packet;
  int16_t delta_x, delta_y;

  while(curr_seconds < idle_time) { /* You may want to use a different condition */
    /* Get a request message. */
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
                if (msg.m_notify.interrupts & mask) { /* subscribed interrupt */
                  mouse_ih();
                  if ((curr == 0) && !(BIT(3) & mouse_byte)) return 1;
                  mouse_packet.bytes[curr] = mouse_byte;
                  if (curr == 0) {
                    mouse_packet.lb = mouse_byte & BIT(0);
                    mouse_packet.rb = mouse_byte & BIT(1);
                    mouse_packet.mb = mouse_byte & BIT(2);
                    mouse_packet.x_ov = mouse_byte & BIT(6);
                    mouse_packet.y_ov = mouse_byte & BIT(7);
                    if (mouse_byte & BIT(4)) delta_x = 0xFF00;
                    else delta_x = 0;
                    if (mouse_byte & BIT(5)) delta_y = 0xFF00;
                    else delta_y = 0;
                  }
                  else if (curr == 1) {
                    delta_x |= mouse_byte;
                    mouse_packet.delta_x = delta_x;
                  }
                  else if (curr == 2) {
                    delta_y |= mouse_byte;
                    mouse_packet.delta_y = delta_y;
                    mouse_print_packet(&mouse_packet);
                  }
                  curr_seconds = 0;
                  curr_delay = counter % hz;
                  curr = (curr+1) % 3;
                }
                if (msg.m_notify.interrupts & mask_timer) {
                  timer_int_handler();
                  if (counter % hz == curr_delay) {
                    curr_seconds++;
                  }
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */	
        }
    } else { /* received a standard message, not a notification */
        /* no standard messages expected: do nothing */
    }
 }

  if (mouse_unsubscribe_interrupts()) return 1;
  if (mouse_restore()) return 1; 
  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
