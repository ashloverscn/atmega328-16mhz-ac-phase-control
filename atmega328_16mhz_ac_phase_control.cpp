/*
  Professional AC Phase Control Library for ATmega328 / 168 (16MHz)
  Uses Timer1 with Prescaler = 8 (1 tick = 0.5 microseconds)
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>
#include "atmega328_16mhz_ac_phase_control.h"

// Hardware Timer Limits (16MHz clock, prescaler 8 -> 2MHz)
// 10ms half-wave = 20,000 ticks total max
#define MIN_TICK_DELAY 500   // 100% Power (250µs delay)
#define MAX_TICK_DELAY 19500 // 0% Power (9.75ms delay / OFF)

static volatile unsigned int current_phase_delay = MAX_TICK_DELAY;

void atmega328_16mhz_ac_phase_controlClass::init()
{
  // Zero-Cross pin (PD2 / INT0) as input with pullup
  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);

  // TRIAC gate pin (PB1 / Digital Pin 9) as output, initially LOW
  DDRB |= (1 << PB1);
  PORTB &= ~(1 << PB1);

  // --- Setup Timer1 in Normal Mode ---
  noInterrupts();
  TCCR1A = 0; 
  TCCR1B = 0;
  TCNT1  = 0; 

  // Set Prescaler to 8 -> 16MHz / 8 = 2MHz (0.5us per tick)
  TCCR1B |= (1 << CS11);
  
  // Configure INT0 for FALLING edge interrupt (Zero-Crossing)
  EICRA &= ~((1 << ISC01) | (1 << ISC00));
  EICRA |= (1 << ISC01); 
  EIMSK |= (1 << INT0);

  // Disable match interrupt initially until zero-cross triggers it
  TIMSK1 &= ~(1 << OCIE1A);
  interrupts();
}

void atmega328_16mhz_ac_phase_controlClass::uninit()
{
  noInterrupts();
  TIMSK1 &= ~(1 << OCIE1A);
  EIMSK &= ~(1 << INT0);
  PORTB &= ~(1 << PB1);
  
  DDRD &= ~(1 << PD2);
  PORTD &= ~(1 << PD2);
  DDRB &= ~(1 << PB1);
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  interrupts();
}

void atmega328_16mhz_ac_phase_controlClass::set_ac_power(int value)
{
  if (value < MIN_TICK_DELAY)
  {
    value = MIN_TICK_DELAY;
  }
  if (value > MAX_TICK_DELAY)
  {
    value = MAX_TICK_DELAY;
  }

  // ATOMIC WRITE: Protect 16-bit variable updates from race conditions
  cli();
  current_phase_delay = value;
  sei();
}

// --- Zero-Crossing External Interrupt (Pin D2 / INT0) ---
ISR(INT0_vect)
{
  TCNT1 = 0; // Hard reset timer to synchronize with the AC wave
  
  // ATOMIC READ: Safely grab the 16-bit delay value
  cli();
  unsigned int local_delay = current_phase_delay;
  sei();
  
  OCR1A = local_delay;
  TIFR1 |= (1 << OCF1A);   // Clear pending compare match flag
  TIMSK1 |= (1 << OCIE1A); // Enable match interrupt for this half-wave
}

// --- Timer1 Compare Match Interrupt (Triggers TRIAC Firing) ---
ISR(TIMER1_COMPA_vect)
{
  TIMSK1 &= ~(1 << OCIE1A); // Disable further interrupts until next zero-cross

  // ATOMIC READ for comparison
  cli();
  unsigned int local_delay = current_phase_delay;
  sei();

  // Only fire the TRIAC if we are safely within the half-wave window
  if (local_delay < 19000) {
    PORTB |= (1 << PB1);  // TRIAC HIGH
    
    // Precise ~20µs gate pulse using NOP assembly instructions
    for(uint8_t i = 0; i < 16; i++) {
        __asm__("nop\n\t");
    }
    
    PORTB &= ~(1 << PB1); // TRIAC LOW
  } else {
    // 0% power / fully OFF: ensure pin stays LOW
    PORTB &= ~(1 << PB1);
  }
}

atmega328_16mhz_ac_phase_controlClass atmega328_16mhz_ac_phase_control;