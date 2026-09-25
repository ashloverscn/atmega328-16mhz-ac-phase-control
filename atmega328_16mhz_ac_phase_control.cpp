/*
this library is intended to be use with atmega328 and 168
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>
#include "atmega328_16mhz_ac_phase_control.h"

#define MAX_PHASE_DELAY 623
#define MIN_PHASE_DELAY 1
#define TRIAC_PULSE_TICKS 2

static volatile int current_phase_delay = MAX_PHASE_DELAY;

void atmega328_16mhz_ac_phase_controlClass::init()
{
  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);

  DDRB |= (1 << PB1);
  PORTB &= ~(1 << PB1);

  TCCR1A = 0x00;
  TCCR1B = 0x00;
  TCNT1 = 0x00;

  TIMSK1 = (1 << TOIE1) | (1 << OCIE1A);

  EICRA &= ~((1 << ISC01) | (1 << ISC00));
  EICRA |= (1 << ISC01);

  EIMSK |= (1 << INT0);

  sei();
}

void atmega328_16mhz_ac_phase_controlClass::uninit()
{
  PORTB &= ~(1 << PB1);

  DDRD = 0x00;
  PORTD = 0x00;
  DDRB = 0x00;
  PORTB = 0x00;

  TIMSK1 = 0x00;
  TCCR1A = 0x00;
  TCCR1B = 0x00;
  TCNT1 = 0x00;
  OCR1A = 0x00;

  EICRA = 0x00;
  EIMSK = 0x00;

  cli();
}

void atmega328_16mhz_ac_phase_controlClass::set_ac_power(int value)
{
  if (value < MIN_PHASE_DELAY)
  {
    value = MIN_PHASE_DELAY;
  }

  if (value > MAX_PHASE_DELAY)
  {
    value = MAX_PHASE_DELAY;
  }

  current_phase_delay = value;
}

ISR(INT0_vect)
{
  PORTB &= ~(1 << PB1);

  TCCR1B = 0x00;
  TCNT1 = 0x00;
  OCR1A = current_phase_delay;

  TIFR1 |= (1 << OCF1A) | (1 << TOV1);

  TCCR1B = 0x04;
}

ISR(TIMER1_COMPA_vect)
{
  PORTB |= (1 << PB1);

  OCR1A = TCNT1 + TRIAC_PULSE_TICKS;

  TIMSK1 &= ~(1 << OCIE1A);
}

ISR(TIMER1_OVF_vect)
{
  PORTB &= ~(1 << PB1);

  TCCR1B = 0x00;

  TIMSK1 |= (1 << OCIE1A);
}

atmega328_16mhz_ac_phase_controlClass atmega328_16mhz_ac_phase_control;