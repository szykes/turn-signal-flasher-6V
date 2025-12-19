#ifndef AVR_H_
#define AVR_H_

#include <stdint.h>
#include <stdbool.h>

void hw_init(void);

void mcu_enable_global_interrupt(void);
void mcu_disable_global_interrupt(void);

void delay_us(uint16_t us);

bool gpio_flashing_get_state(void);
void gpio_flashing_turn_off(void);
void gpio_flashing_turn_on(void);

void timer_start_long(void);
void timer_set_short(void);
void timer_stop(void);

uint8_t adc_start_and_get(void);

void wdt_restart(void);

#endif // AVR_H_
