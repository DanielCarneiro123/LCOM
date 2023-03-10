#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

static int hook_id = 0;
int counter = 0;
int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  
  if (!(timer != 0 || timer != 1 || timer != 2)) return 1;
  if (freq < 19 || freq > TIMER_FREQ) return 1;
  uint16_t counter = TIMER_FREQ/freq;
  uint8_t lsb, msb;
  util_get_LSB(counter, &lsb);
  util_get_MSB(counter, &msb);
  uint8_t config;
  if (timer_get_conf(timer, &config) != 0) return 1; // ler a config do timer
  config = config & 0x0f;
  config = config | TIMER_LSB_MSB;
  switch (timer)
  {
    case 0: 
      config = config | TIMER_SEL0;
      break;
    case 1:
      config = config | TIMER_SEL1;
      break;
    case 2:
      config = config | TIMER_SEL2;
      break;
  }
  sys_outb(0x43, config);
  sys_outb(0x40 + timer, lsb);
  sys_outb(0x40 + timer, msb);
  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = hook_id;
  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id)) return 1;
  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id)){
    return 1;
  }
  return 0;
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {  //a configuracao ficara guardada na variavel st como uma control word
  if (!(timer != 0 || timer != 1 || timer != 2)) return 1;
  if (st == NULL){
    return 1;
  }
  uint8_t command = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  sys_outb(TIMER_CTRL, command);
  if (timer == 0){
    util_sys_inb(TIMER_0, st);
  }

  if (timer == 1){
    util_sys_inb(TIMER_1, st);
  }

  if (timer == 2){
    util_sys_inb(TIMER_2, st);
  }
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
              
  
  union timer_status_field_val val; // estrutura semelhante a uma struct
  if (!(timer != 0 || timer != 1 || timer != 2)) return 1;
  
  switch (field) 
  {
    case tsf_all:
      val.byte = st;
      break;
    case tsf_initial:
      if (st & TIMER_LSB_MSB) val.in_mode = MSB_after_LSB;
      else if (st & TIMER_LSB) val.in_mode = LSB_only; //ponto 7 html dado
      else if (st & TIMER_MSB) val.in_mode = MSB_only;
      else val.in_mode = INVAL_val; 
      break;
    case tsf_mode:
      val.count_mode = (st >> 1) & 7;
      break;
    case tsf_base:
      val.bcd = st & 1;
      break;   
  };
  timer_print_config(timer, field, val);
  return 0;
}
