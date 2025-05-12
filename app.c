#include "app.h"

#include "avr.h"

void app_interrupt(void) {
}

void app_main(void) {
  gpio_high();
  gpio_low();
}
