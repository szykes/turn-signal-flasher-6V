#include "avr.h"

#include <avr/io.h>

void hw_init(void) {
  DDRA = (1 << DD3);
}
