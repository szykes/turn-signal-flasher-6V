#include "avr.h"
#include "logic.h"

int main()
{
  hw_init();

  while(1) {
    make();
  }
}
