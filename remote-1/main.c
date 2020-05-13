#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include <math.h>
#include <util/delay.h>
#include "lcd.h"



void ledInit(void){
    DDRB |= (1<<PINB6) | (1<<PINB7);
    PORTB &= ~(1<<PINB6) | (1<<PINB7);
}


void ADCInit(void){

    ADCSRA  |= (1UL<<ADPS2) | (1UL<<ADPS1) | (1UL<<ADPS0) | (1UL<<ADEN);    

    ADMUX |= (0UL<<REFS1) | (1UL<<REFS0);  
}

int readADC(int channel) {

    ADMUX = (ADMUX & 0Xf0) | (channel & 0x0f);

    ADCSRA |= (1UL<<ADSC); 
    while(((ADCSRA) & (1UL<< ADIF))==0); 

    return ADCW;
}

void dbtnInit(void){
    DDRD &= ~(1<<PIND0) | (1<<PIND1) | (1<<PIND2);
    PORTD |= (1<<PIND0) | (1<<PIND1) | (1<<PIND2);
}


int main(void){

    int strInt = 0;
    char *currStr;
    char *selStr;
    char *nxtStr;
    char *strings[5];

    strings[0] = "1an";
    strings[1] = "2an";
    strings[2] = "3an";
    strings[3] = "4an";
    strings[4] = "5an";

    selStr = currStr = strings[0];
    nxtStr = strings[1];

    DDRD |= (1<<PIND4);
    int dead = 0;


    lcdInit();
    ADCInit();
    dbtnInit();

    while(1){
        clearDisplay();
        if (!(PIND & (0<<7))) {
            dead = 1;
        } else {
            dead = 0;
        }

    }

    writeString(currStr);
	writeString(nxtStr);
    moveCursor(0b10010000);




    return 0;
}