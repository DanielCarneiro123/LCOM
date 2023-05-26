#include "queue.h"

static uint8_t size = 0;
static uint8_t top = 0;
static uint8_t queue[16];
extern bool ready;
uint8_t writing_byte;
bool is_writing = false;

/**
 * @brief Pushes a byte to the queue
 * @param byte Byte to push
 * @return int 1 on failure, 0 otherwise
 */
int push(uint8_t byte) {
    if (size >= 16) return 1;
    
    queue[(top + size) % 16] = byte;
    size++;
    
    return 0;
}

/**
 * @brief Checks if the queue has a byte available
 * @return true The queue has a byte available
 * @return false The queue is empty
 */
bool has_byte() {
    return size != 0;
}

/**
 * @brief Pops a byte from the queue
 * After the byte is popped, instead of moving everything one byte to the left, the head of the queue is moved one byte to the right
 * @return uint8_t Popped byte
 */
uint8_t pop() {
    if (size == 0) return 0;
    
    uint8_t byte = queue[top];
    top = (top + 1) % 16;
    size--;

    return byte;
}

/**
 * @brief Updates the queue
 * If the serial port is ready for data and a byte is not being sent, the top byte is sent
 * @return int 1 on failure, 0 otherwise
 */
int update_queue() {
    if (ready && !is_writing) {
        if (has_byte()) {
            writing_byte = pop();
            is_writing = true;
            if (write_sp_data(writing_byte)) return 1;
        }
    }
    return 0;
}

/**
 * @brief Retries to send the last byte
 * Is used when there was an error sending the last byte
 * @return int 1 on failure, 0 otherwise
 */
int retry() {
    if (is_writing) {
        if (write_sp_data(writing_byte)) return 1;
    }
    return 0;
}
