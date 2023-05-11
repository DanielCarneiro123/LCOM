#include "sp.h"

int hook_id_sp = 6;

int sp_subscribe_interrupts(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = BIT(hook_id_sp);
  if (sys_irqsetpolicy(COM2_IRQ, IRQ_REENABLE, &hook_id_sp)) return 1;
  return 0;
}

int sp_unsubscribe_interrupts() {
  if (sys_irqrmpolicy(&hook_id_sp)) return 1;
  return 0;
}
