/*  FILE:           summer.h
 *  DESCRIPTION: 
 *      Declaration of functions for a piezosummer driver
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */

#ifndef __SUMMER_H__
#define __SUMMER_H__

#define NOTE_DURATION_MASK      (0b11100000)
#define NOTE_PITCH_MASK         (~NOTE_DURATION_MASK)

#define GET_DURATION_MULT(x)    ((x>>5) + 1)
#define GET_PITCH_EXP(x)        (x & NOTE_PITCH_MASK)

#include <avr/io.h>

// Pin definitions for the summer
#define SUMMER_DDR  DDRB
#define SUMMER_PIN  PIN1

// Initializes the summer
void Summer_Init();

void Summer_PlayMelody(const char *melody);

void Summer_PlayNote(uint8_t note);

#endif // __SUMMER_H__
