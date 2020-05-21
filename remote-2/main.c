#include <avr/io.h>
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

/*
   Simple program that takes the temperature of an object when it is at the correct distance
   and the device is tilted at a 90 degree angle.

   Written by Jakob Lundkvist 
   */


// Global variables
static volatile int cBtn = 0;
static volatile int lastCallback = 0;
static volatile int callStop = 0;
static volatile char *emStateString, *distString, *speedString;
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
    DDRC |= (1<<PINC5) | (1<<PINC4) | (1<<PINC3);
    PORTC &= ~(1<<PINC5) | (1<<PINC4) | (1<<PINC3);
}

void btnInit(void) {

    DDRD &= ~(1<<PIND3) | (1<<PIND6) | (1<<PIND7);
    PORTD |= (1<<PIND3) | (1<<PIND6) | (1<<PIND7);
}

void joyStickInit(void) {

    DDRC &= ~(1<<PINC2);
    PORTC |= (1<<PINC2);
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

void menuRight(void) {
    _delay_ms(10);
    if (!(++strInt > 4)) {
        currStr = nxtStr;
        nxtStr = strings[strInt];
    } else {
        strInt = 0;
        currStr = nxtStr;
        nxtStr = strings[strInt];
    }
}

void menuLeft(void) {

    _delay_ms(10);
    if (!(--strInt < 0)) {
        nxtStr = currStr;
        currStr = strings[strInt];
    } else {
        strInt = 4;
        nxtStr = currStr;
        currStr = strings[strInt];
    }

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
    if (payLoad2 != NULL) {
        strcpy(msg.args[1], payLoad2);
        argc++;
    }
    Message_Send(msg, argc);
}

void callback(Message msg) {

    cli();
    switch(msg.type) {
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

        case HEARTBEAT:
            lastCallback = 0;
            PORTC &= ~(1<<PINC4);

            if (!strcmp(msg.args[0], "0")) {
                Message msg;
                sendMessage(HEARTBEAT, "1", NULL, msg);
            }
            break;

        default:
            break;
    }
    sei();
}


void vertDrive(int dir) {

    Message msg;
    if (dir == 1) {
        strcpy(msg.args[0], "1");
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

    writeString(currStr);

    writeString("  G:");
    writeData(gear+48);

    moveCursor(0b10010000);

    writeString("Selected:");
    writeString(selStr);

    writeData(' ');

    if (dead) {
        writeData('D');
    } else {
        writeData('N');
    }
    writeData(cBtn + 48);

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
    joyStickInit();
    btnInit();
    Message_Init(4800);
    Message_Register(0xff, callback);
}

int checkDead(int dead) {
    Message msg;

    if (!(PIND & (1<<7)) && (dead != 1)) {

        sendMessage(DEADMAN, "1", "1", msg);
        _delay_ms(10);
        return 1;
    } else if ((PIND & (1<<7)) && (dead != 0)) {

        sendMessage(DEADMAN, "1", "0", msg);
        _delay_ms(10);
        return 0;
    }

    return dead;
}

int cheatCodes(int cState) {
    while (1) {
        clearDisplay();
        writeString("Enter Code:");
        writeData(cState+48);
        moveCursor(0b10010000);
        writeString("EXIT=6&7");

        if (!cState && !(PIND & (1<<7))) {
            cState++;
        } else if ((cState == 1) && (!(PIND & (1<<6)))){
            cState++;
        } else if ((cState == 2) && (!(PIND & (1<<3)))){
            blink();
            break;
        } else if (!(PIND & (1<<7)) && !(PIND & (1<<6))) {
            return 0;
        }
        _delay_ms(50);

    }
    return 0;
}


/*
   The main loop that controlls the program flow. 
*/
int main(void) {

    strInt = prev = -1;

    strings[0] = "st1";
    strings[1] = "st2";
    strings[2] = "st2";
    strings[3] = "st3";
    strings[4] = "CC";

    selStr = currStr = strings[0];
    nxtStr = strings[1];
    emStateString = "E";
    speedString = "S";
    distString = "D";

    int cState = 0;
    int dead = 0;
    int vertAdc = 0;
    int horAdc = 0;
    int lH = 0;
    int lV = 0;
    int vert, hor;
    int gear = 1;

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

        if (vertAdc >= 1000) {
            vert = 1;
        } else if (vertAdc <= 200) {
            vert = -1;
        } else {
            vert = 0;
        }
        
        if (horAdc >= 1000) {
            hor = 1;
        } else if (horAdc <= 200) {
            hor = -1;
        } else {
            hor = 0;
        }

        if ((dead = checkDead(dead))) {

            if (!(PIND & (1<<6))) {
                sendMessage(HONK, "2", NULL, msg);
            } else if (lV != vert) {
                lV = vert;
                vertDrive(vert);
            } else if (lH != hor) {
                lH = hor;
                horDrive(hor);
            }

        } else {

            if ((horAdc <= 200)) {

                menuRight();
                if (prev) {
                    menuRight();
                }
                prev = 0;

            } else if ((horAdc >= 1000)) {

                menuLeft();
                if (!prev) {
                    menuLeft();
                }
                prev = 1;
            }

            if (!(PIND & (1<<6))) {

                sendMessage(HONK, "0", NULL, msg);

            } else if (!(PIND & (1<<3))) {

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

            } else if (!(PINC & (1<<2))) {

                if (!strcmp(currStr, "CC")) {
                    cState = cheatCodes(cState);
                } else {
                    selStr = currStr;
                    sendMessage(CSTSTRING, selStr, NULL, msg);
                }
            } 
        }

        writeToScreen(dead, gear);

        _delay_ms(50);
    }
    return 0;
}
