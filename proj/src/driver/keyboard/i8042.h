#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#define IRQ_KEYBOARD    1

#define BREAK_ESC       0x81
#define MAKE_CODE       BIT(7)
#define TWO_BYTES       0xE0
#define ENABLE_INT      BIT(0)

#define TIMEOUT_ERROR   BIT(6)
#define PARITY_ERROR    BIT(7)
#define FULL_OUT_BUFFER BIT(0)
#define FULL_IN_BUFFER  BIT(1)

#define KBC_STATUS_REG  0x64
#define KBC_IN_CMD      0x64
#define KBC_OUT_CMD     0x60
#define KBC_READ_CMD    0x20
#define KBC_WRITE_CMD   0x60

#define WAIT_KBC        20000
#define MAX_ATTEMPTS     10

#define S_KEY           31
#define G_KEY           34
#define E_KEY           18
#define Q_KEY           16
#define W_KEY           17
#define T_KEY           20
#define R_KEY           0x13
#define ONE_KEY         02
#define TWO_KEY         03
#define THREE_KEY       04
#define FOUR_KEY        05
#define FIVE_KEY        06
#define SIX_KEY         07
#define SEVEN_KEY       0x08
#define EIGHT_KEY       0x09
#define ZERO_KEY        0x0b
#define P_KEY           0x19
#define O_KEY           0x18
#define ENTER_KEY       0x1c

#endif /* _LCOM_I8042_H_ */
