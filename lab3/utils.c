#include <lcom/lcf.h>
#include <stdint.h>

uint32_t cnt = 0;
int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t i;
  if (sys_inb(port, &i) !=0){
    return 1;
  }
  #ifdef LAB3
  cnt++;
  *value = i & 0xff;
  #endif  
  return 0;
}

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) return 1;
  *lsb = val & 0x00ff;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) return 1;
  *msb = val >> 8;
  return 0;
}


