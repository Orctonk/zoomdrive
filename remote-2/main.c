#include <avr/io.h>
#include "define.h"
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
static volatile int eBtn = 0;
static volatile char *infoStr;
int reset = 0;
int tous, huns, tens, ones, intDec, dist, tilt;
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
    if (!(++strInt == 5)) {
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
    if (!(--strInt == -1)) {
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

        case INFO:
            strcpy(infoStr, msg.args[0]);
            break;

        case HEARTBEAT:
            lastCallback = 0;
            PORTC &= ~(1<<PINC4);

            if (!strcmp(msg.args[0], "0")) {
                Message msg;
                msg.type = HEARTBEAT;
                strcpy(msg.args[0], "1\0");
                Message_Send(msg, 1);
            }
            break;

        case EMSTATE:
            if (!strcmp(msg.args[0], "1")) {
                eBtn = 1;
                PORTC |= (1<<PINC3);
            } else {
                eBtn = 0;
                PORTC &= ~(1<<PINC3);
            } 
            break;

        default:
            break;
    }
    sei();
}


void vertDrive(int dir) {
    Message msg;
    msg.type = ENGINE_POWER;
    if (dir == 1) {
        strcpy(msg.args[0], "1");
    } else if (dir == -1) {
        strcpy(msg.args[0], "-1");
    } else {
        strcpy(msg.args[0], "0");
    }
    Message_Send(msg, 1);
}

void horDrive(int dir) {
    Message msg;
    msg.type = HEADING;
    if (dir == 1) {
        strcpy(msg.args[0], "-1");
    } else if (dir == -1) {
        strcpy(msg.args[0], "1");
    } else {
        strcpy(msg.args[0], "0");
    }
    Message_Send(msg, 1);
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
    writeData(eBtn + 48);
    writeData(cBtn + 48);

    moveCursor(0b10100000);
    
    writeString(infoStr);

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
        msg.type = DEADMAN;
        strcpy(msg.args[0], "1");
        strcpy(msg.args[1], "1");
        Message_Send(msg, 2);
        _delay_ms(10);
        return 1;
    } else if ((PIND & (1<<7)) && (dead != 0)) {
        msg.type = DEADMAN;
        strcpy(msg.args[0], "1");
        strcpy(msg.args[1], "0");
        Message_Send(msg, 2);
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

        if (!cState && !(PIND & (1<<7))) {
            cState++;
        } else if ((cState == 1) && (!(PIND & (1<<6)))){
            cState++;
        } else if ((cState == 2) && (!(PIND & (1<<3)))){
            blink();
            break;
        } else if (!(PINC & (1<<2))) {
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

    /*
     *  TEXT IDEAS:
     *
     *
     * */
    strings[0] = "st1";
    strings[1] = "st2";
    strings[2] = "st2";
    strings[3] = "st3";
    strings[4] = "CC";

    selStr = currStr = strings[0];
    nxtStr = strings[1];
    infoStr = "0";

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
                msg.type = HONK;
                strcpy(msg.args[0], "2");
                Message_Send(msg, 1);
            } 

            if (lV != vert) {
                if ((vert == 1)) {
                    lV = vert;
                    vertDrive(vert);
                } else if ((vert == -1)) {
                    lV = vert;
                    vertDrive(vert);
                } else if ((vert == 0)) {
                    lV = vert;
                    vertDrive(vert);
                }
            } else if (lH != hor) {

                if ((hor == 1)) {
                    lH = hor;
                    horDrive(hor);
                } else if ((hor == -1)) {
                    lH = hor;
                    horDrive(hor);
                } else if ((hor == 0)) {
                    lH = hor;
                    horDrive(hor);
                }
            }

        } else {

            if (!(PIND & (1<<6))) {
                msg.type = HONK;
                strcpy(msg.args[0], "0");
                Message_Send(msg, 1);
            } 

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
            
            if (!(PIND & (1<<3))) {
                msg.type = GEAR;
                switch (++gear) {
                    case 2:
                        strcpy(msg.args[0], "2");
                        break;
                    case 3:
                        strcpy(msg.args[0], "3");
                        break;
                    default:
                        gear = 1;
                        strcpy(msg.args[0], "1");
                }
                Message_Send(msg, 1);
                _delay_ms(30);
            }

            if (!(PINC & (1<<2))) {
                if (!strcmp(currStr, "CC")) {
                    cState = cheatCodes(cState);
                } else {
                    selStr = currStr;
                    msg.type = CSTSTRING;
                    strcpy(msg.args[0], selStr);
                    Message_Send(msg, 1);
                }
            } 
        }

        writeToScreen(dead, gear);

        _delay_ms(50);
    }

    return 0;
}
