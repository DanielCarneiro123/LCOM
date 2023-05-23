#include "sp.h"

int hook_id_sp = 6;
uint8_t sp_data = 0;
bool ready = true;
bool new_data = false;
extern uint32_t color_table[];

int (sp_setup)() {
    if (sys_outb(COM2_BASE + LINE_CONTROL_OFFSET, DIVISOR_LATCH_BIT)) return 1;
    if (sys_outb(COM2_BASE + 0, 0x8)) return 1;
    if (sys_outb(COM2_BASE + 1, 0x7)) return 1;

    if (sys_outb(COM2_BASE + LINE_CONTROL_OFFSET, EIGHT_BIT_CHAR)) return 1;
    if (sys_outb(COM2_BASE + FIFO_CONTROL_OFFSET, FIFO_DISABLED)) return 1;
    if (sys_outb(COM2_BASE + INTERRUPT_ENABLE_OFFSET, IER_DATA_AVAILABLE | IER_THR_EMPTY)) return 1;
    return 0;
}

int (sp_subscribe_interrupts)(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = BIT(hook_id_sp);
  if (sys_irqsetpolicy(COM2_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_sp)) return 1;
  return 0;
}

int (sp_unsubscribe_interrupts)() {
  if (sys_irqrmpolicy(&hook_id_sp)) return 1;
  return 0;
}

int (read_lsr)(uint8_t *lsr) {
    return util_sys_inb(COM2_BASE + LINE_STATUS_OFFSET, lsr);
}

int (read_sp_data)() {
    int attempts = SP_ATTEMPTS;
    uint8_t status;
    while (attempts) {
        if (read_lsr(&status)) return 1;

        if (status & RECIEVER_DATA_AVAILABLE) {
            if (util_sys_inb(COM2_BASE + RECEIVER_BUFFER_OFFSET, &sp_data)) return 1;
            printf("\n\n\nBYTE READ: %d\n\n\n", sp_data);

            if (sp_data != SP_NACK && sp_data != SP_ACK && (status & SP_OVERRUN_ERROR || status & SP_PARITY_ERROR || status & SP_FRAMING_ERROR)) write_sp_data(SP_NACK);
            else if (sp_data != SP_NACK && sp_data != SP_ACK) write_sp_data(SP_ACK);
            return 0;
        }

        tickdelay(micros_to_ticks(SP_WAIT));
        attempts--;
    }

    return 0;
}

int (write_sp_data)(uint8_t data) {
    int attempts = SP_ATTEMPTS;
    uint8_t status;
    while (attempts) {
        if (read_lsr(&status)) return 1;

        if (status & THR_EMPTY) {
            printf("\n\n\nBYTE WRITTEN: %d\n\n\n", data);
            ready = false;
            return sys_outb(COM2_BASE + TRANSMITTER_HOLDING_OFFSET, data);
        }

        tickdelay(micros_to_ticks(SP_WAIT));
        attempts--;
    }

    return 1;
}

void (sp_ih)() {
    uint8_t iir;
    util_sys_inb(COM2_BASE + INTERRUPT_IDENT_OFFSET, &iir);
    printf("\n\n\n IIR IS %d\n\n\n", iir);
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
}

uint8_t (prepare_move_byte)(uint8_t position, uint32_t color, uint8_t size) {

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

    if (size == 1) { 
        byte |= BIT(5); /*então é ball*/
    }

    for (uint i = 0; i < 8; i++) {
        if (color_table[i] == color) {
            byte |= i;
            break;
        }
    }

    return byte;
}
