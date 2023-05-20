#define COM1_BASE       0x3F8
#define COM2_BASE       0x2F8

#define COM1_IRQ        4
#define COM2_IRQ        3

#define RECEIVER_BUFFER_OFFSET          0
#define TRANSMITTER_HOLDING_OFFSET      0
#define INTERRUPT_ENABLE_OFFSET         1
#define INTERRUPT_IDENT_OFFSET          2
#define FIFO_CONTROL_OFFSET             2
#define LINE_CONTROL_OFFSET             3
#define MODEM_CONTROL_OFFSET            4
#define LINE_STATUS_OFFSET              5
#define MODEM_STATUS_OFFSET             6
#define SCRATCHPAD_OFFSET               7

#define IIR_NO_PENDING                  BIT(0)
#define IIR_DATA_AVAILABLE              BIT(2)
#define IIR_TRANSMITTER_EMPTY           BIT(1)
#define INT_ID                          (BIT(0) | BIT(1) | BIT(2) | BIT(3))

#define SP_OVERRUN_ERROR                   BIT(1)
#define SP_PARITY_ERROR                    BIT(2)
#define SP_FRAMING_ERROR                   BIT(3)

#define RECIEVER_DATA_AVAILABLE            BIT(0)
#define THR_EMPTY                          BIT(5)

#define SP_ATTEMPTS                     10
#define SP_WAIT                         20000
