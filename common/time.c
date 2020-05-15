#include <avr/interrupt.h>
#include "time.h"

// ------------- PRIVATE -------------

#ifndef TIME_CALIBRATION_FACTOR
#define TIME_CALIBRATION_FACTOR 1
#endif

// Raw timer ticks since system startup
static volatile uint32_t ms = 0;
static volatile uint32_t cbtime = 0;
static volatile uint32_t lastcb = 0;
static TimerCallback callback = (void*)0;

// Timer interrupt vector
ISR(TIMER2_COMPA_vect){
    ms++;
    if(callback != (void*)0 && ms-lastcb >= cbtime){
        callback();
        lastcb = ms;
    }
}

// ------------- PUBLIC -------------

// Initializes time module
void Time_Init(){
    // Start timer 
    TCCR2B = (0<<CS20)|(0<<CS21)|(1<<CS22);
    TCCR2A = (1<<WGM21);
    OCR2A = 125 * TIME_CALIBRATION_FACTOR;
    TIMSK2 |= (1<<OCIE2A);
}

// Gets the time since system startup
time Time_Get(){
    time t;
    uint32_t secs = Time_GetMillis() / 1000; // Get seconds
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

void Time_RegisterTimer(uint32_t cbperiod, TimerCallback cb) {
    callback = cb;
    cbtime = cbperiod;
}