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

void (kbc_ih)() {
    if (read_KBC_output(KBC_OUT_CMD, &scancode)) error = true;
    else error = false;
}

int keyboard_restore()
{
  uint8_t commandWord;

  // Leitura da configuração atual
  if (write_KBC_command(0x64, 0x20) != 0)
    return 1;  // notificar o i8042 da leitura
    
  if (read_KBC_output(0x60, &commandWord) != 0)
    return 1;  // ler a configuração

  // Activar o bit das interrupções
  commandWord = commandWord | BIT(0);

  // Escrita da nova configuração
  if (write_KBC_command(0x64, 0x60) != 0)
    return 1;  // notificar o i8042 da escrita
    
  if (write_KBC_command(0x60, commandWord) != 0)
    return 1;  // escrever a configuração
    
  return 0;
}
