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

int duty_cicle = 200; 
int speed = 0; 
int degree = 0;

/*
 *
 */
void right_wheel(int right){
    if(right == 1){
        TCCR0B |= (1<< CS02);

        PORTB |= (1<< RIGHT_IN1);
        PORTB &= ~(1<< RIGHT_IN2);
         
    }
    else if(right == -1){
        TCCR0B |= (1<< CS02);

        PORTB |= (1<< RIGHT_IN2);
        PORTB &= ~(1<< RIGHT_IN1);
    }
    else if (right == 0){
        TCCR0B &= ~(1<< CS02);
        
    }
}

/*
 *
 */
void left_wheel(int left){
    if(left == 1){
        TCCR0B |= (1<< CS02);

        PORTD |= (1<< LEFT_IN1);
        PORTB &= ~(1<< LEFT_IN2);
         
    }
    else if(left == -1){
        TCCR0B |= (1<< CS02);

        PORTD &= ~ (1<< LEFT_IN1);
        PORTB |= (1<< LEFT_IN2);
    }
    else if (left == 0){
        TCCR0B &= ~(1<< CS02);
        
    }
}    


/*
 * Initialize motor
 */
void engine_init(){
    PWM_DDR |= (1<< RIGHT_PWM_PIN);
	TCCR0B = (1<< WGM02);
	OCR0A = duty_cicle; 
    OCR0B = duty_cicle; 
	TCCR0A = (1<<COM0A0)|(1<<WGM00)|(1<<COM0B0);

    DDRB |= (1<< RIGHT_IN1);
    DDRB |= (1<< RIGHT_IN2);
    DDRD |= (1<< LEFT_IN1);
    DDRB |= (1<< LEFT_IN2);
}

/*
 * Set speed of viecle.
 * 
 * new_speed: 1 if moving forward, -1 if moving backwards, 0 if standing still. 
 */
void engine_set_speed(int8_t new_speed){
    speed = new_speed;

    right_wheel(speed);
    left_wheel(speed); 
    // if(speed == 1){
    //     TCCR0B |= (1<< CS02);

    //     PORTB |= (1<< RIGHT_IN1);
    //     PORTB &= ~(1<< RIGHT_IN2);

    //     PORTD |= (1<< LEFT_IN1);
    //     PORTB &= ~(1<< LEFT_IN2);
         
    // }
    // else if(speed == -1){
    //     TCCR0B |= (1<< CS02);

    //     PORTB |= (1<< RIGHT_IN2);
    //     PORTB &= ~(1<< RIGHT_IN1);

    //     PORTD &= ~ (1<< LEFT_IN1);
    //     PORTB |= (1<< LEFT_IN2);
    // }
    // else if (speed == 0){
    //     TCCR0B &= ~(1<< CS02);
        
    // }
}

/*
 * Make the viecle turn.
 * 
 * degree: 0 if forward, 1 if right, -1 if left.
 */
void engine_turn(int8_t new_degree){
    int degree = new_degree;
    if(degree == 1){
        // OCR0A = 0; 
        // OCR0B = duty_cicle; 
        // TCCR0B |= (1<< CS02);

        // PORTD |= (1<< LEFT_IN1);
        // PORTB &= ~(1<< LEFT_IN2);

        right_wheel(0);
        left_wheel(1);
    }
    else if(degree == -1){
        // OCR0A = duty_cicle; 
        // OCR0B = 0; 
        // TCCR0B |= (1<< CS02);

        // PORTB |= (1<< RIGHT_IN1);
        // PORTB &= ~(1<< RIGHT_IN2);
        left_wheel(1);
        right_wheel(0);
    }

    else if(degree == 0){
        // OCR0A = duty_cicle; 
        // OCR0B = duty_cicle; 
        engine_set_speed(speed);
    }
}

/*
* Get the speed of the vheicle.  
* 
* Return: 1, if moving forward normal speed. -1 if moving backwards. 0 if standing still.
*/
int engine_get_speed(){
    return speed; 
}
