#include "define.h"
#include <stddef.h>
#include <avr/io.h>
#include <util/delay.h>

/*
 * Contains helperfunctions for the LCD.  
 *
 * Author: Jakob Lundkvist
 * */

/**
 * Sends and recieves data from the LCD.
 */ 
uint8_t SPI_TransmitRecieve(uint8_t data) {
    PORTB &= ~(1UL << PORTB2);
    SPDR = data;
    while (!BIT_SET(SPSR, SPIF));
    uint8_t retData = SPDR;
    PORTB |= (1UL<<PORTB2);

    return retData;
}

/**
 * Writes a byte to the LCD.
 */ 
void writeData(uint8_t data) {

    PORTB |= (1UL<<1);
    SPI_TransmitRecieve(data);
    _delay_us(40);
    PORTB &= ~(1UL<<1);
}

/**
 * Writes a string to the LCD.
 */ 
void writeString(char* string) {
    while (*string != '\0') {
        writeData(*string++);
    }
}

/**
 * Moves the cursor to the spot specified in the argument. 
 */ 
void moveCursor(uint8_t data) {
    SPI_TransmitRecieve(data);
    _delay_us(40);
}

/**
 * Clears the display.
 */ 
void clearDisplay(void) {
    SPI_TransmitRecieve(0x01);
    _delay_ms(2);
}

void writeTemp(int tous, int huns, int hTemp, int lTemp, int dec) {
    writeData(tous);
    writeData(huns);
    writeData(hTemp);
    writeData(lTemp);
    writeData(',');
    writeData(dec);
    writeData(' ');
    _delay_us(40);
}

/**
 * Initializes the LCD assuming it runs on 3.3V.
 */ 
void lcdInit(void){

    DDRB = 0xff;
    SPCR |= (1UL<<SPE) | (1UL<<MSTR) | (1UL<<SPR0);


    _delay_ms(50);
    SPI_TransmitRecieve(0x39);
    _delay_us(40);
    SPI_TransmitRecieve(0x1D);
    _delay_us(40);
    SPI_TransmitRecieve(0x50);
    _delay_us(40);
    SPI_TransmitRecieve(0x6C);
    _delay_us(40);
    SPI_TransmitRecieve(0x7C);
    _delay_us(40);
    SPI_TransmitRecieve(0x38);
    _delay_us(40);
    SPI_TransmitRecieve(0x0F);
    _delay_us(40);
    SPI_TransmitRecieve(0x01);
    _delay_ms(2);
    SPI_TransmitRecieve(0x06);
    _delay_us(40);
    return;
}
