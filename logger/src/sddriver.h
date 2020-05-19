/*  FILE:           sddriver.h
 *  DESCRIPTION: 
 *      Declarations for the SD-card driver functions
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */

#ifndef __SDDRIVER_H__
#define __SDDRIVER_H__

#include <avr/io.h>
#include <stdbool.h>

// Defines for the SD-card chip select pin
#define SD_PORT PORTB
#define SD_DDR  DDRB
#define SD_CS   PIN0

// Initializes the SD-card
void SD_Init(void);

// Writes a formated string to a connected SD-card
// Note that the final string should not be over
// 128 characters long including null terminator
void SD_Write(char *fmt, ...);

#endif // __SDDRIVER_H__
