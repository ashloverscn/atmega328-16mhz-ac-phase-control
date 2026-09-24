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

void atmega328_16mhz_ac_phase_controlClass::init()
{
  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);

  DDRB |= (1 << PB1);
  PORTB &= ~(1 << PB1);

  TIMSK1 = 0x02;
  TCCR1A = 0x00;
  TCCR1B = 0x00;
  TCNT1 = 0x00;

  EICRA &= ~((1 << ISC01) | (1 << ISC00));
  EICRA |= (1 << ISC01) | (1 << ISC00);

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

  OCR1A = value;
}

ISR(INT0_vect)
{
  PORTB &= ~(1 << PB1);

  TCCR1B = 0x00;
  TCNT1 = 0x00;

  TCCR1B = 0x04;
}

ISR(TIMER1_COMPA_vect)
{
  PORTB |= (1 << PB1);

  TCNT1 = 0x00;
  OCR1A = TRIAC_PULSE_TICKS;

  TCCR1B = 0x04;
}

ISR(TIMER1_OVF_vect)
{
  PORTB &= ~(1 << PB1);
  TCCR1B = 0x00;
}

atmega328_16mhz_ac_phase_controlClass atmega328_16mhz_ac_phase_control;