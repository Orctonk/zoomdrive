/*  FILE:			emdriver.c
 *  DESCRIPTION: 
 *      Implementation of the emergency break and release buttons.
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */
#include "emdriver.h"

#include "time.h"
#include "message.h"

#include <string.h>

// The last timesstamp at which the release button was up.
uint32_t release_lastup;

/* 
 * Initializes the emergency break component.
 */
void EM_Init(void) {
    EM_DDR &= ~((1<<EM_BUTTON_PIN)|(1<<EM_RELEASE_PIN));
    EM_PORT |= (1<<EM_BUTTON_PIN)|(1<<EM_RELEASE_PIN);
    release_lastup = 0;
}

/* 
 * Checks the emergency break and release buttons and sends messages when
 * the conditions are met.
 */
void EM_Check(void) {
    uint32_t curtime = Time_GetMillis();
    // break button is pressed.
    if((EM_PIN & (1<<EM_BUTTON_PIN)) == 0){
        Message msg;
        msg.type = EMBUTTON;
        strcpy(msg.args[0],"1");
        Message_Send(msg,1);
    }
    // Release button is pressed.
    if((EM_PIN & (1<<EM_RELEASE_PIN)) == 0){
        // Button has been pressed for 3 secs.
        if(curtime - release_lastup >= 3000){
            Message msg;
            msg.type = EMBUTTON;
            strcpy(msg.args[0],"0");
            Message_Send(msg,1);
        }
    } else {
        release_lastup = curtime;
    }
}