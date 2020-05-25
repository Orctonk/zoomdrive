#include <avr/io.h>
#include <stdio.h>
#include "define.h"
#include <stdlib.h>
#include "../common/message.h"
#include "../common/time.h"
#include "summer.h"
#include <math.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h> 
#include "lcd.h"


// Global variables
static volatile int cBtn = 0;
static volatile int lastCallback = 0;
static volatile int callStop = 0;
static volatile char *emStateString, *distString, *speedString, *cString;
int reset = 0;
int strInt, prev;
char *currStr;
char *selStr;
char *nxtStr;
char *strings[5];

void blink(void) {
    PORTC |= (1<<PORTC5);
    _delay_ms(10);
    PORTC &= ~(1<<PORTC5);
    _delay_ms(10);
}

void ledInit(void) {
    DDRB |= (1<<PINB6) | (1<<PINB7);
    PORTB &= ~(1<<PINB6) | (1<<PINB7);
}

void btnInit(void) {

    DDRC &= ~(1<<PINC2) | (1<<PINC1) | (1<<PINC0);
    PORTC |= (1<<PINC2) | (1<<PINC1) | (1<<PINC0);
}

/*
   A function that initializes the ADC to be used.

return:
void

input:
void

*/
void ADCInit(void){

    ADCSRA  |= (1UL<<ADPS2) | (1UL<<ADPS1) | (1UL<<ADPS0) | (1UL<<ADEN);    

    ADMUX |= (0UL<<REFS1) | (1UL<<REFS0);  
}

/*
   A function that reads from the ADC-channel specified as the parameter. 

return:
@ADCW - What has been read on that channel.

input:
@channel - The channel to be read from.

*/
int readADC(int channel) {

    ADMUX = (ADMUX & 0Xf0) | (channel & 0x0f);

    ADCSRA |= (1UL<<ADSC); 
    while(((ADCSRA) & (1UL<< ADIF))==0); 

    return ADCW;
}

void timeCallback(void) {

    if (lastCallback) {
        PORTC |= (1<<PINC4);
    }
    lastCallback = 1;
}

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

void callback(Message msg) {

    cli();
    switch(msg.type) {

        case HEARTBEAT:
            lastCallback = 0;
            PORTC &= ~(1<<PINC4);

            if (!strcmp(msg.args[0], "0")) {
                Message msg;
                sendMessage(HEARTBEAT, "2", NULL, msg);
                _delay_ms(50);
            }
            break;

        case DEADMAN:
            
            if (!strcmp(msg.args[0], "1")) {
                PORTC ^= (1<<PINC5);
            }

            break;

        case HONK:
            if(!strcmp(msg.args[0], "0")) {
                if (!strcmp(msg.args[1], "1")) {
                    summer_start();
                } else {
                    summer_stop();
                }
            }

            break;

        case CSTSTRING:
            strcpy(cString, msg.args[0]);
            break;

        case CARBUTTON:
            if (!strcmp(msg.args[0], "1")) {
                cBtn = 1;
                summer_start();
            } else {
                cBtn = 0;
                summer_stop();
            }
            break;

        case UPDATE:
            if (!strcmp(msg.args[0], "1")) {
                PORTC |= (1<<PINC3);
            } else {
                PORTC &= ~(1<<PINC3);
            }
            strcpy(emStateString, msg.args[0]);
            strcpy(speedString, msg.args[1]);
            strcpy(distString, msg.args[2]);
            break;


        default:
            break;
    }
    sei();
}


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

void writeToScreen(int dead, int gear) {
    writeString("G:");
    writeData(gear+48);

    if (dead) {
        writeData('D');
    } else {
        writeData('N');
    }
    writeData(cBtn + 48);

    moveCursor(0b10010000);

    writeString("INFO: ");
    writeString(cString);

    moveCursor(0b10100000);

    writeString("EB:");
    writeString(emStateString);
    writeString(" V:");
    writeString(speedString);
    writeString(" D:");
    writeString(distString);

}


void inits(void) {

    // Initialize all the necessary parts.
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

int checkDead(int dead) {
    Message msg;

    if (!(PIND & (1<<7)) && (dead != 1)) {

        sendMessage(DEADMAN, "2", "1", msg);
        _delay_ms(50);
        return 1;
    } else if ((PIND & (1<<7)) && (dead != 0)) {

        sendMessage(DEADMAN, "2", "0", msg);
        _delay_ms(50);
        return 0;
    }

    return dead;
}

/*
   The main loop that controlls the program flow. 
   */
int main(void) {

    cString = "Hej";
    emStateString = "E";
    speedString = "S";
    distString = "D";

    int dead, vertAdc, horAdc, ldh, lndh, lH, lV, vert, hor, gear;
    dead = vertAdc = horAdc = lH = lV = ldh = lndh = vert = hor = 0;
    gear = 1;

    Message msg;

    inits();
    sei();

    // Loop as long as there is power in the MCU.
    while(1) {

        Message_Update();
        Time_Update();

        clearDisplay();

        vertAdc = readADC(0);
        horAdc = readADC(1);



        if (vertAdc >= 530) {
            vert = 1;
        } else if (vertAdc <= 470) {
            vert = -1;
        } else {
            vert = 0;
        }


        // Kanke vänd på dessa???

        if (horAdc >= 530) {
            hor = 1;
        } else if (horAdc <= 470) {
            hor = -1;
        } else {
            hor = 0;
        }

        if ((dead = checkDead(dead))) {

            if (!(PINC & (1<<2)) && (ldh != 1)) {
                sendMessage(HONK, "2", "1", msg);
                ldh = 1;
            } else if ((PINC & (1<<2)) && (ldh != 0)) {
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

            if (!(PINC & (1<<1)) && (lndh != 1)) {
                sendMessage(HONK, "1", "1", msg);
                lndh = 1;
            } else if ((PINC & (1<<1)) && (lndh != 0)) {
                sendMessage(HONK, "1", "0", msg);
                lndh = 0;
            } else if (!(PINC & (1<<0))) {

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

        writeToScreen(dead, gear);

        _delay_ms(50);
    }
    return 0;
}
