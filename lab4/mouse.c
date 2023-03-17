#include "mouse.h"

int hook_id_mouse = 2;
uint8_t mouse_byte;
bool error = false;

int mouse_subscribe_interrupts(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = BIT(hook_id_mouse);
  if (sys_irqsetpolicy(IRQ_MOUSE, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse)) return 1;
  return 0;
}

int mouse_unsubscribe_interrupts() {
  if (sys_irqrmpolicy(&hook_id_mouse)) return 1;
  return 0;
}

void (mouse_ih)() {
  if (read_KBC_output(KBC_OUT_CMD, &mouse_byte, 1)) error = true;
  else error = false;
}

int write_to_mouse(uint8_t command)
{
  uint8_t attemps = 10;
  uint8_t mouse_response;

  // Enquanto houver tentativas e a resposta não for satisfatória
  do {
    attemps--;
    // Ativar do modo D4 do i8042
    if (write_KBC_command(0x64, 0xD4))
      return 1;
    // O comando para o rato é escrito na porta 0x60
    if (write_KBC_command(0x60, command))
      return 1;
    tickdelay(micros_to_ticks(20000));
    // Ler a resposta do rato pela porta 0x60
    if (read_KBC_output(0x60, &mouse_response, 1))
      return 1;
  } while (mouse_response != 0xFA && attemps);       

  return 0;
}

int mouse_restore() {
    if (write_to_mouse(DISABLE_STREAM)) return 1;
    return 0;
}
