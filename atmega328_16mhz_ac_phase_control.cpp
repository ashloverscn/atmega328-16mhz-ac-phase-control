/*
this library is intended to be use with atmega328 and 168
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>
#include "atmega328_16mhz_ac_phase_control.h"


 void atmega328_16mhz_ac_phase_controlClass::init()
{
  //setup input and output
  
  DDRD &= ~(1 << PD2); //INT0 pin as input for zero cross detect 
  PORTD |= (1 << PD2); //enable pull-up resistor for PD2
  DDRB |= (1 << PB1); //triac gate control pin as output
  
  //setup timer
  
  // set up Timer1 
  //(see ATMEGA 328 data sheet pg 134 for more details)
  //OCR1A = 100;      //initialize the comparator
  TIMSK1 = 0x03;    //enable comparator A and overflow interrupts
  TCCR1A = 0x00;    //timer control registers set for
  TCCR1B = 0x00;    //normal operation, timer disabled

  //setup interrupt

  //my circuit gives active high pulse for zcd that is ZCD = High or 5v 
  
  //PD2 INT0 set up zero crossing interrupt on RISING edge
  EICRA |= (1 << ISC01);EICRA |= (1 << ISC00);
  EIMSK |= (1 << INT0);
  //enable global interrupt
  sei();
}

 void atmega328_16mhz_ac_phase_controlClass::uninit()
{
	//set default port registers
	DDRD = 0x00;
	PORTD = 0x00;
	DDRB = 0x00;
	PORTB = 0x00;
    //clear timer control register
	TIMSK1 = 0x00;  
	TCCR1A = 0x00;  
	TCCR1B = 0x00; 
	TCNT1 = 0x00;
    //clear interrupt register
	EICRA = 0x00;
	EIMSK = 0x00;
	//clear global interrupt
	cli();
}
 void atmega328_16mhz_ac_phase_controlClass::set_ac_power(int value)
 {
	OCR1A = value;//speed[0 to 10] or 1 to 623 	 
 }

 ISR(INT0_vect){ //zero cross detect   
  PORTB &= ~(1 << PB1);//turn off triac gate
  TCNT1 = 0x00;   //reset timer count back to zero
  TCCR1B=0x04; //start timer with divide by 256 input  
}

 ISR(TIMER1_COMPA_vect){ //comparator match
  PORTB |= (1 << PB1);//set triac gate to high
}

atmega328_16mhz_ac_phase_controlClass atmega328_16mhz_ac_phase_control;
