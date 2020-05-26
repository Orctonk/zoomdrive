#include <avr/interrupt.h>
#include <avr/io.h>

#include "message.h"
#include "time.h"
#include "emdriver.h"
#include "sddriver.h"
#include "logger.h"

void init(void);
void timerCallback(void);
void msgcat(char *dest, Message msg);

int main(void)
{
	init();
	while(1){
		SD_Check();
		Time_Update();
		Message_Update();
		EM_Check();
		Logger_UpdateDisplay();
	} 
}

void init(void){
	Message_Init(9600);
	Time_Init();
	Time_RegisterTimer(1000,timerCallback);
	EM_Init();
	Logger_Init();
	SD_Init();
	sei();
}

void timerCallback(void){
	Message msg;
	msg.type = HEARTBEAT;
	msg.args[0][0] = '0';
	msg.args[0][1] = '\0';
	Message_Send(msg,1);
}