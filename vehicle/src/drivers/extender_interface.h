/*
 * extender_interface.h
 * 
 * Date: 2020-05-115
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */

#ifndef EXTENDER_INTERFACE_H
#define EEXTENDER_INTERFACE_H

#include <stdbool.h>

// #define EX_PORT PORTC
// #define SDA_PIN PIN4
// #define SCL_PIN PIN5

/*
 * Initialize 
 */
void init();

/*
 * Turn on or off the green lamp.
 * 
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */
void green_lamp(int lamp_switch);

/*
 * Turn on or off the green lamp.
 * 
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */
void yellow_lamp(int lamp_switch); 

/*
 * Return true if the distance sensors are within 15cm of an object. 
 */
bool within_15cm(); 

/*
 * Turn on or off the right turn signal.
 * 
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */
void turn_signal_right(int lamp_switch);

/*
 * Turn on or off the left turn signal.
 * 
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */     
void turn_signal_left(int lamp_switch); 


#endif
