#include "mouse.h"

// Variáveis globais do módulo
int hook_id_mouse = 2;        // um valor qualquer [0..7], desde que seja diferente do teclado e do timer
uint8_t byte_index = 0;       // [0..2]
uint8_t mouse_byte;         // o byte mais recente lido
MouseInfo mouse_info = {0, 0, 0, 100, 100};
MouseInfo mouse_buffer;
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
void mouse_update_buffer() {
  if (byte_index == 0 && (mouse_byte & FIRST_BYTE)) { // é o byte CONTROL, o bit 3 está ativo
    mouse_buffer.right_click = mouse_byte & MOUSE_RB;
    mouse_buffer.left_click = mouse_byte & MOUSE_LB;
    if (mouse_byte & MOUSE_X_SIGNAL) mouse_buffer.x = 0xFF00;
    else mouse_buffer.x = 0;
    if (mouse_byte & MOUSE_Y_SIGNAL) mouse_buffer.y = 0xFF00;
    else mouse_buffer.y = 0;
    byte_index++;
  }
  else if (byte_index == 1) {                            // recebe os deslocamentos em X e Y
    mouse_buffer.x |= mouse_byte;
    byte_index++;
  }
  else if (byte_index == 2) {
    mouse_buffer.y |= mouse_byte;
    byte_index++;
  }  
}

// Transforma o array de bytes numa struct definida de acordo com as necessidades da aplicação
void (mouse_sync_info)(){
  int16_t new_x = mouse_info.x + mouse_buffer.x;
  int16_t new_y = mouse_info.y - mouse_buffer.y;

  mouse_info.right_click = mouse_buffer.right_click;
  mouse_info.left_click = mouse_buffer.left_click;

  if (new_x < 0 || new_x > mode_info.XResolution || new_y < 0 || new_y > mode_info.YResolution) return;
  mouse_info.x = new_x;
  mouse_info.y = new_y;
}

// A escrita para o rato tem de ser feita de forma mais controlada do que no keyboard
// Temos de injetar o comando DIRETAMENTE no rato e esperar por uma resposta afirmativa (ACK).
int (mouse_write)(uint8_t command) {
  uint8_t attempts = 10;
  uint8_t mouse_response;

  // Enquanto houver tentativas e a resposta não for satisfatória
  do {
    attempts--;
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
  } while (mouse_response != 0xFA && attempts);       

  return 0;
}
