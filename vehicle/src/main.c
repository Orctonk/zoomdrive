/*
 * Date: 2020-05-11
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */

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
#include "summer.h"
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
bool collision_det;      // If the Haptic sensors have been activated. 

bool dm1;                // Deadman's grip from controller 1.
bool dm2;                // Deadman's grip from controller 2.
bool dm3;                // Deadman's grip from piano (controller 3)
bool dm4;                // Deadman's grip from PS4 (controller 4)


/*
 * Initialze device by calling all initialization functions 
 */
void init(void){ 
    SPI_init();
    LCD_init();
    Summer_Init(); 
    engine_init();
    I2C_init();
    Time_Init(); 

    LED_strip_init();

    LED_DDR|=(1<<LED_PIN);
	LED_PORT&=~(1<<LED_PIN);

    locked = false; 
    cb_pressed = false; 
    collision_det = false;

    dm1 = false;
    dm2 = false; 
    dm3 = false;
    dm4 = false;

    Message_Init(9600);
}

/*
 * Recieves a message and perform right action.
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
        if(!strcmp(msg.args[0],"0")){
            dm1 = atoi(msg.args[1]);
        }
        else if(!strcmp(msg.args[0],"1")){
            dm2 = atoi(msg.args[1]);
        }
        else if(!strcmp(msg.args[0],"2")){
            dm3 = atoi(msg.args[1]);
        }
        else if(!strcmp(msg.args[0],"3")){
            dm3 = atoi(msg.args[1]);          // Fix dm4
        }
            
        //XOR on four values
        if (!((((dm1 != dm2) != dm3 )!= dm4) && !(dm1 && dm2 && dm3 && dm4))){
            //If all controllers deadman's grip is activated the vheicle should stop.
            engine_set_direction(0);
            engine_turn(0);
        }
    }
    else if(msg.type == HONK){
        if(!locked && !strcmp(msg.args[0],"2")) {
            LED_lights(atoi(msg.args[1]));
            Summer_PlayMelody(atoi(msg.args[1]));
            
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
            if ((((dm1 != dm2) != dm3 )!= dm4) && !(dm1 && dm2 && dm3 && dm4)){
                if(!strcmp(msg.args[0],"1") && (front_distance() >= 30)){
                    engine_set_direction(1);
                }
                else if(!strcmp(msg.args[0],"-1") && (back_distance() >= 30)){
                    engine_set_direction(-1);
                }
                else {
                    engine_set_direction(0);
                } 
            }
        }
    }
    else if(msg.type == HEADING){
        if(!locked) {
            if ((((dm1 != dm2) != dm3 )!= dm4) && !(dm1 && dm2 && dm3 && dm4)){
                engine_turn(atoi(msg.args[0]));
            }
        }  
    }
    else if(msg.type == HEARTBEAT){
        //Save time of heartbeat. 
        last_hb = Time_GetMillis();
    }
    else if(msg.type == GEAR) {
        engine_set_gear(atoi(msg.args[0]));
    }
}

/*
 * Sends updates.
 */
void send_update(Message update_msg){
    update_msg.type = UPDATE_EM;
    itoa(locked ? 1 : 0, update_msg.args[0], 10);
    Message_Send(update_msg, 1);

    update_msg.type = UPDATE_STATE;
    itoa(engine_get_direction(), update_msg.args[0], 10);
    itoa(engine_get_heading(), update_msg.args[1], 10);
    itoa(engine_get_gear(), update_msg.args[2], 10);
    Message_Send(update_msg, 3);

    update_msg.type = UPDATE_SENSORS; 
    sprintf(update_msg.args[0],"%u" ,front_distance()); 
    sprintf(update_msg.args[1],"%u" ,back_distance()); 
    sprintf(update_msg.args[2],"%.2f" ,engine_get_distance());
    Message_Send(update_msg, 3);

}

/*
 * Handles if the car button has been pressed.
 */
void car_button_handle(Message update_msg){
    if(button_pressed() && !cb_pressed){
        cb_pressed = true; 
        update_msg.type = CARBUTTON;
        strcpy(update_msg.args[0], "1"); 
        Message_Send(update_msg, 1); 
    }
    else if(!button_pressed() && cb_pressed){
        cb_pressed = false;
        update_msg.type = CARBUTTON;
        strcpy(update_msg.args[0],"0"); 
        Message_Send(update_msg, 1); 
    }
}

/*
 * Handles if a collision has occured.
 */
void collision_handle(Message update_msg){
    if((right_collision()|| left_collision())&& !collision_det){
        locked = true;
        collision_det = true;
        update_msg.type = COLLISION; 
        itoa(left_collision() ? 1 : 0, update_msg.args[0], 10);
        itoa(right_collision() ? 1 : 0, update_msg.args[1], 10);
        Message_Send(update_msg, 2);

        LCD_set_cursor(0x20);
        LCD_write_string("COLLISION     ");  
    }
    if(!(right_collision()|| left_collision())&& collision_det){
        collision_det = false;

        LCD_set_cursor(0x20);
        LCD_write_string("               ");
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
            send_update(update_msg);
            last_up = curtime;
        }

        //Check if heartbeat recieved. 
        if (curtime - last_hb > 3000){
            locked = true; 
        }

        //Check if locked.
        if(locked){
            LED_PORT |= (1<<LED_PIN);
            engine_set_direction(0);
            engine_turn(0);
            LCD_set_cursor(0x10);
            LCD_write_string("STOPPED        ");  
        }
        else {
           LED_PORT &= ~(1<<LED_PIN);
        }

        //Check in what direction the vehicle is going.
        if(engine_get_direction() == -1){
            //If the vehicle is backing, 
            if(back_distance()<= 30){
                engine_set_direction(0);
            }
            backing_lights(true);
            yellow_lamp(true); 
            LCD_set_cursor(0x10);
            LCD_write_string("Going backwards!");  
        }
        else if(engine_get_direction() > 0){
            //If the vehicle is driving forward
            if(front_distance()<= 30){
                engine_set_direction(0);
            }
            LCD_set_cursor(0x10);
            LCD_write_string("Going forward !  ");  
        }
        else {
            backing_lights(false);
            yellow_lamp(false); 
            
            if(!locked ){
                LCD_set_cursor(0x10);
                LCD_write_string("                ");  
            }
        }


        if ((((dm1 != dm2) != dm3 )!= dm4) && !(dm1 && dm2 && dm3 && dm4)){
            green_lamp(true);
        }
        else {
            green_lamp(false);
            
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

        //Check if the car button is pressed. 
        car_button_handle(update_msg);

        //Check if a collission has occured.
        collision_handle(update_msg);
        }
    }
}