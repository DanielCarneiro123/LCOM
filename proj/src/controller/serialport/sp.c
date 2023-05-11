#include "sp.h"

int hook_id_sp = 6;

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

void sp_ih() {
    uint8_t iir;
    util_sys_inb(COM2_BASE + INTERRUPT_IDENT_OFFSET, &iir);
    if ((iir & IIR_NO_PENDING) == 0) {
        switch (iir & INT_ID) {
            case IIR_DATA_AVAILABLE:
                break;
            case IIR_TRANSMITTER_EMPTY:
                break;    
        }
    }
}
