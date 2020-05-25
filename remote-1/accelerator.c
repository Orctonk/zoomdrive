#include "accelerator.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

void accInit(){
    ADCSRA = (1<<ADEN) | (0<<ADATE) | (1<<ADPS1) | (1<<ADPS0);
    ADMUX |= (1<<REFS0)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0)|(0<<ADLAR);
}

int accRead(){
    ADMUX = (1<<REFS0)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0)|(0<<ADLAR);
    ADCSRA |= (1<<ADSC);

    while(bit_is_set(ADCSRA, ADSC));

    return ADC;
}