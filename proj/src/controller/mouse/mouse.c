#include "mouse.h"

// Variáveis globais do módulo
int hook_id_mouse = 2;        // um valor qualquer [0..7], desde que seja diferente do teclado e do timer
uint8_t byte_index = 0;       // [0..2]
uint8_t mouse_bytes[3];       // bytes apanhados
uint8_t mouse_byte;         // o byte mais recente lido
MouseInfo mouse_info = {0, 0, 0, 100, 100};
extern vbe_mode_info_t mode_info;
bool mouse_error = false;

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
  if (read_KBC_output(KBC_OUT_CMD, &mouse_byte, 1)) mouse_error = true;
  else mouse_error = false;
}

// Avalia a disposição dos bytes no array @mouse_bytes
// O primeiro byte do pacote é sempre o que tem o BIT(3) ativo
void mouse_sync_bytes() {
  if (byte_index == 0 && (mouse_byte & FIRST_BYTE)) { // é o byte CONTROL, o bit 3 está ativo
    mouse_bytes[byte_index]= mouse_byte;
    byte_index++;
  }
  else if (byte_index > 0) {                            // recebe os deslocamentos em X e Y
    mouse_bytes[byte_index] = mouse_byte;
    byte_index++;
  }
}

// Transforma o array de bytes numa struct definida de acordo com as necessidades da aplicação
void (mouse_sync_info)(){

  mouse_info.right_click = mouse_bytes[0] & MOUSE_RB;
  mouse_info.left_click = mouse_bytes[0] & MOUSE_LB;

  if (mouse_bytes[0] & MOUSE_X_OVERFLOW || mouse_bytes[0] & MOUSE_Y_OVERFLOW) return;

  int16_t delta_x = mouse_info.x + ((mouse_bytes[0] & MOUSE_X_SIGNAL) ? (0xFF00 | mouse_bytes[1]) : mouse_bytes[1]);
  int16_t delta_y = mouse_info.y - ((mouse_bytes[0] & MOUSE_Y_SIGNAL) ? (0xFF00 | mouse_bytes[2]) : mouse_bytes[2]);

  if (delta_x < 0 || delta_x > mode_info.XResolution || delta_y < 0 || delta_y > mode_info.YResolution) return;
  mouse_info.x = delta_x;
  mouse_info.y = delta_y;
}

// A escrita para o rato tem de ser feita de forma mais controlada do que no keyboard
// Temos de injetar o comando DIRETAMENTE no rato e esperar por uma resposta afirmativa (ACK).
int (mouse_write)(uint8_t command) {
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
