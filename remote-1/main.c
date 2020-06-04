#include <avr/io.h>
#include <stdio.h>
#include "define.h"
#include <stdlib.h>
#include "summer.h"
#include "../common/message.h"
#include "../common/time.h"
#include <math.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h> 
#include "lcd.h"

/**
 * A simple program for a remote control to a vehicle. Uses a accelerometer 
 * to steer the car.
 * 
 * Author: Erika Lundin
 * Date: 2020-06-04
 */


// Global variables
static volatile int cBtn = 0;
static volatile int gear = 1;
static volatile int lastCallback = 0;
static volatile int callStop = 0;
static volatile char *rString;
static volatile char *cString;
static volatile char *sString;
static volatile char *tString;
int reset = 0;
int strInt, prev, star;
char *currStr;
char *selStr;
char *nxtStr;
char *infoStr;
char *strings[5];

/**
 * Blinks the LED on PB6.
 */ 
void blink(void) {
    PORTB |= (1<<PORTB6);
    _delay_ms(10);
    PORTB &= ~(1<<PORTB6);
    _delay_ms(10);
}

/**
 * Initialize the LED-display.
 */ 
void ledInit(void) {
    DDRB |= (1<<PINB6) | (1<<PINB7);
    PORTB &= ~(1<<PINB6) | (1<<PINB7);
}

/**
 * Initialize the buttons.
 */ 
void btnInit(void) {

    DDRD &= ~(1<<PIND2) | (1<<PIND3) | (1<<PIND4);
    PORTD |= (1<<PIND2) | (1<<PIND3) | (1<<PIND4);
}

/**
* A function that initializes the ADC to be used.
*/
void ADCInit(void){

    ADCSRA  |= (1UL<<ADPS2) | (1UL<<ADPS1) | (1UL<<ADPS0) | (1UL<<ADEN);    

    ADMUX |= (0UL<<REFS1) | (1UL<<REFS0);  
}

/**
* A function that reads from the ADC-channel specified as the parameter. 
*/
int readADC(int channel) {

    ADMUX = (ADMUX & 0Xf0) | (channel & 0x0f);

    ADCSRA |= (1UL<<ADSC); 
    while(((ADCSRA) & (1UL<< ADIF))==0); 

    return ADCW;
}

/**
 * A function that checks if if the hearbeats have stopped based on a set timer.
 * If they have it toggles an LED.
 */ 
void timeCallback(void) {

    if (lastCallback) {
        PORTB |= (1<<PINB7);
    }
    lastCallback = 1;
}

/**
 * A simple helperfunction for sending a message.
 */ 
void sendMessage(int topic, char* payLoad1, char* payLoad2, Message msg) {
    int argc =  1;
    msg.type = topic;
    strcpy(msg.args[0], payLoad1);
    if (payLoad2) {
        strcpy(msg.args[1], payLoad2);
        msg.args[1][strlen(payLoad2)] = '\0';
        argc++;
    } else {
        msg.args[0][strlen(payLoad1)] = '\0';
    }
    Message_Send(msg, argc);
}

/**
 * Handles messages that comes through Usart.
 */ 
void callback(Message msg) {
    
    //Case after what type of message that needs to be handled. 
    switch(msg.type) {

        // Reset last callback and reply to a heartbeat from logger.
        case HEARTBEAT:
            lastCallback = 0;
            PORTB &= ~(1<<PINB7);

            if (!strcmp(msg.args[0], "0")) {
                Message msg;
                sendMessage(HEARTBEAT, "1", NULL, msg);
                _delay_ms(50);
            }
            break;
        // Check if dead man's grip is activated on remote 2 and if so, turn on 
        // LED PB6.
        case DEADMAN:
            
            if (!strcmp(msg.args[0], "2")) {
                if (!strcmp(msg.args[1], "1")) {
                    PORTB |= (1<<PINB6);
                } else {
                    PORTB &= ~(1<<PINB6);
                }
            }

            break;
        
        //Syncs gears between the remotes.
        case GEAR:
            gear = atoi(msg.args[0]);
            break;

        // Check if the honk is directed at this remote.
        case HONK:
            if(!strcmp(msg.args[0], "0")) {
                if (!strcmp(msg.args[1], "1")) {
                    summer_start();
                } else {
                    summer_stop();
                }
            }

            break;

        //Check if the carbutton is activated, if so the summer will honk.
        case CARBUTTON:
            if (!strcmp(msg.args[0], "1")) {
                cBtn = 1;
                summer_start();
            } else {
                cBtn = 0;
                summer_stop();
            }
            break;

        //Check if the emergencybutton is activated.
        case UPDATE_EM:
            if (!strcmp(msg.args[0], "1")) {
                star = 1;
                
            } else {
                star = 0;
            }
            break;

        //Read in information from the car.
        case INFORMATION:
            strcpy(cString, msg.args[0]);

            break;

        //Update the sensorsmessages from the car.
        case UPDATE_SENSORS:
            sprintf(sString, "%s", msg.args[0]);
            sprintf(rString, "%s", msg.args[1]);
            sprintf(tString, "%s", msg.args[2]);

            break;
        
        default:
            break;
    }
}

/**
 * Sends a message to the car with the vertical direction it should drive in.
 */
void vertDrive(int dir) {

    Message msg;
    if (dir == 1) {
        sendMessage(ENGINE_POWER, "1", NULL, msg);
    } else if (dir == -1) {
        sendMessage(ENGINE_POWER, "-1", NULL, msg);
    } else {
        sendMessage(ENGINE_POWER, "0", NULL, msg);
    }
}

/**
 * Sends a message to the car with the horizontal direction it should drive in.
 */
void horDrive(int dir) {

    Message msg;
    if (dir == 1) {
        sendMessage(HEADING, "-1", NULL, msg);
    } else if (dir == -1) {
        sendMessage(HEADING, "1", NULL, msg);
    } else {
        sendMessage(HEADING, "0", NULL, msg);
    }
}

/**
 * Print the information to the LCD-display.
 */
void writeToScreen(int dead, int gear, int star) {
    writeString("G:");
    writeData(gear+48);

    if (dead) {
        writeData('D');
    } else {
        writeData('N');
    }
    writeData(cBtn + 48);

    if (star){
        writeData('*');
    }
    moveCursor(0b10010000);

    writeString("INFO: ");
    writeString(cString);
    moveCursor(0b10100000);
    writeString(sString);
    writeData(' ');
    writeString(rString);
    writeData(' ');
    writeString(tString);



}

/**
 * Initialize all the necessary parts.
 */
void inits(void) {

    Time_Init();
    Time_RegisterTimer(600, timeCallback);
    lcdInit();
    summer_init();
    ADCInit();
    ledInit();
    btnInit();
    Message_Init(4800);
    Message_Register(0xff, callback);
}

/**
 * Checks if the DeadMan button is pressed and returns the answer.
 */ 
int checkDead(int dead) {
    Message msg;

    if (!(PIND & (1<<2)) && (dead != 1)) {

        sendMessage(DEADMAN, "1", "1", msg);
        _delay_ms(50);
        return 1;
    } else if ((PIND & (1<<2)) && (dead != 0)) {

        sendMessage(DEADMAN, "1", "0", msg);
        _delay_ms(50);
        return 0;
    }

    return dead;
}

/*
   The main loop that controlls the program flow. 
   */
int main(void) {

    cString = "NaN";
    sString = "Srr";
    rString = "Rrr";
    tString = "Trr";

    int dead, vertAdc, horAdc, ldh, lndh, lH, lV, vert, hor;
    dead = vertAdc = horAdc = lH = lV = ldh = lndh = vert = hor = 0;

    Message msg;

    inits();
    sei();



    // Loop as long as there is power in the MCU.
    while(1) {

        Message_Update();
        Time_Update();

        clearDisplay();

        vertAdc = readADC(2);
        horAdc = readADC(1);


        if (vertAdc <= 480) {
            vert = 1;
        } else if (vertAdc >= 580) {
            vert = -1;
        } else {
            vert = 0;
        }
        

        if (horAdc <= 480) {
            hor = -1;
        } else if (horAdc >= 580) {
            hor = 1;
        } else {
            hor = 0;
        }

        if ((dead = checkDead(dead))) {

            if (!(PIND & (1<<3)) && (ldh != 1)) {
                ldh = 1;
            } else if ((PIND & (1<<3)) && (ldh != 0)) {
                sendMessage(HONK, "2", "0", msg);
                ldh = 0;
            } else if (lV != vert) {
                lV = vert;
                vertDrive(vert);
            } else if (lH != hor) {
                lH = hor;
                horDrive(hor);
            } 

        } else {

            if (!(PIND & (1<<3)) && (lndh != 1)) {
                sendMessage(HONK, "1", "1", msg);
                lndh = 1;
            } else if ((PIND & (1<<3)) && (lndh != 0)) {
                sendMessage(HONK, "1", "0", msg);
                lndh = 0;
            } else if (!(PIND & (1<<4))) {

                switch (++gear) {
                    case 2:
                        sendMessage(GEAR, "2", NULL, msg);
                        break;
                    case 3:
                        sendMessage(GEAR, "3", NULL, msg);
                        break;
                    default:
                        gear = 1;
                        sendMessage(GEAR, "1", NULL, msg);
                }
                _delay_ms(30);

            } 
        }

        writeToScreen(dead, gear, star);

        _delay_ms(50);
    }
    return 0;
}
