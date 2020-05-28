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
static volatile int gear = 1;
static volatile int lastCallback = 0;
static volatile int callStop = 0;
static volatile char *infoStr;
int reset = 0;
int mH = 0;
int strInt, prev, mPrev, mStrInt;
char *currStr;
char *selStr;
char *nxtStr;
char *mCurrStr, *mSelStr, *mNxtStr;
char *mStrings[5];
char *strings[10];

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

void MelmenuRight(void) {
    _delay_ms(10);
    if (!(++mStrInt > 4)) {
        mCurrStr = mNxtStr;
        mNxtStr = mStrings[mStrInt];
    } else {
        mStrInt = 0;
        mCurrStr = mNxtStr;
        mNxtStr = mStrings[mStrInt];
    }
}

void MelmenuLeft(void) {

    _delay_ms(10);
    if (!(--mStrInt < 0)) {
        mNxtStr = mCurrStr;
        mCurrStr = mStrings[mStrInt];
    } else {
        mStrInt = 4;
        mNxtStr = mCurrStr;
        mCurrStr = mStrings[mStrInt];
    }

}

void menuRight(void) {
    _delay_ms(10);
    if (!(++strInt > 9)) {
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

        case GEAR:
            gear = atoi(msg.args[0]);
            break;

        case HONK:
            if(!strcmp(msg.args[0], "1")) {
                if (!strcmp(msg.args[1], "1")) {
                    summer_start();
                } else {
                    summer_stop();
                }
            }

            break;

        case CARBUTTON:

            // Safer - controlls correct input
            
            if (!strcmp(msg.args[0], "1")) {
                cBtn = 1;
                summer_start();
            } else {
                cBtn = 0;
                summer_stop();
            }
            

            break;

        case UPDATE_EM:

            if (!strcmp(msg.args[0], "1")) {
                PORTC |= (1<<PINC3);
            } else {
                PORTC &= ~(1<<PINC3);
            }
            break;


        case INFO:
            blink();
            strcpy(infoStr, msg.args[0]);

            break;

        default:
            break;
    }
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

void writeRegMenu(int dead, int gear) {
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
            Message msg;
            sendMessage(HONK, "3", NULL, msg);
            break;
        } else if (!(PIND & (1<<7)) && !(PIND & (1<<6))) {
            return 0;
        }
        _delay_ms(50);

    }
    return 0;
}

int getVert() {


    int vertAdc = readADC(0);

    if (vertAdc >= 800) {
        return 1;
    } else if (vertAdc <= 400) {
        return -1;
    } else {
        return 0;
    }

}

int getHor() {

    int horAdc = readADC(1);

    if (horAdc >= 800) {
        return 1;
    } else if (horAdc <= 400) {
        return -1;
    } else {
        return 0;
    }

}

void writeMelMenu() {

    writeString(mCurrStr);
    writeString(" ");
    writeString(mNxtStr);
    moveCursor(0b10010000);
    writeString("Selected: ");
    writeString(mSelStr);
    moveCursor(0b10100000);
    writeData(mStrInt + 48);
}

void melodiesMenu() {

    while(1) {
        clearDisplay();
        writeMelMenu();

        mH = getHor();

        if (mH == -1) {

            MelmenuRight();
            if (mPrev) {
                MelmenuRight();
            }
            mPrev = 0;

        } else if (mH == 1) {

            MelmenuLeft();
            if (!mPrev) {
                MelmenuLeft();
            }
            mPrev = 1;
        } 

        if (!(PINC & (1<<2))) {
            mSelStr = mCurrStr;
            if (!strcmp(mSelStr, "EXIT")) {
                return;
            } else {
                char buff[10];
                Message msg;
                sendMessage(HONK, "2", itoa(mStrInt, buff, 10), msg);
            }
        }

        _delay_ms(50);

    }

}

int checkCMD() {
    //Message msg;
    //if (!strcmp(selStr, strings[6])) {
    //    sendMessage(CMD, "0", NULL, msg);
    //    return 1;
    //} else if (!strcmp(selStr, strings[7])) {
    //    sendMessage(CMD  "1", NULL, msg);
    //    return 1;
    //} else if (!strcmp(selStr, strings[8])) {
    //    sendMessage(CMD, "2", NULL, msg);
    //    return 1;
    //} else if (!strcmp(selStr, strings[9])) {
    //    sendMessage(CMD, "3", NULL, msg);
    //    return 1;
    //}
    return 0;
}

/*
   The main loop that controlls the program flow. 
   */
int main(void) {

    strInt = prev = -1;
    mStrInt = 0;
    mPrev = 1;

    mStrings[0] = "Glass";
    mStrings[1] = "USSR";
    mStrings[2] = "HG";
    mStrings[3] = "Twinkle";
    mStrings[4] = "EXIT";

    mSelStr = mCurrStr = mStrings[0];
    nxtStr = mStrings[1];

    strings[0] = "st1";
    strings[1] = "st2";
    strings[2] = "st2";
    strings[3] = "st3";
    strings[4] = "Melodies";
    strings[5] = "CC";
    strings[6] = "FWD";
    strings[7] = "BCK";
    strings[8] = "90R";
    strings[9] = "90L";


    infoStr = "NaN";
    selStr = currStr = strings[0];
    nxtStr = strings[1];


    int cState, dead, vertAdc, horAdc, ldh, lndh, lH, lV, vert, hor;
    cState = dead = vertAdc = horAdc = lH = lV = ldh = lndh = vert = hor = 0;

    Message msg;

    inits();
    sei();


    // Loop as long as there is power in the MCU.
    while(1) {

        Message_Update();
        Time_Update();

        clearDisplay();

        vert = getVert();
        hor = getHor();
        if ((dead = checkDead(dead))) {

            if (!(PIND & (1<<6)) && (ldh != 1)) {
                sendMessage(HONK, "2", "1", msg);
                ldh = 1;
            } else if ((PIND & (1<<6)) && (ldh != 0)) {
                sendMessage(HONK, "2", "0", msg);
                ldh = 0;
            }  

            if (lV != vert) {
                lV = vert;
                vertDrive(vert);
            }

            if (lH != hor) {
                lH = hor;
                horDrive(hor);
            } 

        } else {

            if (hor == -1) {

                menuRight();
                if (prev) {
                    menuRight();
                }
                prev = 0;

            } else if (hor == 1) {

                menuLeft();
                if (!prev) {
                    menuLeft();
                }
                prev = 1;
            } 

            if (!(PIND & (1<<6)) && (lndh != 1)) {
                sendMessage(HONK, "0", "1", msg);
                lndh = 1;
            } else if ((PIND & (1<<6)) && (lndh != 0)) {
                sendMessage(HONK, "0", "0", msg);
                lndh = 0;
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
                } else if (!strcmp(currStr, "Melodies")) {
                    _delay_ms(30);
                    melodiesMenu();

                } else {
                    selStr = currStr;
                    if (!checkCMD()) {
                        sendMessage(CSTSTRING, selStr, NULL, msg);
                    }
                }
            }
        }

        writeRegMenu(dead, gear);

        _delay_ms(50);
    }
    return 0;
}
