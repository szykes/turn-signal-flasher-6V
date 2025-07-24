#include "avr.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "app.h"

void hw_init(void) {
  // PB1 -> output for flashing
  // PB3 -> ADC3 -> bulb reference
  // PB4 -> ADC2 -> Vcc reference

  DDRB = (1 << DDB1);
  gpio_flashing_turn_off();

  OCR1C = 65;
  TIMSK = (1 << OCIE1A);

  ADMUX = (1 << ADLAR) | (1 << MUX1) | (1 << MUX2);
  DIDR0 = (1 << ADC2D ) | (1 << ADC3D);

  wdt_enable(WDTO_250MS);
}

void mcu_sei(void) {
  sei();
}

void mcu_cli(void) {
  cli();
}

bool gpio_flashing_get_state(void) {
  return !(PORTB & (1 << PORT1));
}

void gpio_flashing_turn_off(void) {
  PORTB |= (1 << PORT1);
}

void gpio_flashing_turn_on(void) {
  PORTB &= ~(1 << PORT1);
}

void timer_start(void) {
  TCCR1 = (1 << CTC1) | (1 << CS13) | (1 << CS12) | (1 << CS11); // Prescaler is 8192
}

void timer_stop(void) {
  TCCR1 = 0;
  TCNT1 = 0;
}

ISR(TIMER1_COMPA_vect, ISR_BLOCK) {
  app_timer_interrupt();
}

uint8_t adc_start_and_get(void) {
  // ADC = ((Vpos - Vneg) * 1024) / Vcc
  ADCSRA = (1 << ADEN) | (1 << ADSC);
  while ((ADCSRA & (1 << ADIF)) == 0) {}
  ADCSRA |= (1 << ADIF);
  return ADCH;
}

void wdt_restart(void) {
  wdt_reset();
}
