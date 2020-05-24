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

int duty_cycle = 150; 
int speed = 0; 
int degree = 0;

/*
 * Set speed for right wheel
 */
void right_wheel(int right){
    if(right == 1){
        OCR0A = duty_cycle;
        PORTB |= (1<< RIGHT_IN1);
        PORTB &= ~(1<< RIGHT_IN2);
    }
    else if(right == -1){
        OCR0A = duty_cycle;
        PORTB |= (1<< RIGHT_IN2);
        PORTB &= ~(1<< RIGHT_IN1);
    }
    else if (right == 0){  
        OCR0A = 0;
    }
}

/*
 * Set speed for left wheel
 */
void left_wheel(int left){
    if(left == 1){
        OCR0B = duty_cycle;
        PORTD &= ~(1<< LEFT_IN1);
        PORTB |= (1<< LEFT_IN2);
    }
    else if(left == -1){
        OCR0B = duty_cycle;
        PORTD |= (1<< LEFT_IN1);
        PORTB &= ~(1<< LEFT_IN2);
    }
    else if (left == 0){
        OCR0B = 0; 
    }
}    

/*
 * Decides in what direction each wheel should go.
 */
void recalc_engine(void){
    if(degree == 1){
        if(speed == 0){
            right_wheel(1);
            left_wheel(-1);
        }
        else {
            right_wheel(0);
            left_wheel(speed);
        }  
    } 
    else if (degree == -1){
        if(speed == 0){
            right_wheel(-1);
            left_wheel(1);
        }
        else {
            right_wheel(speed);
            left_wheel(0);
        }
    }
     else{
        right_wheel(speed);
        left_wheel(speed);
    }
}

/*
 * Initialize motor
 */
void engine_init(void){
    PWM_DDR |= (1<< RIGHT_PWM_PIN)|(1<<LEFT_PWM_PIN);

    DDRB |= (1<< RIGHT_IN1);
    DDRB |= (1<< RIGHT_IN2);
    DDRD |= (1<< LEFT_IN1);
    DDRB |= (1<< LEFT_IN2);

    PORTB |= (1<< RIGHT_IN1);
    PORTB &= ~(1<< RIGHT_IN2);
    PORTD |= (1<< LEFT_IN1);
    PORTB &= ~(1<< LEFT_IN2);


	OCR0A = 0; 
    OCR0B = 0; 
    TCCR0B = (1<<CS01);
	TCCR0A = (1<<WGM01)|(1<<WGM00)|(1<<COM0A1)|(1<<COM0B1);

    recalc_engine();
}

/*
 * Set speed of 
 * 
 * new_speed: 1 if moving forward, -1 if moving backwards, 0 if standing still. 
 */
void engine_set_speed(int8_t new_speed){
    speed = new_speed;
    recalc_engine();
}

/*
 * Make the viecle turn.
 * 
 * degree: 0 if forward, 1 if right, -1 if left.
 */
void engine_turn(int8_t new_degree){
    degree = new_degree;
    recalc_engine();
}

/*
* Get the speed of the vheicle.  
* 
* Return: 1, if moving forward normal speed. -1 if moving backwards. 0 if standing still.
*/
int engine_get_speed(void){
    return speed; 
}

/*
 * Get the heading of the vheicle.  
 * 
 * Return: 1, if moving right normal speed. -1 if moving left. 0 if standing still.
 */
int engine_get_heading(void){
    return degree;
}

