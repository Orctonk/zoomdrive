/*  FILE:           spi.h
 *  DESCRIPTION: 
 *      Declarations for functions of a simple SPI driver
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */

#ifndef __SPI_H__
#define __SPI_H__

#include <stdint.h>

// Initializes the SPI
void SPI_Init(void);

// Disables the SPI
void SPI_Disable(void);

// Transmits @data over SPI to the currently selected device
// and returns the returns the recieved data
uint8_t SPI_TransmitReceive(uint8_t data);

#endif