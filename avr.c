#include "avr.h"

#include <avr/io.h>

void hw_init(void) {
  DDRA = (1 << DD3);
}

void gpio_high(void) {
  PORTA |= (1 << PORT3);
}

void gpio_low(void) {
  PORTA &= ~(1 << PORT3);
}
