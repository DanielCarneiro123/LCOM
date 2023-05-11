#include "sp.h"

int hook_id_sp = 6;
uint8_t sp_data;

int sp_setup() {
    if (sys_outb(COM2_BASE + LINE_CONTROL_OFFSET, BIT(0) | BIT(1))) return 1;
    if (sys_outb(COM2_BASE + INTERRUPT_ENABLE_OFFSET, BIT(0) | BIT(1))) return 1;
    return 0;
}

int sp_subscribe_interrupts(uint8_t *bit_no) {
  if (bit_no == NULL) return 1;
  *bit_no = BIT(hook_id_sp);
  if (sys_irqsetpolicy(COM2_IRQ, IRQ_REENABLE, &hook_id_sp)) return 1;
  return 0;
}

int sp_unsubscribe_interrupts() {
  if (sys_irqrmpolicy(&hook_id_sp)) return 1;
  return 0;
}

int read_lsr(uint8_t *lsr) {
    return util_sys_inb(COM2_BASE + LINE_STATUS_OFFSET, lsr);
}

int read_sp_data() {
    int attempts = SP_ATTEMPTS;
    uint8_t status;
    while (attempts) {
        if (read_lsr(&status)) return 1;

        if (status & BIT(0)) {
            if (util_sys_inb(COM2_BASE + RECEIVER_BUFFER_OFFSET, &sp_data)) return 1;

            if (status & SP_OVERRUN_ERROR) return 1;
            if (status & SP_PARITY_ERROR) return 1;
            if (status & SP_FRAMING_ERROR) return 1;
            return 0;
        }

        tickdelay(micros_to_ticks(SP_WAIT));
        attempts--;
    }

    return 1;
}

int write_sp_data(uint8_t data) {
    int attempts = SP_ATTEMPTS;
    uint8_t status;
    while (attempts) {
        if (read_lsr(&status)) return 1;

        if (status & BIT(5)) {
            return sys_outb(COM2_BASE + TRANSMITTER_HOLDING_OFFSET, data);
        }

        tickdelay(micros_to_ticks(SP_WAIT));
        attempts--;
    }

    return 1;
}

void sp_ih() {
    uint8_t iir;
    util_sys_inb(COM2_BASE + INTERRUPT_IDENT_OFFSET, &iir);
    if ((iir & IIR_NO_PENDING) == 0) {
        switch (iir & INT_ID) {
            case IIR_DATA_AVAILABLE:
                read_sp_data();
                break;
            case IIR_TRANSMITTER_EMPTY:
                write_sp_data(143);
                break;    
        }
    }
}
