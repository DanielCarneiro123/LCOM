#include "KBC.h"
#include "i8042.h"

/**
 * @brief Reads the KBC's status
 * Reads the KBC's status and places it in a byte passed by pointer
 * @param status Address of byte in which to store status
 * @return int 1 on failure, 0 otherwise
 */
int read_KBC_status(uint8_t* status) {
    if (status == NULL) return 1;
    return util_sys_inb(KBC_STATUS_REG, status);
}

/**
 * @brief Reads the KBC's output
 * Reads the KBC's output, either from the mouse or from the keyboard
 * @param port Port to read from
 * @param output Address of byte in which to store output
 * @param mouse 1 if reading mouse, 0 if reading keyboard
 * @return int 1 on failure, 0 otherwise
 */
int read_KBC_output(uint8_t port, uint8_t *output, uint8_t mouse) {
    int attempts = MAX_ATTEMPTS;
    uint8_t status;
    while (attempts) {
        if (read_KBC_status(&status)) return 1;

        if ((status & FULL_OUT_BUFFER)) {
            if (util_sys_inb(port, output)) return 1;
            if (status & PARITY_ERROR) return 1;
            if (status & TIMEOUT_ERROR) return 1;
            if (mouse && !(status & BIT(5))) {
                printf("Error: Mouse output not found\n");
                return 1;
            } 
            if (!mouse && (status & BIT(5))) {
                printf("Error: Keyboard output not found\n");
                return 1;
            }   
            return 0;
        }
        tickdelay(micros_to_ticks(WAIT_KBC));
        attempts--;
    }
    return 1;
}

int read_KBC_output_poll(uint8_t port, uint8_t *output) {
    int attempts = MAX_ATTEMPTS;
    uint8_t status;
    while (attempts) {
        if (read_KBC_status(&status)) return 1;
        
        if ((status & FULL_OUT_BUFFER)) {
            if (util_sys_inb(port, output)) return 1;
            if (status & PARITY_ERROR) return 1;
            if (status & TIMEOUT_ERROR) return 1;
            return 0;
        }
        //tickdelay(micros_to_ticks(WAIT_KBC));
        attempts--;
    }
    return 1;
}

/**
 * @brief Writes a command to the KBC
 * Writes a command to the KBC in a given port
 * @param port Port to write in
 * @param commandByte Command to write
 * @return int 1 on failure, 0 otherwise
 */
int write_KBC_command(uint8_t port, uint8_t commandByte) {
    int attempts = MAX_ATTEMPTS;
    uint8_t status;
    while (attempts) {
        if (read_KBC_status(&status)) return 1;
        if (!(status & FULL_IN_BUFFER)) {
            return sys_outb(port, commandByte);
        }
        tickdelay(micros_to_ticks(WAIT_KBC));
        attempts--;
    }
    return 1;
}
