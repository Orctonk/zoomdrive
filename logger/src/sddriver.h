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
#define SD_PORT         PORTB
#define SD_DDR          DDRB
#define SD_CS           PIN0
#define SD_POWER_PIN    PIN7

/*
 * Initializes SD-card
 */
void SD_Init(void);

/*
 * Updates state of SD-card depending on toggle
 */
void SD_Check(void);

/*
 * Writes a formated string to a connected SD-card
 * Note that the final string should not be over
 * 128 characters long including null terminator
 * 
 * time:    The timestamp of the logged message
 * type:    The integer type of the message
 * msg:     The string representation of the message
 */
void SD_Write(uint32_t time, uint8_t type, char *msg);

/* 
 *Checks whether the SD card is connected
 *
 * Return:  True if SD is connected or false otherwise
 */
bool SD_IsConnected(void);

#endif // __SDDRIVER_H__
