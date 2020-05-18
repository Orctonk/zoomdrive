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
void init(){
    i2c_init();

}

//THe fuk funkar knappen?? 

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
 * Return true if the distance sensors are within 15cm of an object. 
 */
bool within_15cm(){

	// SRF02

	// Skriv till plats 0 
	// 0x51 

	//Läs från 2 och 3, cm

	return false; 
}

