/*
 * Summer.c
 *
 * Created: 2020-05-12
 * Author: Mimmi
 */ 

#include "summer.h"
#include <util/delay.h>

/*
 *Initializes the device.
 */
void summer_init(){
	SUMMER_DDR |= (1<< SUMMER_PIN);
	TCCR1B = (1<<WGM12);
	OCR1A = 48; 
	OCR1B = 48;
	TCCR1A = (1<<COM1A0);
	
	
}

/*
 *Will indicate to the user that the the reading is happening.
 */
void summer_start(){
	TCCR1B |= (1<< CS12); 
}

/*
 *Will indicate to the user when a reading is not happening.
 */
void summer_stop(){
	TCCR1B &= ~(1<< CS12); 
}

/*
 * The buzzer beeps two times.
 */
void summer_beep(){
	summer_stop();
	_delay_ms(500);
	summer_start();
	_delay_ms(500);
	summer_stop();
	_delay_ms(500);
	summer_start();
	_delay_ms(500);
	summer_stop();

}