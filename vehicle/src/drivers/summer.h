/*
 * summer.h
 * 
 * Date: 2020-05-11
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */


#ifndef SUMMER_H_
#define SUMMER_H_

#include <avr/io.h>

#define SUMMER_DDR DDRB
#define SUMMER_PIN PIN1

/*
 *Initializes the device.
 */
void summer_init(void);

/*
 * Start
 */
void summer_start(void);

/*
 * Stop
 */
void summer_stop(void);

/*
 * The buzzer beeps two times.
 */
void summer_beep(void); 

#endif /* SUMMER_H_ */