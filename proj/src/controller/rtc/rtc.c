#include "rtc.h"

// Variáveis globais
int rtc_hook_id = RTC_MASK; // Máscara constante = 5
rtc_info time_info;   // Estrutura que irá conter toda a informação

// O setup consiste em determinar o modo de contagem do RTC
// E também preencher pela primeira vez a informação do tempo
int rtc_setup() {
    if (rtc_set_binary()) return 1;
    if (rtc_update_time()) return 1;
    return 0;
}

int rtc_set_binary() {
    uint8_t status;
    if (rtc_read(REGISTER_B, &status)) return 1;
    if (rtc_write(REGISTER_B, status & BINARY)) return 1;
    return 0;
}

// Subscrição das interrupções. Semelhante aos labs anteriores.
int rtc_subscribe_interrupts(uint8_t *bit_no) {
    if (bit_no == NULL) return 1;
    *bit_no = BIT(rtc_hook_id);
    if (sys_irqsetpolicy(IRQ_RTC, IRQ_REENABLE, &rtc_hook_id)) return 1;
    return 0;
}

// Desativação das interrupções. Semelhante aos labs anteriores.
int rtc_unsubscribe_interrupts() {
    return sys_irqrmpolicy(&rtc_hook_id);
}

// Leitura do output do RTC, dado um comando
int rtc_read(uint8_t rregister, uint8_t *status) {
    if (status == NULL) return 1;
    if (sys_outb(REGISTER_INPUT, rregister)) return 1;
	if (util_sys_inb(REGISTER_OUTPUT, status)) return 1;
    return 0;
}

int rtc_write(uint8_t wregister, uint8_t data) {
    if (sys_outb(REGISTER_INPUT, wregister)) return 1;
    if (sys_outb(REGISTER_OUTPUT, data)) return 1;
    return 0;
}

// Retorna 1 se naquele momento o RTC está a atualizar os seus valores internos
// Nesse caso não devemos ler nenhum registo
int rtc_is_updating() {
    uint8_t status;
    if (rtc_read(REGISTER_A, &status)) return 1;
	return !!(status & UPDATING);
}

// Faz update às informações da struct time_info
int rtc_update_time() {
    
    // Se o RTC estiver ocupado a atualizar os registos não devemos ler dados
    if (rtc_is_updating()) return 1;
    uint8_t time;

    // Seconds
    if (rtc_read(SECONDS, &time)) return 1;
    time_info.seconds = time;

    // Minutes
    if (rtc_read(MINUTES, &time)) return 1;
    time_info.minutes = time;

    // Hours
    if (rtc_read(HOURS, &time)) return 1;
    time_info.hours = time;

    return 0;
}
