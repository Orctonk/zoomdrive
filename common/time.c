#include <avr/interrupt.h>
#include "./time.h"

// ------------- PRIVATE -------------

// Raw timer ticks since system startup
static volatile uint32_t ms = 0;

// Timer interrupt vector
ISR(TIMER2_OVF_vect){
    ms++;
}

// ------------- PUBLIC -------------

// Initializes time module
void Time_Init(){
    // Start timer 
    TCCR2B = (1<<CS20)|(0<<CS21)|(0<<CS22);
    TCCR2A = (1<<WGM21);
    OCR0A = 125;
    TIMSK2 |= (1<<TOIE2);
}

// Gets the time since system startup
time Time_Get(){
    time t;
    uint32_t secs = Time_Getmillis() / 10; // Get seconds
    // Reduce stepwise
    t.hrs = secs / (60 * 60);
    secs -= t.hrs * (60 * 60);
    t.mins = secs / 60;
    secs -= t.mins * 60;
    t.secs = secs;
    return t;
}

// Gets the number of seconds since system startup
uint32_t Time_GetMillis(){
    return ms ;
}