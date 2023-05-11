#ifndef _LCOM_SP_H_
#define _LCOM_SP_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "UART.h"

int sp_subscribe_interrupts(uint8_t *bit_no);
int sp_unsubscribe_interrupts();

#endif
