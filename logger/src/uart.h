/*  FILE:           uart.h
 *  DESCRIPTION: 
 *      UART utility functions for debugging, disabled when DEBUG is
 *      not defined.
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */

#ifndef __UART_H__
#define __UART_H__

#include <avr/io.h>
#ifdef DEBUG
#include <stdio.h>

// Puts a single char onto the specified FILE stream
static int UART_PutChar(char c, FILE *stream){
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = c;
    return 0;
}

// Create a static FILE stream using UART_PutChar for output
static FILE mystdout = FDEV_SETUP_STREAM(UART_PutChar,NULL,_FDEV_SETUP_WRITE);
#endif

// Initializes the UART
// Is a nop in release mode
void UART_Init(uint16_t baud){
#ifdef DEBUG
    uint16_t bsd = (F_CPU/16/baud) - 1;

    PRR &= ~(1<<PRUSART0);
    UBRR0 = bsd;

    UCSR0B = (1<<TXEN0);
    UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);

    stdout = &mystdout;
#endif
}

// Initializes the UART
// Is a nop in release mode
void UART_Disable(){
#ifdef DEBUG
    while(!(UCSR0A & (1<<UDRE0)));
    UCSR0B = 0;
#endif
    UCSR0A = 0;
}

#ifndef DEBUG
// Adds a dummy printf in release mode
int printf(char *fmt,...){}
#endif

#endif // __UART_H__
