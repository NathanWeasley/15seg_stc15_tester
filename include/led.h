#include "stdint.h"

void led_init(void);

void led_set_refresh_flag(uint8_t value);
uint8_t led_need_refresh(void);
void led_refresh(void);

void led_command_recv(uint8_t byte);
void led_proc_irq(void);

void led_set_display(uint8_t word);

uint8_t led_1sec(void);
