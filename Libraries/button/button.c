#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/interrupt.h>
#include <avr/io.h>
#include <button.h>
#include <util/delay.h>

void enableButton (int button) {
  if (button < 0 || button > 2) return;
  DDRC &= ~_BV(PC1 + button);
  PORTC |= _BV(PC1 + button);
}

void enableAllButtons() {
  DDRC &= ~( _BV(PC1) | _BV(PC2) | _BV(PC3) );
  PORTC |= _BV(PC1) | _BV(PC2) | _BV(PC3);
}

int buttonPushed(int button) {
  if (button < 0 || button > 2) return 0;
  if (bit_is_clear(PINC, (PC1 + button))) {
    return 1;
  }

  return 0;
}

int buttonReleased(int button) {
  if (button < 0 || button > 2) return 0;
  if (bit_is_set(PINC, (PC1 + button))) {
    return 1;
  }

  return 0;
}

void enableButtonInterrupt(int button) {
  if (button < 0 || button > 2) return;
  PCICR |= _BV(PCIE1);
  PCMSK1 |= _BV(PC1 + button);
  sei();
}

void enableAllButtonInterrupts() {
  PCICR |= _BV(PCIE1);
  PCMSK1 |= _BV(PC1) | _BV(PC2) | _BV(PC3);
  sei();
}