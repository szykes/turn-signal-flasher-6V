#include "avr.h"

#include "mock.h"

void mcu_enable_global_interrupt(void) {
  MOCK_RECORD();
}

void mcu_disable_global_interrupt(void) {
  MOCK_RECORD();
}

void delay_us(uint16_t us) {
  MOCK_RECORD_1_PARAM(uint16_t, us);
}

bool gpio_flashing_get_state(void) {
  MOCK_RECORD_RET(bool)
}

void gpio_flashing_turn_off(void) {
  MOCK_RECORD();
}

void gpio_flashing_turn_on(void) {
  MOCK_RECORD();
}

void timer_start_long(void) {
  MOCK_RECORD();
}

void timer_set_short(void) {
  MOCK_RECORD();
}

void timer_stop(void) {
  MOCK_RECORD();
}

uint8_t adc_start_and_get(void) {
  MOCK_RECORD_RET(uint8_t);
}
