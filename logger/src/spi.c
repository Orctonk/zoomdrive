/*  FILE:           spi.c
 *  DESCRIPTION: 
 *      Implementation of a simple SPI driver using polled data transfer
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */

#include "spi.h"
#include <avr/io.h>

// Initializes the SPI
void SPI_Init(void){
    // Set MOSI and SCK output, all others input
	DDRB = (1<<DDB5)|(1<<DDB3)|(1<<DDB2);
	// Enable SPI, Master, set clock rate fck/16
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

// Disables the SPI
void SPI_Disable(void){
    SPCR = 0;
}

// Transmits @data over SPI to the currently selected device
// and returns the returns the recieved data
uint8_t SPI_TransmitReceive(uint8_t data){
    SPDR = data;                
    while(!(SPSR & (1<<SPIF))); // Wait until transmission is completed
    return SPDR;
}