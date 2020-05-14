#include <avr/io.h>
#include "define.h"

#define UBBR_VAL 12;

void usart_init(void) {
    UBRR0 = UBBR_VAL;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

/**
 * Send a byte over the usart.
 * data - byte to send.
 */
void usart_write(int data) {
    while (!BIT_SET(UCSR0A, UDRE0));
    UDR0 = data;
}

/**
 * Send a string over usart.
 * string - string to send.
 */
void usart_write_string(const char* string) {
    while (*string != '\0') {
        usart_write(*string);
        string++;
    }
}
