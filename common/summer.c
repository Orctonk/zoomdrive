/*  FILE:           summer.c
 *  DESCRIPTION: 
 *      Implementation of a piezosummer driver using avr clocks to control output
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */

#include "summer.h"
#include <avr/interrupt.h>
#include <math.h>
#include <avr/pgmspace.h>

// Structural defines for notes
// 3MSB represent duration where binary value n represents a 2^n:th note 
// 5LSB represent pitch where 0 is the lowest pitch and 31 is the highest
#define NOTE_DURATION_MASK      (0b11100000)
#define NOTE_PITCH_MASK         (~NOTE_DURATION_MASK)

#define GET_DURATION_DIV(x)     (1<<(x>>5))
#define GET_PITCH_EXP(x)        ((x & NOTE_PITCH_MASK) - 1)

// Defines for pitch values
#define LOWC_OCR (F_CPU/520)    // Lowest possible note
#define ROOT12_2 1.05946309     // Multiply by twelfth root of 2 to get next note
                                // See https://en.wikipedia.org/wiki/Equal_temperament

// ------------- PRIVATE -------------

// Store all melodies in program memory to reduce SRAM usage
static const uint8_t melodies[MELODY_COUNT][MELODY_MAX_LEN] PROGMEM = {
    { // Basic Honk
        0x71, 0x80, 0x71, 0x00
    },
    { // Hemglass
        0x63, 0x6c, 0x6f, 0x6c, 0x48, 0x63, 0x6c, 0x6f, 0x6c, 0x48, 0x63, 0x6d, 
        0x60, 0x6d, 0x6a, 0x60, 0x6a, 0x48, 0x68, 0x00 
    },
    { // USSR
        0x2d, 0x4d, 0x60, 0x68, 0x4d, 0x68, 0x88, 0x8a, 0x4c, 0x85, 0x80, 0x65, 
        0x4a, 0x68, 0x88, 0x86, 0x48, 0x81, 0x80, 0x61, 0x63, 0x83, 0x80, 0x63, 
        0x83, 0x85, 0x66, 0x86, 0x80, 0x66, 0x86, 0x88, 0x4a, 0x6c, 0x6d, 0x4f, 
        0x6f, 0x68, 0x51, 0x6f, 0x8f, 0x8d, 0x4f, 0x6c, 0x68, 0x4d, 0x6c, 0x8c, 
        0x8a, 0x4c, 0x85, 0x80, 0x65, 0x4a, 0x68, 0x88, 0x86, 0x48, 0x61, 0x80, 
        0x81, 0x4d, 0x6c, 0x8c, 0x8a, 0x68, 0x6c, 0x6d, 0x6f, 0x31, 0x6f, 0x6d, 
        0x6c, 0x6d, 0x4f, 0x6f, 0x88, 0x80, 0x68, 0x6c, 0x6d, 0x6f, 0x2d, 0x6c, 
        0x6a, 0x68, 0x6a, 0x4c, 0x6c, 0x85, 0x80, 0x65, 0x68, 0x6a, 0x6c, 0x4d, 
        0x6a, 0x8a, 0x8c, 0x4d, 0x6a, 0x8a, 0x8c, 0x4d, 0x6a, 0x6d, 0x52, 0x72, 
        0x60, 0x32, 0x71, 0x6f, 0x6d, 0x6f, 0x51, 0x71, 0x8d, 0x80, 0x2d, 0x2f, 
        0x6d, 0x6c, 0x6a, 0x6c, 0x4d, 0x6d, 0x8a, 0x80, 0x2a, 0x4d, 0x6c, 0x8c, 
        0x8a, 0x48, 0x61, 0x80, 0x81, 0x28, 0x4a, 0x4c, 0x2d, 0x6d, 0x00 
    },
    { // Herr Gurka
        0x4a, 0x4c, 0x4e, 0x4f, 0x51, 0x53, 0x6a, 0x80, 0x8a, 0x4c, 0x6e, 0x80, 
        0x8e, 0x6f, 0x8f, 0x80, 0x4f, 0x40, 0x4a, 0x4c, 0x4e, 0x4f, 0x51, 0x53, 
        0x6a, 0x80, 0x8a, 0x4c, 0x6e, 0x80, 0x8e, 0x6f, 0x8f, 0x80, 0x4f, 0x40, 
        0x56, 0x53, 0x76, 0x96, 0x80, 0x76, 0x74, 0x31, 0x54, 0x51, 0x54, 0x36, 
        0x76, 0x96, 0x80, 0x56, 0x53, 0x76, 0x96, 0x80, 0x76, 0x74, 0x31, 0x4a, 
        0x4c, 0x4e, 0x2f, 0x4f, 0x00 
    },
    { // Blinka lilla stjärna
        0x61, 0x81, 0x80, 0x41, 0x68, 0x88, 0x80, 0x48, 0x6a, 0x8a, 0x80, 0x4a, 
        0x28, 0x66, 0x86, 0x80, 0x46, 0x65, 0x85, 0x80, 0x45, 0x63, 0x83, 0x80, 
        0x43, 0x21, 0x68, 0x88, 0x80, 0x48, 0x66, 0x86, 0x80, 0x46, 0x65, 0x85, 
        0x80, 0x45, 0x23, 0x68, 0x88, 0x80, 0x48, 0x66, 0x86, 0x80, 0x46, 0x65, 
        0x85, 0x80, 0x45, 0x23, 0x61, 0x81, 0x80, 0x41, 0x68, 0x88, 0x80, 0x48, 
        0x6a, 0x8a, 0x80, 0x4a, 0x28, 0x66, 0x86, 0x80, 0x46, 0x65, 0x85, 0x80, 
        0x45, 0x63, 0x83, 0x80, 0x43, 0x21, 0x00 
    }
};

#ifdef SUMMER_USE_SW
#include <stdbool.h>
static volatile bool rest = false;
#endif

// BPM for each melody
static const uint8_t bpms[MELODY_COUNT] = {180, 110, 75, 128, 100 };

// State variables for currently playing melody
static volatile uint16_t note_time = 0; // The timer cycles left for current note
static volatile uint8_t cur_melody;     // The index of the current melody
static volatile uint8_t current_note;   // The index of the curent note

// Plays a single note on the buzzer
void Summer_PlayNote(uint8_t note){
    if(GET_PITCH_EXP(note) == -1){  // Stop buzzer if note is a rest
#ifdef SUMMER_USE_SW
        rest = true;
#else
        TCCR1A &= ~(1<<COM1A0);
#endif
    } else{                         // Start buzzer and set correct frequency
#ifdef SUMMER_USE_SW
        rest = false;
#else
        TCCR1A |= (1<<COM1A0);
#endif
        OCR1A = LOWC_OCR / (pow(ROOT12_2,GET_PITCH_EXP(note)));
    }
    // cycles needed = frequency / (bar time) / (note length divisor)
    note_time = ((F_CPU/(OCR1A + 1))/(bpms[cur_melody]/(240.0)))/GET_DURATION_DIV(note);
}

// Interrupt vector to play next note after the current notes length is passed
// Stops the melody when it encounters a 0xFF
ISR(TIMER1_COMPA_vect){
#ifdef SUMMER_USE_SW
    if(rest)
        SW_SUMMER_PORT &= ~(1<<SUMMER_PIN);
    else
        SW_SUMMER_PORT ^= (1<<SUMMER_PIN);
#endif
    note_time--; 
    if (note_time == 0){
        PORTD ^= (1<<PIN3);
        current_note++;
        // Read next note from program memory
        uint8_t note = pgm_read_byte(&(melodies[cur_melody][current_note]));
        if(note == 0x00){   // Stop buzzer if 0xFF
            TIMSK1 = 0;
            TCCR1B &= ~(1<<CS10);
#ifdef SUMMER_USE_SW
            SW_SUMMER_PORT &= ~(1<<SUMMER_PIN);
#endif
        } else
            Summer_PlayNote(note);
    }
}

// ------------- PUBLIC -------------

// Initializes the summer
void Summer_Init(void){
    SUMMER_DDR |= (1<<SUMMER_PIN);
    // Start timer
#ifdef SUMMER_USE_SW
    TCCR1A = 0;
#else
    TCCR1A = (1<<COM1A0);
#endif
    TCCR1B = (1<<WGM12);
}

// Plays a specified melody on the buzzer
void Summer_PlayMelody(uint8_t melody){
    cur_melody = melody;
    current_note = 0;
    Summer_PlayNote(pgm_read_byte(&(melodies[melody][0])));
    TIMSK1 = (1<<OCIE1A);
    TCCR1B |= (1<<CS10);
}
