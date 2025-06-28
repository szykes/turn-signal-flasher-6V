#include "app.h"

#include "framework.h"

#define VOLTAGE_LOW (30u)
#define VOLTAGE_HIGH (230u)

static bool tc_free_running(void) {
  TEST_BEGIN();

  for (int i = 0; i < 1000; i++) {
    MOCK_EXPECT("mcu_cli", "");
    MOCK_EXPECT("mcu_sei", "");

    uint8_t voltage = VOLTAGE_HIGH;
    MOCK_EXPECT_RET("adc_start_and_get", uint8_t, voltage, "");

    bool flashing_state = false;
    MOCK_EXPECT_RET("gpio_flashing_get_state", bool, flashing_state, "");

    app_main();
  }

  TEST_END();
}

int main(void) {
  TEST_EVALUATE_INIT();
  TEST_EVALUATE(tc_free_running());
  TEST_EVALUATE_END();
}
