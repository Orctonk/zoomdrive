#include "summer.h"
#include <util/delay.h>

/*
 *Initializes the device.
 */
void summer_init(){
    SUMMER_DDR |= (1<< SUMMER_PIN);
    TCCR0B = 0;
    OCR0A = 6; 
    OCR0B = 6;
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
