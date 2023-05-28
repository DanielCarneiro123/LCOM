#ifndef _LCOM_DATA_H_
#define _LCOM_DATA_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>

typedef struct {
    uint16_t x; /** x coordinate of position */
    uint16_t y; /** y coordinate of position */
    uint32_t color; /** color currently present (RGB) */
} Position;

typedef enum {
    START,
    GAME,
    END
} MenuState;

typedef struct {
    uint16_t x; /** x coordinate of position */
    uint16_t y; /** y coordinate of position */
    uint8_t color; /** color currently present (0 for empty, 1 for black, 2 for white) */
} PositionSmall;

#endif
