#include "keyboard.h"
#include "i8042.h"

int hook_id_kbd = 1;
uint8_t scancode = 0;
bool error = false;

/**
 * @brief Subscribes to the keyboard's interrupts in exclusive mode
 * Subscribes to the keyboard's interrupts with the bit mask defined in hook_id_kbd
 * @param bit_no Bit mask for which the subscription takes place
 * @return int 1 on failure, 0 otherwise
 */
int keyboard_subscribe_interrupts(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = BIT(hook_id_kbd);
  if (sys_irqsetpolicy(IRQ_KEYBOARD, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_kbd)) return 1;
  return 0;
}

/**
 * @brief Unsubscribes from the keyboard's interrupts
 * @return int 1 on failure, 0 otherwise
 */
int keyboard_unsubscribe_interrupts() {
  if (sys_irqrmpolicy(&hook_id_kbd)) return 1;
  return 0;
}

/**
 * @brief Interrupt handler for the keyboard
 * Reads a scancode from the KBC
 */
void (kbc_ih)() {
    if (read_KBC_output(KBC_OUT_CMD, &scancode, 0)) error = true;
    else error = false;
}

void (kbc_ih_poll)() {
    if (read_KBC_output_poll(KBC_OUT_CMD, &scancode)) error = true;
    else error = false;
}
