/*  FILE:           time.h
 *  DESCRIPTION: 
 *      Declarations for time-keeping utilities
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */

#ifndef __TIME_H__
#define __TIME_H__

#include <stdint.h>

typedef void(*TimerCallback)(void);

// Type to store time
typedef struct time {
    uint8_t hrs;
    uint8_t mins;
    uint8_t secs;
} time;

// Initializes time module
void Time_Init(void);

// Gets the time since system startup
time Time_Get(void);

// Gets the number of milliseconds since system startup
uint32_t Time_GetMillis(void);

// Register a timer callback which is called with a period of
// at least @ms milliseconds.
void Time_RegisterTimer(uint32_t ms, TimerCallback);

// Call timer callback if enough time has passed
// Infrequent calls to this function results in errors to the period of the 
// registered callback
void Time_Update(void);

#endif // __TIME_H__
