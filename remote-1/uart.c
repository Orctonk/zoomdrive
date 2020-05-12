#include <avr/io.h>
#include "define.h"

void uartInit(void) {
    UBRR0 = UBBR_VAL;  
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
}

void uartSendData(int data) {
    while(!BIT_SET(UCSR0A, UDRE0));
    UDR0 = data;
}

void uartSendString(char* string) {
    while (*string != '\0') {
        uartSendData(*string);
        string++;
    }
}