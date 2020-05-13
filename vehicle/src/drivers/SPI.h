/*
 * SPI.h
 *
 * Created: 2020-05-12
 *  Author: Mimmi
 */ 


#ifndef SPI_H_
#define SPI_H_
#include <avr/io.h>

/* 
 * Set SPI data direction and enable SPI.
 */ 
void SPI_init();

/*
 *Sends data thought SPI to the currently selected slave. 
 *
 *temp: The data to be sent.
 *
 *Returns: The data it receives.
 */
uint8_t SPI_transmit_recieve(uint8_t temp);


#endif /* SPI_H_ */