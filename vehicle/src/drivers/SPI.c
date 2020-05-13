/*
 * SPI.c
 *
 * Created: 2020-05-12
 *  Author: Mimmi
 */ 
#include "SPI.h"

/* 
 * Set SPI data direction and enable SPI.
 */ 
void SPI_init(){
	DDRB |= (1<< DDB3)|(1<<DDB5)|(1<<DDB2);
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

/*
 *Sends data thought SPI to the currently selected slave. 
 *
 *temp: The data to be sent.
 *
 *Returns: The data it receives.
 */
uint8_t SPI_transmit_recieve(uint8_t temp){
	SPDR = temp; 
	while (!(SPSR & (1<< SPIF)));
	return SPDR; 
}