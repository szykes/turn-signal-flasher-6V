#ifndef AVR_H_
#define AVR_H_

typedef enum {
  VOLTAGE_LEVEL_ = 0,
} voltage_level_e;

void hw_init(void);

void mcu_cli(void);
void mcu_sei(void);

void gpio_relay_high(void);
void gpio_relay_low(void);

voltage_level_e adc_get(void);

void wdt_init(void);
void wdt_restart(void);

#endif // AVR_H_
