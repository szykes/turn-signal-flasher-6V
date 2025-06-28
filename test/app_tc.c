#include "app.h"

#include "framework.h"

#include <limits.h>
#include <stdio.h>

#define VOLTAGE_LOW (30u)
#define VOLTAGE_HIGH (230u)

static void high_voltage_flashing_off(const char *str) {
  uint8_t voltage = VOLTAGE_HIGH;
  MOCK_EXPECT_RET("adc_start_and_get", uint8_t, voltage, str);

  bool flashing_state = false;
  MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, str);
}

static void high_voltage_flashing_on(const char *str) {
  uint8_t voltage = VOLTAGE_HIGH;
  MOCK_EXPECT_RET("adc_start_and_get", uint8_t, voltage, str);

  bool flashing_state = true;
  MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, str);
}

static void low_voltage_flashing_off(const char *str) {
  uint8_t voltage = VOLTAGE_LOW;
  MOCK_EXPECT_RET("adc_start_and_get", uint8_t, voltage, str);

  bool flashing_state = false;
  MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, str);
}

static void low_voltage_flashing_on(const char *str) {
  uint8_t voltage = VOLTAGE_LOW;
  MOCK_EXPECT_RET("adc_start_and_get", uint8_t, voltage, str);

  bool flashing_state = true;
  MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, str);
}

static void do_nothing_when_flashing_on(const char *str) {
  for (size_t i = 0; i < 10; i++) {
    MOCK_EXPECT("mcu_cli", str);
    MOCK_EXPECT("mcu_sei", str);

    high_voltage_flashing_on(str);

    app_main();
  }
}

static void do_nothing_when_flashing_off(const char *str) {
  for (size_t i = 0; i < 10; i++) {
    MOCK_EXPECT("mcu_cli", str);
    MOCK_EXPECT("mcu_sei", str);

    low_voltage_flashing_off(str);

    app_main();
  }
}

static void be_idle(const char *str) {
  for (size_t i = 0; i < 100; i++) {
    MOCK_EXPECT("mcu_cli", str);
    MOCK_EXPECT("mcu_sei", str);

    high_voltage_flashing_off(str);

    app_main();
  }
}

static bool tc_free_running(void) {
  TEST_BEGIN();

  be_idle("");

  TEST_END();
}

static bool tc_shortcut(void) {
  TEST_BEGIN();

  MOCK_EXPECT("mcu_cli", "");
  MOCK_EXPECT("mcu_sei", "");

  low_voltage_flashing_on("");

  MOCK_EXPECT("gpio_flashing_turn_off", "");

  app_main();

  TEST_END();
}

static bool tc_flashing_start_then_stop(void) {
  TEST_BEGIN();

  char msg[50];
  bool flashing_state;

  MOCK_EXPECT("mcu_cli", "starting flashing");
  MOCK_EXPECT("mcu_sei", "starting flashing");

  low_voltage_flashing_off("starting flashing");
  MOCK_EXPECT("gpio_flashing_turn_on", "starting flashing");
  MOCK_EXPECT("timer_start", "starting flashing");

  app_main();

  do_nothing_when_flashing_on("flashing turned on");

  for (size_t i = 0; i < 30; i++) {
    msg[0] = '\0';
    snprintf(msg, sizeof(msg), "flashing turning off, i: %lu", i);

    app_timer_interrupt();

    MOCK_EXPECT("mcu_cli", msg);
    MOCK_EXPECT("mcu_sei", msg);

    flashing_state = true;
    MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, msg);
    MOCK_EXPECT("gpio_flashing_turn_off", msg);
    low_voltage_flashing_off(msg);

    app_main();

    msg[0] = '\0';
    snprintf(msg, sizeof(msg), "flashing turned off, i: %lu", i);

    do_nothing_when_flashing_off(msg);

    msg[0] = '\0';
    snprintf(msg, sizeof(msg), "flashing turning on, i: %lu", i);
    app_timer_interrupt();

    MOCK_EXPECT("mcu_cli", msg);
    MOCK_EXPECT("mcu_sei", msg);

    flashing_state = false;
    MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, msg);
    MOCK_EXPECT("gpio_flashing_turn_on", msg);
    high_voltage_flashing_on(msg);

    app_main();

    msg[0] = '\0';
    snprintf(msg, sizeof(msg), "flashing turned on, i: %lu", i);
    do_nothing_when_flashing_on(msg);
  }

  app_timer_interrupt();

  MOCK_EXPECT("mcu_cli", "last flashing turning off");
  MOCK_EXPECT("mcu_sei", "last flashing turning off");

  flashing_state = true;
  MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, "last flashing turning off");
  MOCK_EXPECT("gpio_flashing_turn_off", "last flashing turning off");
  high_voltage_flashing_off("last flashing turning off");
  MOCK_EXPECT("gpio_flashing_turn_off", "stopping flashing");
  MOCK_EXPECT("timer_stop", "stopping flashing");

  app_main();

  be_idle("stopped flashing");

  TEST_END();
}

static bool tc_flashing_start_then_shortcut(void) {
  TEST_BEGIN();

  bool flashing_state;

  MOCK_EXPECT("mcu_cli", "starting flashing");
  MOCK_EXPECT("mcu_sei", "starting flashing");

  low_voltage_flashing_off("starting flashing");
  MOCK_EXPECT("gpio_flashing_turn_on", "starting flashing");
  MOCK_EXPECT("timer_start", "starting flashing");

  app_main();

  do_nothing_when_flashing_on("flashing turned on");

  app_timer_interrupt();

  MOCK_EXPECT("mcu_cli", "flashing turning off");
  MOCK_EXPECT("mcu_sei", "flashing turning off");

  flashing_state = true;
  MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, "flashing turning off");
  MOCK_EXPECT("gpio_flashing_turn_off", "flashing turning off");
  low_voltage_flashing_off("flashing turning off");

  app_main();

  do_nothing_when_flashing_off("flashing turned off");

  app_timer_interrupt();

  MOCK_EXPECT("mcu_cli", "flashing turning on");
  MOCK_EXPECT("mcu_sei", "flashing turning on");

  flashing_state = false;
  MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, "flashing turning on");
  MOCK_EXPECT("gpio_flashing_turn_on", "flashing turning on");
  high_voltage_flashing_on("flashing turning on");

  app_main();

  MOCK_EXPECT("mcu_cli", "shortcut");
  MOCK_EXPECT("mcu_sei", "shortcut");

  low_voltage_flashing_on("shortcut");
  MOCK_EXPECT("gpio_flashing_turn_off", "shortcut");
  MOCK_EXPECT("gpio_flashing_turn_off", "shortcut");
  MOCK_EXPECT("timer_stop", "shortcut");

  app_main();

  be_idle("stopped flashing");

  TEST_END();
}

int main(void) {
  TEST_EVALUATE_INIT();
  TEST_EVALUATE(tc_free_running());
  TEST_EVALUATE(tc_shortcut());
  TEST_EVALUATE(tc_flashing_start_then_stop());
  TEST_EVALUATE(tc_flashing_start_then_shortcut());
  TEST_EVALUATE_END();
}
