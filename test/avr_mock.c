#include "avr.h"

#include "mock.h"

void gpio_high(void) {
  MOCK_RECORD();
}

void gpio_low(void) {
  MOCK_RECORD();
}
