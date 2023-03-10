#include "keyboard.h"
#include "i8042.h"

int hook_id = 1;
uint8_t scancode = 0;
bool error = false;

int keyboard_subscribe_interrupts(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = BIT(hook_id);
  if (sys_irqsetpolicy(IRQ_KEYBOARD, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id)) return 1;
  return 0;
}

int keyboard_unsubscribe_interrupts() {
  if (sys_irqrmpolicy(&hook_id)) return 1;
  return 0;
}

void kbc_ih() {
    if (read_KBC_output(KBC_OUT_CMD, &scancode)) error = true;
    else error = false;
}
