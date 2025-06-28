#include "avr.h"

#include "mock.h"

void mcu_cli(void) {
  MOCK_RECORD();
}

void mcu_sei(void) {
  MOCK_RECORD();
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

void timer_start(void) {
  MOCK_RECORD();
}

void timer_stop(void) {
  MOCK_RECORD();
}

uint8_t adc_start_and_get(void) {
  MOCK_RECORD_RET(uint8_t);
}
