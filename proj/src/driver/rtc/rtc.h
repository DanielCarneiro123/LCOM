#ifndef _LCOM_RTC_H_
#define _LCOM_RTC_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>

#define RTC_MASK            5
#define IRQ_RTC             8
#define REGISTER_INPUT      0x70
#define REGISTER_OUTPUT     0x71

#define REGISTER_A          10
#define REGISTER_B          11

#define SECONDS             0
#define MINUTES             2
#define HOURS               4   

#define BINARY              BIT(2)
#define UPDATING            BIT(7)
#define HOUR_MODE           BIT(1)

typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} rtc_info;

int rtc_setup();
int rtc_read(uint8_t rregister, uint8_t *status);
int rtc_write(uint8_t wregister, uint8_t data);
uint8_t rtc_is_updating();
int rtc_get_binary();
int rtc_get_hours();
uint8_t convert_output(uint8_t output);
uint8_t convert_hours(uint8_T hours);
int rtc_update_time();

#endif
