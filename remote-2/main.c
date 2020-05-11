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

void blink(void) {
	PORTB |= (1<<PORTB0);
	_delay_ms(100);
	PORTB &= ~(1<<PORTB0);
	_delay_ms(100);
}

void ledInit(void) {
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
              

/*
	The main loop that controlls the program flow. 
*/
int main(void) {


    DDRC &= ~(1<<PINC2);
    PORTC |= (1<<PINC2);


	// Initialize all the necessary parts.
    lcdInit();
    ADCInit();

	// Loop as long as there is power in the MCU.
    while(1) {

        clearDisplay();

		writeString("Hor: ");
        bcd2Asc(readADC(1));
		writeTemp(tous, huns, tens, ones, intDec);

        moveCursor(0b10100000);


		writeString("Vert: ");
        bcd2Asc(readADC(0));
		writeTemp(tous, huns, tens, ones, intDec);
	
        _delay_ms(10);
    }

    return 0;
}

