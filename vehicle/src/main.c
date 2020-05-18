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
#include "drivers/I2C.h"
#include "message.h"

#define LED_PORT PORTC
#define LED_PIN PIN0 
#define LED_DDR DDRC

//Variable declerations
int32_t last_hb;         //Last recieved heartbeat.
int32_t last_up;

bool locked;             //Locked if in emergency stop mode.
bool dm1;                // Deadmam's grip from controller 1
bool dm2;                // Deadmam's grip from controller 2

/*
 * Initialze device by calling all initialization functions 
 */
void init(){  
    SPI_init();
    LCD_init();
    summer_init(); 
    engine_init();
    I2C_init();
    timer_init();

    LED_DDR|=(1<<LED_PIN);
	LED_PORT&=~(1<<LED_PIN);

    locked = true; 
    dm1 = false;
    dm2 = false; 
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
    else if(msg.type == DEADMAN){
      if(msg.args[0] == 0){
          dm1 = msg.args[1];
      }
      else if(msg.args[0] == 1){
          dm2 = msg.args[1];
      }
      if(dm1 && dm2){
          //If both controllers deadman's grip is activated the vheicle should stop.
          engine_set_speed(0);  
      }
    }
    else if(msg.type == HONK){
        if(!locked) {
            summer_beep();
        }
    }
    else if(msg.type == EMBUTTON){
        //If the emergency button has been pressed, activate emergency mode.
        if(msg.args[0] == 1){
            //Lock
            locked = true; 
            engine_set_speed(0);   
            LCD_set_cursor(0x00);
            LCD_write_string("STOPPED");  
            Message reply;
            reply.type = EMSTATE; 
            Message_Send(reply, 0);
        }
        else if(msg .args[0] == 0){
            //Unlock
            locked = false; 
            CD_set_cursor(0x00);
            LCD_write_string("      "); 
        }
    }
    else if(msg.type == ENGINE_POWER){
        if(!locked) {
            if((dm1 && !dm2)||(dm2 && !dm1)){
                engine_set_speed(atoi(msg.args[0]));
            }
        }
    }
    else if(msg.type == HEADING){
        if(!locked) {
            engine_turn(atoi(msg.args[0]));
        }
       
    }
    else if(msg.type == HEARTBEAT){
        //Save time of heartbeat. 
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
        if(timer_get_time() -last_up > 1){
            Message update; 
            update.type = SPEED;
            itoa(engine_get_speed(), update.args[0], 10);
            Message_Send(update, 0);
        }

        if (timer_get_time() - last_hb > 3){
            engine_set_speed(0);
        }
        
        if(locked){
            LED_PORT |= (1<<LED_PIN);
        }
        else {
            LED_PORT &= ~(1<<LED_PIN);
        }

        if(engine_get_speed() == -1){
            //If the vhiecle is backing, 
            summer_start(); 
            yellow_lamp(1); 
        }
        else if(engine_get_speed() > 0){
            green_lamp(1);
        }
        else {
            summer_stop();
            green_lamp(0);
            yellow_lamp(0); 
        }
}