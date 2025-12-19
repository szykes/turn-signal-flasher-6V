#include "avr.h"
#include "app.h"

int main(void)
{
  hw_init();
  mcu_enable_global_interrupt();

  while(1) {
    app_main();
    wdt_restart();
  }
}
