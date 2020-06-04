/*
 *  FILE:
 *      I2C.c
 * 
 *  DESCRIPTION: 
 *      Components using the I2C interface. Contains two LEDs, one button, two haptic sensors
 * 		and two distance sensors. 
 * 
 *  AUTHOR:
 *      Mimmi Edholm 
 * 
 */

#include "I2C.h"
#include "i2cmaster.h"

#include <util/delay.h>

#include "LCD.h"
#include <stdlib.h>
/*
 * Initialize 
 */
void I2C_init(void){
    i2c_init();

	if( i2c_start((EXTENDER_SLAVE_ADRESS)| I2C_WRITE) !=0){
		i2c_stop();
		return 0; 
	}
	i2c_write(0xFF);
	i2c_stop();

}

/*
 * Turn on or off the green lamp.
 * 
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */
void green_lamp(bool lamp_switch){
	
	if( i2c_start((EXTENDER_SLAVE_ADRESS)|I2C_READ ) != 0){
		i2c_stop();
		return;
	}
	//Read current and apply new command.
	uint8_t data = i2c_readAck(); 
	if(lamp_switch){
		data &= ~(1<< PIN1);
	}
	else {
		data |= (1<< PIN1);
	}
	
	if( i2c_rep_start((EXTENDER_SLAVE_ADRESS)|I2C_WRITE ) != 0){
		i2c_stop();
		return;
	}
	if( i2c_write(data) != 0) {
		i2c_stop();
		return;
	}
	if( i2c_write(data) != 0) {
		i2c_stop();
		return;
	}
	i2c_stop();
}

/*
 * Turn on or off the green lamp.
 * 
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */
void yellow_lamp(bool lamp_switch){
    if( i2c_start((EXTENDER_SLAVE_ADRESS)|I2C_READ ) != 0){
		i2c_stop();
		return;
	}
	//Read current and apply new command.
	uint8_t data = i2c_readAck(); 
	if(lamp_switch){
		data &= ~(1<< PIN0);
	}
	else {
		data |= (1<< PIN0);
	}

	if( i2c_rep_start((EXTENDER_SLAVE_ADRESS)|I2C_WRITE ) != 0){
		i2c_stop();
		return;
	}
	if( i2c_write(data) != 0) {
		i2c_stop();
		return;
	}
	if( i2c_write(data) != 0) {
		i2c_stop();
		return;
	}
	i2c_stop();
}

/*
 * Returns true if the specified button is pressed. 
 */ 	
bool button_pressed(void){
	if( i2c_start((EXTENDER_SLAVE_ADRESS)|I2C_READ ) != 0){
		i2c_stop();
		return 1;
	}
	uint8_t read = i2c_readAck();
	uint8_t ret_read = read & (1<<PIN4);

	if( i2c_rep_start((EXTENDER_SLAVE_ADRESS)| I2C_WRITE) !=0){
		i2c_stop();
		return 0; 
	}

	read |= (1<< PIN4);
	i2c_write(read);
	i2c_stop();
	return !ret_read; 
}

/*
 * Returns true if a collision is detected on the right side
 */
bool right_collision(void){
	if( i2c_start((EXTENDER_SLAVE_ADRESS)|I2C_READ ) != 0){
		i2c_stop();
		return 1;
	}
	uint8_t read = i2c_readAck();
	uint8_t ret_read = read & (1<<PIN2);

	if( i2c_rep_start((EXTENDER_SLAVE_ADRESS)| I2C_WRITE) !=0){
		i2c_stop();
		return 0; 
	}

	read |= (1<< PIN2);
	i2c_write(read);
	i2c_stop();
	return !ret_read;
}

/*
 * Returns true if a collision is detected on the left side
 */
bool left_collision(void){
	if( i2c_start((EXTENDER_SLAVE_ADRESS)|I2C_READ ) != 0){
		i2c_stop();
		return 1;
	}
	uint8_t read = i2c_readAck();
	uint8_t ret_read = read & (1<<PIN3);

	if( i2c_rep_start((EXTENDER_SLAVE_ADRESS)| I2C_WRITE) !=0){
		i2c_stop();
		return 0; 
	}

	read |= (1<< PIN3);
	i2c_write(read);
	i2c_stop();
	return !ret_read;
}


/*
 * Checks the distance to an object from the distance sensor in the front.
 * 
 * Returns: The distance in meters.  
 */
uint16_t front_distance(void){
	if (i2c_start((F_SENSOR_SLAVE_ADRESS)|I2C_WRITE ) != 0){
		i2c_stop();
		return 0;
	}
	i2c_write(0x00);
	i2c_write(0x51);
	i2c_stop();

	_delay_ms(70);

	if(i2c_start((F_SENSOR_SLAVE_ADRESS)|I2C_WRITE) != 0){
		i2c_stop();
		return 0;
	}
	i2c_write(0x02);
	if(i2c_rep_start((F_SENSOR_SLAVE_ADRESS)|I2C_READ)){
		i2c_stop();
		return 0;
	}
	uint16_t distance = i2c_readAck() << 8;
	distance |= i2c_readNak();
	i2c_stop();

	return distance; 
}

/*
 * Checks the distance to an object from the distance sensor in the back.
 * 
 * Returns: The distance in meters.  
 */
uint16_t back_distance(void){
	if (i2c_start((B_SENSOR_SLAVE_ADRESS)|I2C_WRITE ) != 0){
		i2c_stop();
		return 0;
	}
	i2c_write(0x00);
	i2c_write(0x51);
	i2c_stop();

	_delay_ms(70);

	if(i2c_start((B_SENSOR_SLAVE_ADRESS)|I2C_WRITE) != 0){
		i2c_stop();
		return 0;
	}
	i2c_write(0x02);
	if(i2c_rep_start((B_SENSOR_SLAVE_ADRESS)|I2C_READ)){
		i2c_stop();
		return 0;
	}
	uint16_t distance = i2c_readAck() << 8;
	distance |= i2c_readNak();
	i2c_stop();

	return distance; 
}

