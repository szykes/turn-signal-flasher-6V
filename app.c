#include "app.h"

#include "avr.h"

#include <stdbool.h>

#define VOLTAGE_LEVEL (50u)

static volatile bool is_run = false;

static bool is_flashing = false;
static bool is_shortcut = false;

static void evaluate_low_voltage(void) {
  if (gpio_flashing_get_state()) {
    // shortcut
    gpio_flashing_turn_off();
    is_flashing = false;
    is_shortcut = true;
  } else {
    // started flashing OR turned off bulbs OR shortcut
    if (!is_shortcut) {
      is_flashing = true;
    }
  }
}

static void evaluate_high_voltage(void) {
  if (gpio_flashing_get_state()) {
    // turned on bulbs
    // do nothing
  } else {
    // not switched turn signaling
    is_flashing = false;
    is_shortcut = false;
  }
}

static void check_voltage(void) {
  uint8_t voltage = adc_start_and_get();

  if (voltage < VOLTAGE_LEVEL) {
    evaluate_low_voltage();
  } else {
    evaluate_high_voltage();
  }
}

static void evaluate_flashing_state(void) {
  static bool prev_is_flashing = false;

  if (!prev_is_flashing && is_flashing) {
    gpio_flashing_turn_on();
    timer_start();
  } if (prev_is_flashing && !is_flashing) {
    gpio_flashing_turn_off();
    timer_stop();
  }

  prev_is_flashing = is_flashing;
}

void app_timer_interrupt(void) {
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
    if (gpio_flashing_get_state()) {
      gpio_flashing_turn_off();
    } else {
      gpio_flashing_turn_on();
    }
  }

  check_voltage();
  evaluate_flashing_state();
}
