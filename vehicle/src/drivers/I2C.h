/*
 * I2C.h
 * 
 * Date: 2020-05-115
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */

#ifndef I2C_H
#define I2C_H

#include <stdbool.h>

#define EXTENDER_SLAVE_ADRESS 0x07
#define F_SENSOR_SLAVE_ADRESS 0xE0
#define B_SENSOR_SLAVE_ADRESS 0xEA

/*
 * Initialize 
 */
void I2C_init();

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
 * Return true if the front distance sensor is within 15cm of an object. 
 */
bool front_within_15cm(); 

/*
 * Return true if the back distance sensor is within 15cm of an object. 
 */
bool back_within_15com();

#endif
