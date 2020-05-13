/*  FILE:           time.h
 *  DESCRIPTION: 
 *      Declarations for time-keeping utilities
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */

#ifndef __TIME_H__
#define __TIME_H__

#include <stdint.h>

// Type to store time
typedef struct time {
    uint8_t hrs;
    uint8_t mins;
    uint8_t secs;
} time;

// Initializes time module
void Time_Init();

// Gets the time since system startup
time Time_Get();

// Gets the number of milliseconds since system startup
uint32_t Time_GetMillis();

#endif // __TIME_H__
