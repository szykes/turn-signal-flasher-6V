#include "app.h"

#include "framework.h"

static bool tc_logic(void) {
  TEST_BEGIN();

  MOCK_EXPECT("gpio_high", "");
  MOCK_EXPECT("gpio_low", "");

  app_main();

  TEST_END();
}

int main(void) {
  TEST_EVALUATE_INIT();
  TEST_EVALUATE(tc_logic());
  TEST_EVALUATE_END();
}
