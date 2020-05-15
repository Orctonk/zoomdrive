/*
 * engine.h
 * 
 * Date: 2020-05-11
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */

#ifndef ENGINE_H
#define ENGINE_H

#include <avr/io.h>

#define PWM_DDR DDRD
#define EN_DDR DDRD
//Right wheel
#define RIGHT_PWM_PIN PIN6
#define RIGHT_EN_PIN PIN2
#define RIGHT_IN1 PIN6
#define RIGHT_IN2 PIN7
//Left wheel
#define LEFT_PWM_PIN PIN5
#define LEFT_EN_PIN PIN3
#define LEFT_IN1 PIN7
#define LEFT_IN2 PIN0

/*
 * Initialize motor
 */
void engine_init();

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