/*
 *  FILE:
 *      I2C.h
 * 
 *  DESCRIPTION: 
 *      Interface for the components using an I2C interface. Contains two LEDs, 
 *      one button, two haptic sensors and two distance sensors.
 * 
 *  AUTHOR:
 *      Mimmi Edholm 
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
bool right_collision(void);

/*
 * Returns true if a collision is detected on the left side
 */
bool left_collision(void);


/*
 * Checks the distance to an object from the distance sensor.
 * 
 * Returns: The distance in meters.  
 */
uint16_t front_distance(void); 

/*
 * Checks the distance to an object from the distance sensor.
 * 
 * Returns: The distance in meters.  
 */
uint16_t back_distance(void);

#endif
