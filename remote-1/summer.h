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

#define SUMMER_DDR DDRD
#define SUMMER_PIN PIN5

/*
 *Initializes the device.
 */
void summer_init();

/*
 * Start
 */
void summer_start();

/*
 * Stop
 */
void summer_stop();

/*
 * The buzzer beeps two times.
 */
void summer_beep(); 

#endif /* SUMMER_H_ */
