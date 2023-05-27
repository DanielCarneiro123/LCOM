#ifndef _LCOM_DATA_H_
#define _LCOM_DATA_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>

typedef struct {
    uint16_t x;
    uint16_t y;
    uint32_t color;
} Position;

typedef enum {
    RUNNING,
    EXIT,
} SystemState;

typedef enum {
    START,
    GAME,
    END
} MenuState;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint8_t color;
} PositionSmall;

#endif
