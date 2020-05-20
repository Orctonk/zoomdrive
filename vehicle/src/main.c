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
#include <avr/interrupt.h>

#include "drivers/SPI.h"
#include "drivers/LCD.h"
#include "time.h"
#include "drivers/summer.h"
#include "drivers/engine.h"
#include "drivers/I2C.h"
#include "message.h"

#define LED_PORT PORTC
#define LED_PIN PIN0 
#define LED_DDR DDRC

//Variable declerations
uint32_t last_hb = 0;         //Last recieved heartbeat.
uint32_t last_up = 0;

bool locked;             //Locked if in emergency stop mode.
bool dm1;                // Deadmam's grip from controller 1
bool dm2;                // Deadmam's grip from controller 2


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
      if(!strcmp(msg.args[0],"1")){
          dm1 = atoi(msg.args[1]);
      }
      else if(!strcmp(msg.args[0],"2")){
          dm2 = atoi(msg.args[1]);
      }

      if ((dm1 && dm2)||(!dm2 && !dm1)){
          //If both controllers deadman's grip is activated the vheicle should stop.
          engine_set_speed(0);
      }
    }
    else if(msg.type == HONK){
        if(!locked && !strcmp(msg.args[0],"2")) {
            summer_beep();
        }
    }
    else if(msg.type == EMBUTTON){
        //If the emergency button has been pressed, activate emergency mode.
        if(!strcmp(msg.args[0],"1" )){
            //Lock
            locked = true; 
            LCD_set_cursor(0x00);
            LCD_write_string("STOPPED");  
        }
        else if(strcmp(msg.args[0],"0" )== 0){
            //Unlock
            locked = false; 
            LCD_set_cursor(0x00);
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
        last_hb = Time_GetMillis();
    }
}

/*
 * Initialze device by calling all initialization functions 
 */
void init(void){ 
    SPI_init();
    LCD_init();
    summer_init(); 
    engine_init();
    I2C_init();
    Time_Init(); 

    LED_DDR|=(1<<LED_PIN);
	LED_PORT&=~(1<<LED_PIN);

    locked = false; 
    dm1 = false;
    dm2 = false; 
    Message_Init(9600);
    Message_Register(0xff,callback);
}

/*
 * Main function 
 */
int main(void) {
    init();
    sei();

    LCD_set_cursor(0x10);
    LCD_write_string("Speed:");

    uint32_t curtime;
    
    while(1){
        Message_Update();
        curtime = Time_GetMillis();
        if(curtime -last_up > 2000){
            Message update_msg; 
            
            update_msg.type = UPDATE;
            itoa(locked ? 1 : 0, update_msg.args[0], 10);               
            itoa(engine_get_speed(), update_msg.args[1], 10);

            itoa(engine_get_speed(), update_msg.args[2], 10);

            Message_Send(update_msg, 3);
            LCD_set_cursor(0x16); // Display information 

            last_up = curtime;
        }

        if (curtime - last_hb > 3000){
            locked = true; 
            //summer_beep();
        }

        if(locked){
            LED_PORT |= (1<<LED_PIN);
            engine_set_speed(0);
        }
        else {
            LED_PORT &= ~(1<<LED_PIN);
        }

        if(engine_get_speed() == -1){
            //If the vehicle is backing, 
            summer_start(); 
            //yellow_lamp(1); 
        }
        // else if(engine_get_speed() > 0){
        //     //If the vehicle is driving forward
        //     //green_lamp(1);
        // }
        else {
            summer_stop();
            green_lamp(1);
            // yellow_lamp(0); 
        }
    }
}