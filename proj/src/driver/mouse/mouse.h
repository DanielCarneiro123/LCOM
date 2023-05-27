#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

#define MOUSE_MASK BIT(2)

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "i8042.h"
#include "KBC.h"
#include "driver/video/graphics.h"

typedef struct {
    uint8_t right_click;
    uint8_t left_click;
    uint32_t ball_color;
    int16_t x;
    int16_t y;
} MouseInfo;   

int (mouse_subscribe_interrupts)(uint8_t *bit_no);

int (mouse_unsubscribe_interrupts)();

void (mouse_ih)();

void (mouse_update_buffer)();

void (mouse_sync_info)();

int (mouse_write)(uint8_t command);

#endif
