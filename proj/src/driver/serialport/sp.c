#include "sp.h"

int hook_id_sp = 6;
uint8_t sp_data = 0;
bool ready = true;
bool new_data = false;
extern uint32_t color_table[];

/**
 * @brief Sets up the serial port for use
 * Sets the bit rate, the interrupts to be used, disables the fifo, and sets the char sizes
 * @return int 1 on failure, 0 otherwise
 */
int (sp_setup)() {
    if (sys_outb(COM2_BASE + LINE_CONTROL_OFFSET, DIVISOR_LATCH_BIT)) return 1;
    if (sys_outb(COM2_BASE + 0, 0x0C)) return 1;
    if (sys_outb(COM2_BASE + 1, 0x00)) return 1;

    if (sys_outb(COM2_BASE + LINE_CONTROL_OFFSET, EIGHT_BIT_CHAR | BIT(3))) return 1;
    if (sys_outb(COM2_BASE + FIFO_CONTROL_OFFSET, FIFO_DISABLED)) return 1;
    if (sys_outb(COM2_BASE + INTERRUPT_ENABLE_OFFSET, IER_DATA_AVAILABLE | IER_THR_EMPTY)) return 1;
    return 0;
}

/**
 * @brief Subscribes to the serial port's interrupts
 * Subscribes to the serial port's interrupts in exclusive mode with the bit mask defined in hook_id_sp
 * @param bit_no Bit mask for which the subscription takes place
 * @return int 1 on failure, 0 otherwise
 */
int (sp_subscribe_interrupts)(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = BIT(hook_id_sp);
  if (sys_irqsetpolicy(COM2_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_sp)) return 1;
  return 0;
}

/**
 * @brief Unsubscribes from the serial port's interrupts
 * @return int 1 on failure, 0 otherwise
 */
int (sp_unsubscribe_interrupts)() {
  if (sys_irqrmpolicy(&hook_id_sp)) return 1;
  return 0;
}

/**
 * @brief Reads the LSR
 * Reads the serial port's LSR to determine its current status
 * @param lsr Address of byte in which to put LSR
 * @return int 1 on failure, 0 otherwise
 */
int (read_lsr)(uint8_t *lsr) {
    if (lsr == NULL) return 1;
    return util_sys_inb(COM2_BASE + LINE_STATUS_OFFSET, lsr);
}

/**
 * @brief Reads data from the SP
 * Reads data from the SP. If everything went well, an ACK byte is sent, otherwise a NACK byte is sent
 * @return int 1 on failure, 0 otherwise
 */
int (read_sp_data)() {
    int attempts = SP_ATTEMPTS;
    uint8_t status;
    while (attempts) {
        if (read_lsr(&status)) return 1;

        if (status & RECIEVER_DATA_AVAILABLE) {
            if (util_sys_inb(COM2_BASE + RECEIVER_BUFFER_OFFSET, &sp_data)) return 1;
            printf("\n\n\nBYTE READ: %d\n\n\n", sp_data);

            if (sp_data != SP_NACK && sp_data != SP_ACK && (status & SP_OVERRUN_ERROR || status & SP_PARITY_ERROR || status & SP_FRAMING_ERROR || status & BIT(4))) write_sp_data(SP_NACK);
            else if (sp_data != SP_NACK && sp_data != SP_ACK) write_sp_data(SP_ACK);
            return 0;
        }

        tickdelay(micros_to_ticks(SP_WAIT));
        attempts--;
    }

    return 0;
}

/**
 * @brief Writes a byte to the SP
 * Writes a byte to the SP if the THR is empty
 * @param data Data to sent
 * @return int 1 on failure, 0 otherwise
 */
int (write_sp_data)(uint8_t data) {
    int attempts = SP_ATTEMPTS;
    uint8_t status;
    while (attempts) {
        if (read_lsr(&status)) return 1;

        if (status & THR_EMPTY) {
            ready = false;
            if (sys_outb(COM2_BASE + TRANSMITTER_HOLDING_OFFSET, data)) return 1;
            printf("\n\n\nBYTE WRITTEN: %d\n\n\n", data);
            return 0;
        }

        tickdelay(micros_to_ticks(SP_WAIT));
        attempts--;
    }

    return 1;
}

/**
 * @brief Interrupt handler for the serial port
 * Reads the IIR and uses it to the determine which functions to execute
 */
void (sp_ih)() {
    uint8_t iir;
    uint8_t count = 0;
    while (true) {
        util_sys_inb(COM2_BASE + INTERRUPT_IDENT_OFFSET, &iir);
        printf("\n\n\n IIR IS %d\n\n\n", iir);
        if (count > 0) printf("COUNT IS %d\n\n\n", count);
        if (iir & IIR_NO_PENDING) break;
        if ((iir & IIR_NO_PENDING) == 0) {
            switch (iir & INT_ID) {
                case IIR_DATA_AVAILABLE:
                    printf("\n\n\n SHIT IS %d\n\n\n", iir & INT_ID);
                    read_sp_data();
                    new_data = true;
                    break;
                case IIR_TRANSMITTER_EMPTY:
                    printf("\n\n\n SHIT IS %d\n\n\n", iir & INT_ID);
                    ready = true;
                    break;    
            }
        }
        count++;
    }
}

/**
 * @brief Prepares a byte to be sent
 * @param position Position of ball modified
 * @param color Color of ball modified (irrelevant for removing a ball)
 * @param remove 1 if ball is removed, 0 is placed
 * @return uint8_t 
 */
uint8_t (prepare_move_byte)(uint8_t position, uint32_t color, uint8_t remove) {

    unsigned char byte = 0;

    switch (position) {
        case 0:
            byte |= 0x00;
            break;
        case 1:
            byte |= BIT(6);
            break;
        case 2:
            byte |= BIT(7);
            break;
        case 3:
            byte |= BIT(7) | BIT(6);
            break;
        default:
            return 0;
    }

    if (remove == 1) { 
        byte |= BIT(5);
    }

    for (uint i = 0; i < 11; i++) {
        if (color_table[i] == color) {
            byte |= i;
            break;
        }
    }

    return byte;
}
