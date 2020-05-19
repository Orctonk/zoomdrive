
#include "emdriver.h"

#include "time.h"
#include "message.h"

#include <string.h>

uint32_t release_lastup;

void EM_Init(void) {
    EM_DDR &= ~((1<<EM_BUTTON_PIN)|(1<<EM_RELEASE_PIN));
    EM_PORT |= (1<<EM_BUTTON_PIN)|(1<<EM_RELEASE_PIN);
    release_lastup = 0;
}


void EM_Check(void) {
    uint32_t curtime = Time_GetMillis();
    if((EM_PIN & (1<<EM_BUTTON_PIN)) == 0){
        Message msg;
        msg.type = EMBUTTON;
        strcpy(msg.args[0],"1");
        Message_Send(msg,1);
    }
    if((EM_PIN & (1<<EM_RELEASE_PIN)) == 0){
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