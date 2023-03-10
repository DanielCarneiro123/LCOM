#include "KBC.h"

int read_kbc_status(uint8_t* status) {
    return util_sys_inb(KBC_STATUS_REG, status);
}

int read_kbc_output(uint8_t port, uint8_t *output) {
    int attempts = 10;
    uint8_t status;
    while (attempts) {
        if (read_kbc_status(&status)) return 1;
        
        if ((status & FULL_OUT_BUFFER)) {
            if (util_sys_inb(port, output)) return 1;
            if (status & PARITY_ERROR) return 1;
            if (status & TIMEOUT_ERROR) return 1;
            return 0;
        }
        tickdelay(micros_to_ticks(20000));
        attempts--;
    }
    return 1;
}

int write_KBC_command(uint8_t port, uint8_t commandByte) {
    int attempts = 10;
    uint8_t status;
    while (attempts) {
        if (read_kbc_status(&status)) return 1;
        if (!(status & FULL_IN_BUFFER)) {
            return util_sys_outb(port, commandByte);
        }
        tickdelay(micros_to_ticks(20000));
        attempts--;
    }
    return 1;
}