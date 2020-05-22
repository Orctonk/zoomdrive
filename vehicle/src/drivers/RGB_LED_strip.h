/*
 * RGB_LED_strip.h
 * 
 * Date: 2020-05-11
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */

#include <avr/io.h>
#include <stdbool.h>

#define STRIP_DDR DDRD
#define STRIP_PORT PORTD 
#define STRIP_PIN PIN4

/*
 * Initialize 
 */
void LED_strip_init(void);

/*
 * Turn on or off the right turn signal.
 * 
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */
void turn_signal_right(bool lamp_switch);

/*
 * Turn on or off the left turn signal.
 * 
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */     
void turn_signal_left(bool lamp_switch); 