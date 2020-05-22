/*
 * I2C.c
 * 
 * Date: 2020-05-115
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */

#include "I2C.h"
#include "i2cmaster.h"

#include <util/delay.h>

/*
 * Initialize 
 */
void I2C_init(void){
    i2c_init();

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
		data |= (1<< PIN1);
	}
	else {
		data &= ~(1<< PIN1);
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
		data |= (1<< PIN0);
	}
	else {
		data &= ~(1<< PIN0);
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
		return;
	}
	uint8_t read = i2c_readAck();
	read &= (1<<PIN5);
	i2c_stop();

	return read; 
}

/*
 * Return true if the front distance sensor is within 15cm of an object. 
 */
uint16_t front_distance(void){
	i2c_start((F_SENSOR_SLAVE_ADRESS)|I2C_WRITE );
	i2c_write(0x00);
	i2c_write(0x51);
	i2c_stop();

	_delay_ms(70);

	i2c_start((F_SENSOR_SLAVE_ADRESS)|I2C_WRITE);
	i2c_write(0x02);
	i2c_rep_start((F_SENSOR_SLAVE_ADRESS)|I2C_READ);
	uint16_t distance = i2c_readAck() << 8;
	distance |= i2c_readAck();
	i2c_stop();

	return distance; 
}

/*
 * Return true if the back distance sensor is within 15cm of an object. 
 */
uint16_t back_distance(void){
	if( i2c_start((B_SENSOR_SLAVE_ADRESS)|I2C_WRITE ) != 0){
		i2c_stop();
		return 0;
	}
	if( i2c_write(0x51) != 0) {
		i2c_stop();
		return 0;
	}
	if( i2c_rep_start((B_SENSOR_SLAVE_ADRESS)|I2C_READ ) != 0){
		i2c_stop();
		return 0;
	}
	uint16_t distance = (i2c_readAck()<<8);
	distance |= i2c_readAck();
	i2c_stop();

	return distance; 
}

