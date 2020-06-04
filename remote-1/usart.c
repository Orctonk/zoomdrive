#include <avr/io.h>
#include "define.h"

#define UBBR_VAL 12;

/*
 * Contains helperfunctions for the usart.
 *
 * Date: 2020-06-04
 * Author: Erika Lundin
 * */

/**
 * Initialize usart. 
 */
void usart_init(void) {
    UBRR0 = UBBR_VAL;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

/**
 * Sends a byte over the usart.
 */
void usart_write(int data) {
    while (!BIT_SET(UCSR0A, UDRE0));
    UDR0 = data;
}

/**
 * Sends a string over usart.
 */
void usart_write_string(const char* string) {
    while (*string != '\0') {
        usart_write(*string);
        string++;
    }
}
