#include <avr/io.h>
#include <avr/interrupt.h>
#include "define.h"
#include "uart.h"
#include <math.h>
#include <util/delay.h> 
#include "lcd.h"

/*
	Simple program that takes the temperature of an object when it is at the correct distance
	and the device is tilted at a 90 degree angle.

	Written by Jakob Lundkvist 
*/


// Global variables
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
    DDRC |= (1<<PINC5);
    PORTC &= ~(1<<PINC5);
}

void btnInit(void) {

    DDRD &= ~(1<<PIND5) | (1<<PIND6) | (1<<PIND7);
    PORTD |= (1<<PIND5) | (1<<PIND6) | (1<<PIND7);
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
void ADCInit(){

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

void menuRight() {
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

void menuLeft() {

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

/*
	The main loop that controlls the program flow. 
*/
int main(void) {

    strInt = prev = -1;


    strings[0] = "st1";
    strings[1] = "st2";
    strings[2] = "st3";
    strings[3] = "st4";
    strings[4] = "st5";

    selStr = currStr = strings[0];
    nxtStr = strings[1];

    DDRD |= (1<<PIND4);

    int dead = 0;
    int adcVal = 0;

	// Initialize all the necessary parts.
    lcdInit();
    ADCInit();
    ledInit();
    joyStickInit();
    btnInit();


// Loop as long as there is power in the MCU.
    while(1) {

        clearDisplay();
        
        if (!(PIND & (1<<7))) {
            dead = 1;
        } else {
            dead = 0;
        }

		writeString(currStr);

        writeData (' ');

		writeString(nxtStr);

        moveCursor(0b10010000);

        writeString("Selected: ");
        writeString(selStr);


        adcVal = readADC(1);

        if (!dead && (adcVal <= 200)) {
            menuRight();
            if (prev == 1) {
                menuRight();
            }
            prev = 0;
        } else if (!dead && (adcVal >= 800)) {
            menuLeft();
            if (prev == 0) {
                menuLeft();
            }
            prev = 1;
        }

        moveCursor(0b10100000);

		writeString("BTN: ");
        writeData(PINC+48);

        if (dead) {
            writeData('D');
        } else {
            writeData('N');
        }

        if (!PINC && !dead) {
            selStr = currStr;
        }

        if (!(strInt == -1)) {
            writeData(strInt+48);
        } else {
            writeData(48);
        }
        _delay_ms(100);
    }

    return 0;
}

