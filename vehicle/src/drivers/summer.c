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
	TCCR0B = 0;
	OCR0A = 48; 
	OCR0B = 48;
	TCCR0A = (1<<COM0B0)|(1<<WGM01);
	
}

/*
 *Will indicate to the user that the the reading is happening.
 */
void summer_start(){
	TCCR0B = (1<< CS02); 
}

/*
 *Will indicate to the user when a reading is not happening.
 */
void summer_stop(){
	TCCR0B = 0; 
}

/*
 *Will indicate to the user that the reading is complete.
 */
void summer_read_complete(){
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