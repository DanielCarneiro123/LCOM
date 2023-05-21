#include "queue.h"

static uint8_t size = 0;
static uint8_t top = 0;
static uint8_t queue[16];
extern bool ready;

int push(uint8_t byte) {
    if (size >= 16) return 1;
    
    queue[(top + size) % 16] = byte;
    size++;
    
    return 0;
}

bool has_byte() {
    return size != 0;
}

uint8_t pop() {
    if (size == 0) return 0;
    
    uint8_t byte = queue[top];
    top = (top + 1) % 16;
    size--;

    return byte;
}

int update_queue() {
    if (ready) {
        printf("\n\n\n I AM READY \n\n\n");
        if (has_byte()) {
            uint8_t data_to_send = pop();
            if (write_sp_data(data_to_send)) return 1;
        }
    }
    return 0;
}
