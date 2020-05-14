/*
 * Date: 2020-05-11
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "drivers/SPI.h"
#include "drivers/LCD.h"
#include "drivers/timer.h"
#include "drivers/summer.h"
#include "drivers/engine.h"
#include "message.h"

#define LED_PORT PORTC
#define LED_PIN PIN0 
#define LED_DDR DDRC

#define BUTTON_PORT PORTC
#define BUTTON_PIN PIN1
#define BUTTON_DDR DDRC

//int32_t last_update;         //Last recieved heartbeat.

/*
 * Initialze device by calling all initialization functions 
 */
void init(){  
    SPI_init();
    LCD_init();
    summer_init(); 
    engine_init();
    timer_init();

    LED_DDR|=(1<<LED_PIN);
	LED_PORT&=~(1<<LED_PIN);

    BUTTON_DDR|=(1<<BUTTON_PIN);
	BUTTON_PORT&=~(1<<BUTTON_PIN);
}

/*
 * Recieves a message and do perform right action.
 */ 
void callback(Message msg){
   
    if(msg.type == PING){  
        Message reply;
        reply.type = PONG; 
        itoa(msg.type, reply.args[0],10);
        strcpy(reply.args[1],msg.args[0]);
        strcpy(reply.args[2],msg.args[1]);

        LCD_write_string("PING PONG ");
        Message_Send(reply); 
    }
    else if(msg.type == HONK){
        summer_beep();
    }
    else if(msg.type == EMBUTTON){
        engine_set_speed(0); 
    }
    else if(msg.type == ENGINE_POWER){
        engine_set_speed(atoi(msg.args[0]));
    }
    else if(msg.type == HEADING){
        engine_turn(atoi(msg.args[0]));
    }
}

/*
 * Main function 
 */
int main(void) {
    init();

    Message_Init(4800);
    Message_Register(0xff,callback);
    
   
    while(1){
    }
}