#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "i8042.h"
#include "KBC.h"

int (mouse_subscribe_interrupts)(uint8_t *bit_no);

int (mouse_unsubscribe_interrupts)();

int (mouse_restore)();

int (write_to_mouse)(uint8_t command);

#endif
