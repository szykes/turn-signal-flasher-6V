#include "app.h"

#include "avr.h"

#include <stdbool.h>

static volatile bool is_run = false;

void app_interrupt(void) {
  is_run = true;
}

void app_main(void) {
  bool is_running = false;

  mcu_cli();
  if (is_run) {
    is_run = false;
    is_running = true;
  }
  mcu_sei();

  if (is_running) {
  }
}
