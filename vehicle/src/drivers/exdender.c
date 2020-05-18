/*
 * extender.c
 * 
 * Date: 2020-05-115
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */

#include "extender_interface.h"
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
    if(lamp_switch){
        if( i2c_start((SLAVE_ADRESS<<1)|I2C_WRITE ) != 0){
			i2c_stop();
			return 0;
		}

		if( i2c_write(2) != 0) {
			i2c_stop();
			return 0;
		}
    }
}

/*
 * Turn on or off the green lamp.
 * 
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */
void yellow_lamp(bool lamp_switch){
    if(lamp_switch){
        if( i2c_start((SLAVE_ADRESS<<1)|I2C_WRITE ) != 0){
			i2c_stop();
			return 0;
		}

		if( i2c_write(1) != 0) {
			i2c_stop();
			return 0;
		}
    }
}

/*
 * Return true if the distance sensors are within 15cm of an object. 
 */
bool within_15cm(); 

