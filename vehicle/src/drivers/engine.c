/*
 * engine.c
 * 
 * Date: 2020-05-12
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */

#include "engine.h"

int speed = 200; 

/*
 * Initialize motor
 */
void engine_init(){
    PWM_DDR |= (1<< RIGHT_PWM_PIN);
	TCCR0B = (1<< WGM02);
	OCR0A = speed; 
    OCR0B = speed; 
	TCCR0A = (1<<COM0A0)|(1<<WGM00)|(1<<COM0B0);

    DDRB |= (1<< RIGHT_IN1);
    DDRB |= (1<< RIGHT_IN2);
    DDRD |= (1<< LEFT_IN1);
    DDRB |= (1<< LEFT_IN2);
}

/*
 * Set speed of viecle.
 * 
 * speed: 1 if moving forward, -1 if moving backwards, 0 if standing still. 
 */
void engine_set_speed(int8_t power){
    
    if(power == 1){
        TCCR0B |= (1<< CS02);

        PORTB |= (1<< RIGHT_IN1);
        PORTB &= ~(1<< RIGHT_IN2);

        PORTD |= (1<< LEFT_IN1);
        PORTB &= ~(1<< LEFT_IN2);
         
    }
    else if(power == -1){
        TCCR0B |= (1<< CS02);

        PORTB |= (1<< RIGHT_IN2);
        PORTB &= ~(1<< RIGHT_IN1);

        PORTD &= ~ (1<< LEFT_IN1);
        PORTB |= (1<< LEFT_IN2);
    }
    else if (power == 0){
        TCCR0B &= ~(1<< CS02);
    }
}

/*
 * Make the viecle turn.
 * 
 * degree: 0 if forward, 1 if right, -1 if left.
 */
void engine_turn(int8_t degree){
    if(degree == 1){
        OCR0A = 0; 
        OCR0B = speed; 
        TCCR0B |= (1<< CS02);

        PORTD |= (1<< LEFT_IN1);
        PORTB &= ~(1<< LEFT_IN2);

    }
    else if(degree == -1){
        OCR0A = speed; 
        OCR0B = 0; 
        TCCR0B |= (1<< CS02);

        PORTB |= (1<< RIGHT_IN1);
        PORTB &= ~(1<< RIGHT_IN2);
    }
    else {
        OCR0A = speed; 
        OCR0B = speed; 
    }
}