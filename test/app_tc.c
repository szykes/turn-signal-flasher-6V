#include "app.h"

#include "framework.h"

#include <limits.h>
#include <stdio.h>

#define VOLTAGE_LOW (5u)
#define VOLTAGE_HIGH (230u)

const static uint16_t us = 70;

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
    MOCK_EXPECT("mcu_disable_global_interrupt", str);
    MOCK_EXPECT("mcu_enable_global_interrupt", str);
    MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, str);

    high_voltage_flashing_on(str);

    app_main();

    TEST_CHECK_MOCK();
  }
}

static void do_nothing_when_flashing_off(const char *str) {
  for (size_t i = 0; i < 10; i++) {
    MOCK_EXPECT("mcu_disable_global_interrupt", str);
    MOCK_EXPECT("mcu_enable_global_interrupt", str);
    MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, str);

    low_voltage_flashing_off(str);

    app_main();

    TEST_CHECK_MOCK();
  }
}

static void be_idle(const char *str) {
  for (size_t i = 0; i < 100; i++) {
    MOCK_EXPECT("mcu_disable_global_interrupt", str);
    MOCK_EXPECT("mcu_enable_global_interrupt", str);
    MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, str);

    high_voltage_flashing_off(str);

    app_main();

    TEST_CHECK_MOCK();
  }
}

static bool tc_free_running(void) {
  TEST_BEGIN();

  be_idle("");

  TEST_END();
}

static bool tc_shortcut(void) {
  TEST_BEGIN();

  MOCK_EXPECT("mcu_disable_global_interrupt", "turning on");
  MOCK_EXPECT("mcu_enable_global_interrupt", "turning on");
  MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, "turning on");

  low_voltage_flashing_on("turning on");

  MOCK_EXPECT("gpio_flashing_turn_off", "turning off due to shortcut");

  app_main();

  for (size_t i = 0; i < 10; i++) {
    MOCK_EXPECT("mcu_disable_global_interrupt", "remain turned off");
    MOCK_EXPECT("mcu_enable_global_interrupt", "remain turned off");
    MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, "remain turned off");

    low_voltage_flashing_off("remain turned off");

    app_main();

    TEST_CHECK_MOCK();
  }

  MOCK_EXPECT("mcu_disable_global_interrupt", "shortcut gone");
  MOCK_EXPECT("mcu_enable_global_interrupt", "shortcut gone");
  MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, "shortcut gone");

  high_voltage_flashing_off("shortcut gone");

  app_main();

  TEST_END();
}

static bool tc_flashing_start_then_stop(void) {
  TEST_BEGIN();

  char msg[50];
  bool flashing_state;

  MOCK_EXPECT("mcu_disable_global_interrupt", "starting flashing");
  MOCK_EXPECT("mcu_enable_global_interrupt", "starting flashing");
  MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, "starting flashing");

  low_voltage_flashing_off("starting flashing");
  MOCK_EXPECT("gpio_flashing_turn_on", "starting flashing");
  MOCK_EXPECT("timer_start_long", "starting flashing");

  app_main();

  do_nothing_when_flashing_on("flashing turned on");

  for (size_t i = 0; i < 30; i++) {
    msg[0] = '\0';
    snprintf(msg, sizeof(msg), "flashing turning off, i: %lu", i);

    app_timer_interrupt();

    MOCK_EXPECT("mcu_disable_global_interrupt", msg);
    MOCK_EXPECT("mcu_enable_global_interrupt", msg);

    MOCK_EXPECT("timer_set_short", msg);
    flashing_state = true;
    MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, msg);
    MOCK_EXPECT("gpio_flashing_turn_off", msg);

    MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, msg);

    low_voltage_flashing_off(msg);

    app_main();

    msg[0] = '\0';
    snprintf(msg, sizeof(msg), "flashing turned off, i: %lu", i);

    do_nothing_when_flashing_off(msg);

    msg[0] = '\0';
    snprintf(msg, sizeof(msg), "flashing turning on, i: %lu", i);
    app_timer_interrupt();

    MOCK_EXPECT("mcu_disable_global_interrupt", msg);
    MOCK_EXPECT("mcu_enable_global_interrupt", msg);

    MOCK_EXPECT("timer_set_short", msg);
    flashing_state = false;
    MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, msg);
    MOCK_EXPECT("gpio_flashing_turn_on", msg);

    MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, msg);

    high_voltage_flashing_on(msg);

    app_main();

    msg[0] = '\0';
    snprintf(msg, sizeof(msg), "flashing turned on, i: %lu", i);
    do_nothing_when_flashing_on(msg);

    TEST_CHECK_MOCK();
  }

  app_timer_interrupt();

  MOCK_EXPECT("mcu_disable_global_interrupt", "last flashing turning off");
  MOCK_EXPECT("mcu_enable_global_interrupt", "last flashing turning off");

  MOCK_EXPECT("timer_set_short", msg);
  flashing_state = true;
  MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, "last flashing turning off");
  MOCK_EXPECT("gpio_flashing_turn_off", "last flashing turning off");
  MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, "last flashing turning off");

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

  MOCK_EXPECT("mcu_disable_global_interrupt", "starting flashing");
  MOCK_EXPECT("mcu_enable_global_interrupt", "starting flashing");
  MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, "starting flashing");

  low_voltage_flashing_off("starting flashing");
  MOCK_EXPECT("gpio_flashing_turn_on", "starting flashing");
  MOCK_EXPECT("timer_start_long", "starting flashing");

  app_main();

  do_nothing_when_flashing_on("flashing turned on");

  app_timer_interrupt();

  MOCK_EXPECT("mcu_disable_global_interrupt", "flashing turning off");
  MOCK_EXPECT("mcu_enable_global_interrupt", "flashing turning off");

  MOCK_EXPECT("timer_set_short", "flashing turning off");
  flashing_state = true;
  MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, "flashing turning off");
  MOCK_EXPECT("gpio_flashing_turn_off", "flashing turning off");
  MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, "flashing turning off");

  low_voltage_flashing_off("flashing turning off");

  app_main();

  TEST_CHECK_MOCK();

  do_nothing_when_flashing_off("flashing turned off");

  app_timer_interrupt();

  MOCK_EXPECT("mcu_disable_global_interrupt", "flashing turning on");
  MOCK_EXPECT("mcu_enable_global_interrupt", "flashing turning on");

  MOCK_EXPECT("timer_set_short", "flashing turning on");
  flashing_state = false;
  MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, "flashing turning on");
  MOCK_EXPECT("gpio_flashing_turn_on", "flashing turning on");
  MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, "flashing turning on");

  high_voltage_flashing_on("flashing turning on");

  app_main();

  MOCK_EXPECT("mcu_disable_global_interrupt", "shortcut");
  MOCK_EXPECT("mcu_enable_global_interrupt", "shortcut");
  MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, "shortcut");

  low_voltage_flashing_on("shortcut");
  MOCK_EXPECT("gpio_flashing_turn_off", "shortcut");
  MOCK_EXPECT("gpio_flashing_turn_off", "shortcut");
  MOCK_EXPECT("timer_stop", "shortcut");

  app_main();

  TEST_CHECK_MOCK();

  for (size_t i = 0; i < 10; i++) {
    MOCK_EXPECT("mcu_disable_global_interrupt", "remain turned off");
    MOCK_EXPECT("mcu_enable_global_interrupt", "remain turned off");
    MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, "remain turned off");

    low_voltage_flashing_off("remain turned off");

    app_main();

    TEST_CHECK_MOCK();
  }

  MOCK_EXPECT("mcu_disable_global_interrupt", "shortcut gone");
  MOCK_EXPECT("mcu_enable_global_interrupt", "shortcut gone");
  MOCK_EXPECT_1_PARAM("delay_us", uint16_t, us, "shortcut gone");

  high_voltage_flashing_off("shortcut gone");

  app_main();

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
