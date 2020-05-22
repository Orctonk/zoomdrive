/*
 * Date: 2020-05-11
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */

/*****************************************/
// Implementationslista:

// Lås i engine     
// Fixa I2C interface
// Fixa engine  
/*****************************************/
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
#include "drivers/RGB_LED_strip.h"

#define LED_PORT PORTC
#define LED_PIN PIN0 
#define LED_DDR DDRC

//Variable declerations
uint32_t last_hb = 0;    // Last recieved heartbeat.
uint32_t last_up = 0;    // Last time update sent.

bool locked;             // Locked if in emergency stop mode.
bool cb_pressed;         // Car button status.
bool dm1;                // Deadman's grip from controller 1.
bool dm2;                // Deadman's grip from controller 2.

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

    LED_strip_init();

    LED_DDR|=(1<<LED_PIN);
	LED_PORT&=~(1<<LED_PIN);

    locked = false; 
    cb_pressed = false; 
    dm1 = false;
    dm2 = false; 
    
    Message_Init(9600);
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
      if(!strcmp(msg.args[0],"1")){
          dm1 = atoi(msg.args[1]);
      }
      else if(!strcmp(msg.args[0],"2")){
          dm2 = atoi(msg.args[1]);
      }

      if ((dm1 && dm2)||(!dm2 && !dm1)){
          //If both controllers deadman's grip is activated the vheicle should stop.
          engine_set_speed(0);
          engine_turn(0);
      }
    }
    else if(msg.type == HONK){
        if(!locked && !strcmp(msg.args[0],"2")) {
            
            if(!strcmp(msg.args[1],"1" )){
                summer_start();
            }
            else if(!strcmp(msg.args[1],"0")){
                summer_stop();
            }
        }
    }
    else if(msg.type == EMBUTTON){
        //If the emergency button has been pressed, activate emergency mode.
        if(!strcmp(msg.args[0],"1" )){
            //Lock
            locked = true; 
        }
        else if(!strcmp(msg.args[0],"0" )){
            //Unlock
            locked = false; 
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
            if((dm1 && !dm2)||(dm2 && !dm1)){
                engine_turn(atoi(msg.args[0]));
            }
        }  
    }
    else if(msg.type == HEARTBEAT){
        //Save time of heartbeat. 
        last_hb = Time_GetMillis();
    }
}

/*
 * Main function 
 */
int main(void) {
    init();
    Message_Register(0xff,callback);
    sei();


    LCD_set_cursor(0x00);
    LCD_write_string("ZOOMDRIVE TM");

    Message update_msg;
    uint32_t curtime;
    
    while(1){
        //Check for messages 
        Message_Update();
        //Save current time 
        curtime = Time_GetMillis();

        //Send a update every 2 seconds to the remotes and logger. 
        if(curtime -last_up > 2000){
            
            update_msg.type = UPDATE;

            itoa(locked ? 1 : 0, update_msg.args[0], 10);               
            itoa(engine_get_speed(), update_msg.args[1], 10);
            sprintf(update_msg.args[2],"%u" ,front_distance());
        
            Message_Send(update_msg, 3);

            last_up = curtime;
        }

        //Check if heartbeat recieved. 
        if (curtime - last_hb > 3000){
            locked = true; 
        }

        // if(button_pressed() && !cb_pressed){
        //     cb_pressed = true; 
        //     update_msg.type = CARBUTTON;
        //     update_msg.args[0] == "1"; 
        //     Message_Send(update_msg, 1); 

        // }
        // else if(!button_pressed() && cb_pressed){
        //     cb_pressed = false;
        //     update_msg.type = CARBUTTON;
        //     update_msg.args[0] == "0"; 
        //     Message_Send(update_msg, 1); 
        // }

        //Check if in emergency state.LCD_set_cursor(0x10);
            LCD_write_string("               "); 
        if(locked){
            LED_PORT |= (1<<LED_PIN);
            engine_set_speed(0);
            engine_turn(0);
            LCD_set_cursor(0x10);
            LCD_write_string("STOPPED");  
        }
        else {
            LED_PORT &= ~(1<<LED_PIN);
        }

        //Check in what direction the vehicle is going.
        if(engine_get_speed() == -1){
            //If the vehicle is backing, 
            yellow_lamp(true); 
            LCD_set_cursor(0x10);
            LCD_write_string("Going backwards!");  
        }
        else if(engine_get_speed() > 0){
            //If the vehicle is driving forward
            green_lamp(true);
            LCD_set_cursor(0x10);
            LCD_write_string("Going forward");  
        }
        else {
            green_lamp(false);
            yellow_lamp(false); 
        }

        //Check if vehicle is turning and set turn lights.
        if(engine_get_heading() == 1){
            turn_signal_right(true);
            turn_signal_left(false);
        }
        else if(engine_get_heading() == -1){
            
            turn_signal_left(true);
            turn_signal_right(false);
        }
        else{
            turn_signal_right(false);
            turn_signal_left(false);
        }
    }
}