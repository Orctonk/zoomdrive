/*
 * I2C.c
 * 
 * Date: 2020-05-115
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */
#include <avr/io.h>

#include "I2C.h"
#include "i2cmaster.h"

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
	if( i2c_start((EXTENDER_SLAVE_ADRESS<<1)|I2C_READ ) != 0){
		i2c_stop();
		return 0;
	}
	uint8_t data = i2c_readAck(); 
	if(lamp_switch){
		data |= (1<< PIN1);
	}
	else {
		data &= ~(1<< PIN1);
	}

	if( i2c_rep_start((EXTENDER_SLAVE_ADRESS<<1)|I2C_WRITE ) != 0){
		i2c_stop();
		return 0;
	}
	if( i2c_write(data) != 0) {
		i2c_stop();
		return 0;
	}
}

/*
 * Turn on or off the green lamp.
 * 
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */
void yellow_lamp(bool lamp_switch){
    if( i2c_start((EXTENDER_SLAVE_ADRESS<<1)|I2C_READ ) != 0){
		i2c_stop();
		return 0;
	}
	uint8_t data = i2c_readAck(); 
	if(lamp_switch){
		data |= (1<< PIN0);
	}
	else {
		data &= ~(1<< PIN0);
	}

	if( i2c_rep_start((EXTENDER_SLAVE_ADRESS<<1)|I2C_WRITE ) != 0){
		i2c_stop();
		return 0;
	}
	if( i2c_write(data) != 0) {
		i2c_stop();
		return 0;
	}
}

/*
 *
 */ 	
void button_pressed(void){
if( i2c_start((EXTENDER_SLAVE_ADRESS<<1)|I2C_READ ) != 0){
		i2c_stop();
		return 0;
	}
	uint8_t data = i2c_readAck(); 
		// |= (1<< PIN5);				Är knappen hög? 



}

uint16_t front_distance(void){
	if( i2c_start((F_SENSOR_SLAVE_ADRESS)|I2C_WRITE ) != 0){
		i2c_stop();
		return 0;
	}
	if( i2c_write(0x51) != 0) {
		i2c_stop();
		return 0;
	}
	if( i2c_rep_start((F_SENSOR_SLAVE_ADRESS)|I2C_READ ) != 0){
		i2c_stop();
		return 0;
	}
	uint16_t distance = (i2c_readAck()<<8);
	distance |= i2c_readAck();
	i2c_stop();

	return distance; 
}


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

