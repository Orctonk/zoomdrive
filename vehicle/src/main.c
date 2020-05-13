/*
 * Date: 2020-05-11
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */

#include <avr/io.h>
#include <stdio.h>

#include "drivers/SPI.h"
#include "drivers/LCD.h"
#include "drivers/timer.h"

#define LED_PORT PORTC
#define LED_PIN PIN0 
#define LED_DDR DDRC

#define BUTTON_PORT PORTC
#define BUTTON_PIN PIN1
#define BUTTON_DDR DDRC

/*
 * Initialze device by calling all initialization functions 
 */
void init(){  
    SPI_init();
    LCD_init();
    timer_init();

    LED_DDR|=(1<<LED_PIN);
	LED_PORT&=~(1<<LED_PIN);

    BUTTON_DDR|=(1<<BUTTON_PIN);
	BUTTON_PORT&=~(1<<BUTTON_PIN);
}

/*
 * Main function 
 */
int main(void) {
    init();

    LED_PORT|=(1<<LED_PIN);
    LCD_write_string("Welcome");
    
    
    while(1){
    }
}