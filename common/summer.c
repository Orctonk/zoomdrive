/*  FILE:           summer.c
 *  DESCRIPTION: 
 *      Implementation of a piezosummer driver using avr clocks to control output
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */

#include "summer.h"
#include <avr/interrupt.h>
#include <string.h>
#include <math.h>

#define LOWC_OCR (F_CPU/520)
#define SQRT_12 1.05946309

// ------------- PRIVATE -------------

// State variables for periodic buzzing
static volatile uint16_t note_time = 0;
static volatile char cur_melody[128];
static volatile uint8_t current_note;

void Summer_PlayNote(uint8_t note){
    OCR1A = LOWC_OCR / (pow(SQRT_12,GET_PITCH_EXP(note)));
    note_time = ((F_CPU/(OCR1A + 1))*0.206)*GET_DURATION_MULT(note);
    //((8000000/((8000000/260)/4))-1)*0.833
}

// Interrupt vector to toggle buzzer output periodically based on settings
ISR(TIMER1_COMPA_vect){
    note_time--; 
    if (note_time == 0){
        current_note++;
        if(cur_melody[current_note] == '\0'){
            TIMSK1 = 0;
            TCCR1B &= ~(1<<CS10);
        } else
            Summer_PlayNote(cur_melody[current_note]);
    }
}

// ------------- PUBLIC -------------

// Initializes the summer
void Summer_Init(){
    SUMMER_DDR |= (1<<SUMMER_PIN);
    // Start timer
    TCCR1A = (1<<COM1A0);
    TCCR1B = (1<<WGM12);
}

void Summer_PlayMelody(const char *melody){
    strcpy(cur_melody,melody);
    current_note = 0;
    Summer_PlayNote(melody[0]);
    TIMSK1 = (1<<OCIE1A);
    TCCR1B |= (1<<CS10);
}
