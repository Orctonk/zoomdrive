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
#include <stdbool.h>

#include "drivers/SPI.h"
#include "drivers/LCD.h"
#include "drivers/timer.h"
#include "drivers/summer.h"
#include "drivers/engine.h"
#include "message.h"

#define LED_PORT PORTC
#define LED_PIN PIN0 
#define LED_DDR DDRC

//Variable declerations
int32_t last_hb;         //Last recieved heartbeat.

bool locked;             //Locked if in emergency stop mode.

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

    locked = false; 
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

        LCD_set_cursor(0x20);
        LCD_write_string("PING PONG ");
        Message_Send(reply, 0); 
    }
    else if(msg.type == HONK){
        if(!locked) {
            summer_beep();
        }
    }
    else if(msg.type == EMBUTTON){
        if(msg.args[0] == 1){
            //Lock
            locked = true; 
            engine_set_speed(0);     
        }
        else if(msg.args[0] == 0){
            //Unlock
            locked = false; 
        }
    }
    else if(msg.type == ENGINE_POWER){
        if(!locked) {
            engine_set_speed(atoi(msg.args[0]));
        }
        
    }
    else if(msg.type == HEADING){
        if(!locked) {
            engine_turn(atoi(msg.args[0]));
        }
       
    }
    else if(msg.type == HEARTBEAT){
        last_hb = timer_get_time();
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
        if (timer_get_time() - last_hb > 3){
            engine_set_speed(0);
        }
        while(locked){
                LED_PORT|=(1<<LED_PIN);
            }
}