/*
 * timer.h
 *
 * Created: 2020-05-12
 * Author: Mimmi
 */ 


#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

/*
 * Initialize timer
 */
void timer_init(); 

/*
 * Returns the time since start up.
 *
 *Returns a struct containing hours, minutes and seconds  
 */
uint32_t timer_get_time(); 



#endif /* TIMER_H_ */