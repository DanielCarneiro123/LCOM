#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int hook_id_timer = 0;
int counter = 0;

/**
 * @brief Sets the timer's frequency \n 
 * Sets the timer's frequency to a given value passed to the function
 * @param timer Timer to update (0, 1, 2)
 * @param freq New frequency
 * @return 1 on failure, 0 otherwise
 */
int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  if (timer != 0 && timer != 1 && timer != 2) return 1;
  if (freq < 19 || freq > TIMER_FREQ) return 1;
  uint16_t counter = TIMER_FREQ/freq;
  uint8_t lsb, msb;
  util_get_MSB(counter, &msb);
  util_get_LSB(counter, &lsb);
  uint8_t st;
  if (timer_get_conf(timer, &st)) return 1;
  st &= 0xF;
  st = st | BIT(4) | BIT(5);
  switch (timer) {
    case 1:
      st |= BIT(6);
      break;
    case 2:
      st |= BIT(7);
      break;
  }
  sys_outb(TIMER_CTRL, st);
  sys_outb(0x40 + timer, lsb);
  sys_outb(0x40 + timer, msb);
  return 0;
}

/**
 * @brief Subscribes to timer 0's interrupts \n 
 * Subscribes to the timer's interrupts with the bit mask defined in hook_id_timer
 * @param bit_no Bit mask for which the subscription takes place
 * @return 1 on failure, 0 otherwise
 */
int (timer_subscribe_ints)(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = BIT(hook_id_timer);
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id_timer)) return 1;
  return 0;
}

/**
 * @brief Ubsubscribes from the timer's interrupts
 * @return 1 on failure, 0 otherwise
 */
int (timer_unsubscribe_ints)() {
  if (sys_irqrmpolicy(&hook_id_timer)) return 1;
  return 0;
}

/*void (timer_int_handler)() {
  counter++;
}*/

/**
 * @brief Reads the configuration from a timer \n 
 * Reads the configuration from a timer and returns it via a pointer parameter
 * @param timer Timer to read (0, 1, 2)
 * @param st Configuration byte 
 * @return 1 on failure, 0 otherwise
 */
int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  if (timer > 2) return 1;
  uint8_t command = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  sys_outb(TIMER_CTRL, command);
  if (util_sys_inb(0x40 + timer, st)) return 1;
  return 0;
}

/**
 * @brief Displays a timer's configuration in the terminal \n 
 * Displays the configuration using the timer_status_field_val union type and the timer_print_config function
 * @param timer Timer to display (0, 1, 2)
 * @param st Configuration
 * @param field Field to display
 * @return 1 on failure, 0 otherwise
 */
int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  union timer_status_field_val conf;
  switch(field) {
    case tsf_all:
      conf.byte = st;
      break;
    case tsf_initial:
      st &= 0x30;
      st >>= 4;
      conf.in_mode = st;
      break;
    case tsf_mode:
      st &= 0x0E;
      st >>= 1;
      if (st == 6) conf.count_mode = 2;
      else if (st == 7) conf.count_mode = 3;
      else conf.count_mode = st;
      break;
    case tsf_base:
      st &= 0x01;
      conf.bcd = (st == 1) ? true : false;
      break; 
  }
  timer_print_config(timer, field, conf);
  return 0;
}
