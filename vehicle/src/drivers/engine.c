/*
 * engine.c
 * 
 * Date: 2020-05-12
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */

#include <avr/interrupt.h>

#include "engine.h"

int duty_cycle = 100; 
int back_duty_cycle = 100;
int direction = 0; 
int degree = 0;
int gear = 1;
static volatile uint32_t right_en_ticks = 0;
static volatile uint32_t left_en_ticks = 0;

ISR(INT0_vect){
    right_en_ticks ++;
}
ISR(INT1_vect){
    left_en_ticks ++;
}

/*
 * Set direction for right wheel
 */
void right_wheel(int right){
    if(right == 1){
        OCR0A = duty_cycle;
        PORTB |= (1<< RIGHT_IN1);
        PORTB &= ~(1<< RIGHT_IN2);
    }
    else if(right == -1){
        OCR0A = back_duty_cycle;
        PORTB |= (1<< RIGHT_IN2);
        PORTB &= ~(1<< RIGHT_IN1);
    }
    else if (right == 0){  
        OCR0A = 0;
    }
}

/*
 * Set direction for left wheel
 */
void left_wheel(int left){
    if(left >= 1){
        OCR0B = duty_cycle;
        PORTD &= ~(1<< LEFT_IN1);
        PORTB |= (1<< LEFT_IN2);
    }
    else if(left == -1){
        OCR0B = back_duty_cycle;
        PORTD |= (1<< LEFT_IN1);
        PORTB &= ~(1<< LEFT_IN2);
    }
    else if (left == 0){
        OCR0B = 0; 
    }
}    

/*
 * Decides in what direction and speed each wheel should go.
 */
void recalc_engine(void){
    // Set speed according to gear. 
    if (gear == 1){
        duty_cycle = 100;
    }
    else if(gear == 2){
        duty_cycle = 150;
    }
    else if(gear == 3){
        duty_cycle = 250;
    }

    if(degree == 1){
        if(direction == 0){
            right_wheel(1);
            left_wheel(-1);
        }
        else {
            right_wheel(0);
            left_wheel(direction);
        }  
    } 
    else if (degree == -1){
        if(direction == 0){
            right_wheel(-1);
            left_wheel(1);
        }
        else {
            right_wheel(direction);
            left_wheel(0);
        }
    }
     else{
        right_wheel(direction);
        left_wheel(direction);
    }
}

/*
 * Initialize motor
 */
void engine_init(void){
    PWM_DDR |= (1<< RIGHT_PWM_PIN)|(1<<LEFT_PWM_PIN);
    EN_DDR &= ~(1<< RIGHT_EN_PIN)| (1<<LEFT_EN_PIN);
    //EN_PORT |= (1<< RIGHT_EN_PIN)| (1<<LEFT_EN_PIN);

    EICRA = (1<<ISC11)|(1<<ISC10)|(1<<ISC01)|(1<<ISC00);
    EIMSK = (1<<INT1)|(1<<INT0);

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
 * Set direction of 
 * 
 * new_direction: 1 if moving forward, -1 if moving backwards, 0 if standing still. 
 */
void engine_set_direction(int8_t new_direction){
    direction = new_direction;
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
 * Set speed 
 */
void engine_set_gear(int8_t new_gear){
    gear = new_gear; 
}

/*
* Get the direction of the vheicle.  
* 
* Return: 1, if moving forward normal direction. -1 if moving backwards. 0 if standing still.
*/
int engine_get_direction(void){
    return direction; 
}

/*
 * Get the heading of the vheicle.  
 * 
 * Return: 1, if moving right normal direction. -1 if moving left. 0 if standing still.
 */
int engine_get_heading(void){
    return degree;
}

/*
 * Retruns the gear, the speed level of the vehicle.
 */
int engine_get_gear(void){
    return gear;
}

/*
 * Claculate and return in meters the distance the vehicle has gone since start.
 */
float engine_get_distance(){
    
    return ((right_en_ticks + left_en_ticks )/102.5*0.22); 
}


