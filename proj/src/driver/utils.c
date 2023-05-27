#include <lcom/lcf.h>
#include <stdint.h>

/**
 * @brief Gets the LSB from a byte
 * @param val Value to extract LSB from
 * @param lsb Resulting LSB
 * @return int 1 on failure, 0 otherwise
 */
int (util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) return 1;
  *lsb = val & 0xFF;
  return 0;
}

/**
 * @brief Gets the MSB from a byte
 * @param val Value to extract MSB from
 * @param msb Resulting MSB
 * @return int 1 on failure, 0 otherwise
 */
int (util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) return 1;
  *msb = val >> 8;
  return 0;
}

/**
 * @brief Wrapper to sys_inb
 * Allows sys_inb to be used with a uint8_t value instead of uint32_t
 * @param port Port to read
 * @param value Byte in which to store value
 * @return int 1 on failure, 0 otherwise
 */
int util_sys_inb(int port, uint8_t *value) {
  if (value == NULL) return 1;
  uint32_t big_value;
  if (sys_inb(port, &big_value)) return 1;
  *value = big_value & 0xFF;
  return 0;
}
