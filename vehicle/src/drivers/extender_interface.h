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

#define SLAVE_ADRESS 0x07
/*
 * Initialize 
 */
void init();

/*
 * Turn on or off the green lamp.
 * 
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */
void green_lamp(bool lamp_switch);

/*
 * Turn on or off the green lamp.
 * 
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */
void yellow_lamp(bool lamp_switch); 

/*
 * Return true if the distance sensors are within 15cm of an object. 
 */
bool within_15cm(); 


#endif
