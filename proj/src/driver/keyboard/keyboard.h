#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

#define KEYBOARD_MASK BIT(1)

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "i8042.h"
#include "KBC.h"

int (keyboard_subscribe_interrupts)(uint8_t *bit_no);

int (keyboard_unsubscribe_interrupts)();

void (kbc_ih_poll)();
void (kbc_ih)();

#endif
