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
#include <avr/io.h>

#define EXTENDER_SLAVE_ADRESS 0x70
#define F_SENSOR_SLAVE_ADRESS 0xE0
#define B_SENSOR_SLAVE_ADRESS 0xEA

/*
 * Initialize 
 */
void I2C_init(void);

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
 * Returns true if the specified button is pressed. 
 */ 	
bool button_pressed(void);

/*
 * Returns true if a collision is detected on the right side
 */
bool right_collision();

/*
 * Returns true if a collision is detected on the left side
 */
bool left_collision();


/*
 * Return true if the front distance sensor is within 15cm of an object. 
 */
uint16_t front_distance(void); 

/*
 * Return true if the back distance sensor is within 15cm of an object. 
 */
uint16_t back_distance(void);

#endif
