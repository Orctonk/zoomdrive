/*  FILE:			logger.h
 *  DESCRIPTION: 
 *      Logger interface.
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "message.h"

/*
 * Initializes the logger and the OLED, initializing the system status
 */
void Logger_Init(void);

/*
 * Updates the OLED display based on whether status button is pressed
 */
void Logger_UpdateDisplay(void);

#endif // __LOGGER_H__
