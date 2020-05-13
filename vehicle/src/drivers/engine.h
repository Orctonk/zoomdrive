/*
 * engine.h
 * 
 * Date: 2020-05-11
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */

#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>

//#define 

/*
 * Set speed of viecle.
 * 
 * speed: 1 if moving forward, -1 if moving backwards, 0 if standing still. 
 */
void engine_set_speed(int8_t speed);

/*
 * Make the viecle turn.
 * 
 * degree: 0 if forward, 1 if right, -1 if left.
 */
void engine_turn(int8_t degree);

#endif 