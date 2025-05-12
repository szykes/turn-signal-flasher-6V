#include "avr.h"
#include "app.h"

int main()
{
  hw_init();

  while(1) {
    app_main();
  }
}
