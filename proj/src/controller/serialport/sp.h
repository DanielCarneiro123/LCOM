#ifndef _LCOM_SP_H_
#define _LCOM_SP_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "UART.h"

int (sp_setup)();
int (sp_subscribe_interrupts)(uint8_t *bit_no);
int (sp_unsubscribe_interrupts)();
void (sp_ih)();
int (read_sp_data)();
int (read_lsr)(uint8_t *lsr);
int (write_sp_data)(uint8_t data);

#endif
