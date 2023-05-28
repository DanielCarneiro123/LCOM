#include "rtc.h"

int rtc_hook_id = RTC_MASK; 
rtc_info time_info; 
uint8_t is_binary;
uint8_t full_hours;

/**
 * @brief Sets up the RTC for use
 * Sets the counting mode to binary and fills the time struct for the first time
 * @return 1 on failure, 0 otherwise
 */
int rtc_setup() {
    time_info.hours = 0;
    if (rtc_get_binary()) return 1;
    if (rtc_get_hours()) return 1;
    if (rtc_update_time()) return 1;
    return 0;
}

/**
 * @brief Sets the counting mode to binary
 * @return int 1 on failure, 0 otherwise
 */
int rtc_get_binary() {
    uint8_t status;
    if (rtc_read(REGISTER_B, &status)) return 1;
    if (status & BINARY) is_binary = 1;
    else is_binary = 0;
    return 0;
}

int rtc_get_hours() {
    uint8_t status;
    if (rtc_read(REGISTER_B, &status)) return 1;
    if (status & HOUR_MODE) full_hours = 1;
    else full_hours = 0;
    return 0;
}

/**
 * @brief Reads a byte from the RTC
 * @param rregister Register to read from
 * @param status Address of byte in which to place output
 * @return int 1 on failure, 0 otherwise
 */
int rtc_read(uint8_t rregister, uint8_t *status) {
    if (status == NULL) return 1;
    if (sys_outb(REGISTER_INPUT, rregister)) return 1;
	if (util_sys_inb(REGISTER_OUTPUT, status)) return 1;
    return 0;
}

/**
 * @brief Writes a byte to the RTC
 * @param wregister Register to write to
 * @param data Byte to write
 * @return int 1 on failure, 0 otherwise
 */
int rtc_write(uint8_t wregister, uint8_t data) {
    if (sys_outb(REGISTER_INPUT, wregister)) return 1;
    if (sys_outb(REGISTER_OUTPUT, data)) return 1;
    return 0;
}

/**
 * @brief Checks whether the RTC is updating
 * @return uint8_t 0 if not updating, 1 if updating 
 */
uint8_t rtc_is_updating() {
    uint8_t status;
    if (rtc_read(REGISTER_A, &status)) return 1;
	return !!(status & UPDATING);
}

uint8_t convert_output(uint8_t output) {
    uint8_t result = 0;
    if (is_binary == 0) {
        result += output & 0xF;
        result += 10 * (output >> 4);
    }
    else {
        result = output;
    }
    return result;
}

uint8_t convert_hours(uint8_t hours) {
    if (full_hours) {
        hours = convert_output(hours);
    }
    else {
        if (is_binary) {
            if (hours & BIT(7)) {
                hours &= 0x7F;
                hours += 12;
            }
        }
        else {
            if (hours & BIT(7)) {
                hours = convert_output(hours & 0x7F);
                hours += 12;
            }
            else {
                hours = convert_output(hours & 0x7F);
            }
        }
        hours--;
    }

    return hours;
}

/**
 * @brief Updates the values in the time info struct
 * A buffer struct is used while filling in the values in case an error occurs midway. At the end its contents are copied to the main struct
 * @return int 1 on failure, 0 otherwise
 */
int rtc_update_time() {
    
    if (rtc_is_updating()) return 1;
    uint8_t time;

    rtc_info time_buffer;
    if (rtc_read(SECONDS, &time)) return 1;
    time_buffer.seconds = convert_output(time);

    if (rtc_read(MINUTES, &time)) return 1;
    time_buffer.minutes = convert_output(time);

    if (rtc_read(HOURS, &time)) return 1;
    time_buffer.hours = convert_hours(time);

    time_info.seconds = time_buffer.seconds;
    time_info.minutes = time_buffer.minutes;
    time_info.hours = time_buffer.hours;
    return 0;
}
