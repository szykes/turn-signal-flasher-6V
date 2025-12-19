#include "app.h"

#include "avr.h"

#include <stdbool.h>

#define VOLTAGE_DIFF (15u) // ~170 mV

static volatile bool is_interrupted = false;

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

  if (voltage < VOLTAGE_DIFF) {
    evaluate_low_voltage();
  } else {
    evaluate_high_voltage();
  }
}

static void evaluate_flashing_state(void) {
  static bool prev_is_flashing = false;

  if (!prev_is_flashing && is_flashing) {
    gpio_flashing_turn_on();
    timer_start_long();
  } if (prev_is_flashing && !is_flashing) {
    gpio_flashing_turn_off();
    timer_stop();
  }

  prev_is_flashing = is_flashing;
}

void app_timer_interrupt(void) {
  is_interrupted = true;
}

void app_main(void) {
  bool has_interrupted = false;
  mcu_disable_global_interrupt();
  if (is_interrupted) {
    is_interrupted = false;
    has_interrupted = true;
  }
  mcu_enable_global_interrupt();

  if (has_interrupted) {
    timer_set_short();
    if (gpio_flashing_get_state()) {
      gpio_flashing_turn_off();
    } else {
      gpio_flashing_turn_on();
    }
  }

  // Wait for closing MOSFET
  delay_us(70);

  check_voltage();
  evaluate_flashing_state();
}
