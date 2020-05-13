/*
 * timer.c
 *
 * Created: 2020-05-05 10:55:11
 *  Author: Mimmi
 */ 

#include <avr/interrupt.h>

#include "timer.h"

static volatile uint32_t ticks = 0; 

/*
 * Updates the internal timer at the frequency of 100 Hz.  
 */
ISR(TIMER2_COMPA_vect){
	ticks ++;
}
	
/*
 * Initialize timer
 */
void timer_init(){
	OCR2A = 255; 
	TCCR2A = (1<<WGM21);
	TCCR2B = (1<<CS22)|(1<<CS21)|(1<<CS20);
	TIMSK2 = (1<<OCIE2A);
	
}
	
/*
 * Returns the time since start up.
 *
 * Returns a struct containing hours, minutes and seconds  
 */
uint32_t timer_get_time(){

	uint32_t current = ticks / (F_CPU/1024/(255 + 1)); 
	
	return current; 
	}