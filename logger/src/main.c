#include <avr/interrupt.h>
#include <avr/io.h>
//#include <string.h>
//#include <stdlib.h>
#include <stdio.h>

#include "oled/u8g.h"
#include "message.h"
#include "time.h"
#include "emdriver.h"
#include "sddriver.h"
#include "spi.h"

u8g_t u8g;
char log[6][22] = {"1","2","3","4","5","6"};
int nextlog = 1;

void doLog(MessageType type,const char *fmt,...){
	va_list vargs;
	va_start(vargs,fmt);
	uint32_t t = Time_GetMillis();
	vsnprintf(log[nextlog],22,fmt,vargs);
	SD_Write("%lu %u %s\n", t, type, log[nextlog]);
	nextlog = (nextlog + 1) % 6;
}

void mqttCallback(Message msg){
	switch(msg.type){
	case ENGINE_POWER:
		doLog(msg.type,"ENGINEPOWERSET %s", msg.args[0]);
		break;
	case HEADING:
		doLog(msg.type,"ENGINEHEADINGSET %s", msg.args[0]);
		break;
	case EMBUTTON:
		doLog(msg.type,"EMBREAKSET %s", msg.args[0]);
		break;
	case DEADMAN:
		doLog(msg.type,"DEADMANSET %s %s", msg.args[0],msg.args[1]);
		break;
	case HONK:
		doLog(msg.type,"HONKED!");
		break;
	case PING:
		doLog(msg.type,"PING");
		break;
	case EMSTATE:
		doLog(msg.type,"EMSTATEGET %s",msg.args[0]);
		break;
	case SPEED:
		doLog(msg.type,"ENGINEPOWERGET %s", msg.args[0]);
		break;
	case DISTANCE:
		doLog(msg.type,"CARDISTANCEGET %s", msg.args[0]);
		break;
	case PONG:
		doLog(msg.type,"PONG");
		break;
	case CARBUTTON:
		doLog(msg.type,"CARBTNPRESS");
		break;
	case HEARTBEAT:
		doLog(msg.type,"HEARTBEAT %s", msg.args[0]);
		break;
	default:
		break;
	}
}

void timerCallback(void){
	EM_Check();
	Message msg;
	msg.type = HEARTBEAT;
	msg.args[0][0] = '0';
	msg.args[0][1] = '\0';
	Message_Send(msg,1);
}

void draw(void) {
    u8g_SetFont(&u8g, u8g_font_6x10);
    for(int i = 0; i < 6; i++){
      	u8g_DrawStr(&u8g, 0, 10*(6-i), log[(nextlog + i) % 6]);
    }
}

void init(void){
	Message_Init(9600);
	Time_Init();
	Time_RegisterTimer(1000,timerCallback);
	Message_Register(0xff,mqttCallback);
	EM_Init();
	u8g_InitSPI(&u8g, &u8g_dev_ssd1306_128x64_2x_sw_spi, PN(1, 6), PN(1, 7), PN(1, 2), PN(1, 1), U8G_PIN_NONE);
	SPI_Init();
	SD_Init();

	DDRD |= (1<<PIN2);
}

int main(void)
{
	init();
	sei();
	/*
		CS: PORTB, Bit 2 --> PN(1,2)
		A0: PORTB, Bit 1 --> PN(1,1)
		SCK: PORTB, Bit 5 --> PN(1,5)
		MOSI: PORTB, Bit 3 --> PN(1,3)
	*/
	
	for(;;){
		cli();
		u8g_FirstPage(&u8g);
		do {
			draw();
		//} while(1);
		} while ( u8g_NextPage(&u8g) );
		sei();
		u8g_Delay(1000);
	} 
}