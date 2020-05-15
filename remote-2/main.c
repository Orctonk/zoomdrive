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
   A function that takes a number, splits it and converts those splits to ascii.

return:
void

input;
@bcd - the number to split.

*/
void bcd2Asc(double bcd) {

    tous = huns = tens = ones = intDec = 0;

    int intPart = (int)bcd;

    double dec = bcd - intPart;

    // Finds the first decimal of the number.
    intDec = round(dec*10);


    // Counts the 100s, 10s, and 1s in the number.
    while (intPart >= 1000) {	
        tous++;
        intPart -= 100;
    }

    while (intPart >= 100) {	
        huns++;
        intPart -= 100;
    }

    while (intPart >= 10) {	
        tens++;
        intPart -= 10;
    }

    while (intPart >= 1) {
        ones++;
        intPart--;
    }

    if (tous) {
        huns = 0;
    }

    // Convert numbers to ascii.
    tous += 48;
    huns += 48;
    tens += 48;
    ones += 48;
    intDec += 48;	
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
    
}

void callback(Message msg) {
    if (msg.type == CARBUTTON) {
        if (cBtn) {
            cBtn = 1;
            summer_start();
        } else {
            cBtn = 0;
            summer_stop();
        }
    } else if (msg.type == INFO) {

    } else if (msg.type == HEARTBEAT) {
        lastCallback = 0;
    }
}


void vertDrive(int dir) {
    Message msg;
    msg.type = ENGINE_POWER;
    if (dir == 1) {
        strcpy(msg.args[0], "-1\n");
    } else if (dir == -1) {
        strcpy(msg.args[0], "1\n");
    } else {
        strcpy(msg.args[0], "0\n");
    }
    Message_Send(msg, 1);
}

void horDrive(int dir) {
    Message msg;
    msg.type = HEADING;
    if (dir == 1) {
        strcpy(msg.args[0], "-1\n");
    } else if (dir == -1) {
        strcpy(msg.args[0], "1\n");
    } else {
        strcpy(msg.args[0], "0\n");
    }
    Message_Send(msg, 1);
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
    strings[2] = "st3";
    strings[3] = "st4";
    strings[4] = "st5";

    selStr = currStr = strings[0];
    nxtStr = strings[1];


    int dead = 0;
    int vertAdc = 0;
    int horAdc = 0;
    int lH = 0;
    int lV = 0;
    int vert, hor;

    // Initialize all the necessary parts.
    Time_Init();
    lcdInit();
    summer_init();
    ADCInit();
    ledInit();
    joyStickInit();
    btnInit();
    Message_Init(4800);
    Message_Register(0xff, callback);

    // Loop as long as there is power in the MCU.
    while(1) {

        clearDisplay();

        if (!(PIND & (1<<7))) {
            dead = 1;
        } else {
            dead = 0;
        }

        if (!(PINC)) {
            Message msg;
            msg.type = CSTSTRING;
            strcpy(msg.args[0], selStr);
            msg.args[0][strlen(selStr) + 1] = '\n';
            Message_Send(msg, 1);
            selStr = currStr;
        } 

        if (vertAdc >= 800) {
            vert = 1;
        } else if (vertAdc <= 200) {
            vert = -1;
        } else {
            vert = 0;
        }
        
        if (horAdc >= 800) {
            hor = 1;
        } else if (horAdc <= 200) {
            hor = -1;
        } else {
            hor = 0;
        }

        if (dead) {

            if (!(PIND & (1<<6))) {
                Message msg;
                msg.type = HONK;
                Message_Send(msg, 0);
            } 

            if ((vert == 1) && (lV != vert)) {
                lV = vert;
                vertDrive(vert);
            } else if ((vert == -1) && (lV != vert)) {
                lV = vert;
                vertDrive(vert);
            } else if ((vert == 0) && (lV != vert)) {
                lV = vert;
                vertDrive(vert);
            }

            if ((hor == 1) && (lH != hor)) {
                lH = hor;
                horDrive(hor);
            } else if ((hor == -1) && (lH != hor)) {
                lH = hor;
                horDrive(hor);
            } else if ((hor == 0) && (lH != hor)) {
                lH = hor;
                horDrive(hor);
            }

        } else {
            if (horAdc >= 800) {
                menuRight();
            } else if (horAdc <= 200) {
                menuLeft();
            } 
        }

        if (!dead && (vertAdc <= 200)) {
            menuRight();
            if (prev == 1) {
                menuRight();
            }
            prev = 0;
        } else if (!dead && (vertAdc >= 800)) {
            menuLeft();
            if (prev == 0) {
                menuLeft();
            }
            prev = 1;
        }

        cli();

        writeString(currStr);

        writeData(' ');

        writeString(nxtStr);

        moveCursor(0b10010000);

        writeString("Selected: ");
        writeString(selStr);

        vertAdc = readADC(1);
        horAdc = readADC(0);


        writeData(' ');

        if (dead) {
            writeData('D');
        } else {
            writeData('N');
        }
        writeData(cBtn + 48);

        sei();

        _delay_ms(50);

    }

    return 0;
}
