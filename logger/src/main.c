#include <avr/interrupt.h>
#include <avr/io.h>

#include "oled/u8g.h"
#include "message.h"
#include "time.h"
#include "emdriver.h"
#include "sddriver.h"

u8g_t u8g;
char logs[6][22] = {"1","2","3","4","5","6"};
uint8_t nextlog = 1;

void init(void);
void mqttCallback(Message msg);
void timerCallback(void);
void updateDisplay(void);
void msgcat(char *dest, Message msg);

int main(void)
{
	init();
	while(1){
		SD_Check();
		Time_Update();
		Message_Update();
		EM_Check();
		UpdateDisplay();
	} 
}

void init(void){
	Message_Init(9600);
	Time_Init();
	Time_RegisterTimer(1000,timerCallback);
	Message_Register(0xff,mqttCallback);
	EM_Init();
	u8g_InitSPI(&u8g, &u8g_dev_ssd1306_128x64_2x_hw_spi, PN(1, 5), PN(1, 3), PN(1, 2), PN(1, 1), U8G_PIN_NONE);
	SD_Init();
	sei();
}

void mqttCallback(Message msg){
	msgcat(logs[nextlog], msg);

	if(SD_IsConnected()){
		PORTD |= (1<<PIN2);
		uint32_t t = Time_GetMillis();
		SD_Write(t, msg.type, logs[nextlog]);
	}

	nextlog = (nextlog + 1) % 6;
}

void timerCallback(void){
	Message msg;
	msg.type = HEARTBEAT;
	msg.args[0][0] = '0';
	msg.args[0][1] = '\0';
	Message_Send(msg,1);
}

void UpdateDisplay(void) {
	u8g_FirstPage(&u8g);
		do {
			u8g_SetFont(&u8g, u8g_font_6x10r);
			for(uint8_t i = 0; i < 6; i++){
				u8g_DrawStr(&u8g, 0, 10*(6-i), logs[(nextlog + i) % 6]);
			}
		} while ( u8g_NextPage(&u8g) );
}

void msgcat(char *dest, Message msg){
	const char *progStr PROGMEM;
	int argc = 0;
	switch(msg.type){
	case ENGINE_POWER:
		progStr = PSTR("ENGINEPOWERSET");
		argc = 1;
		break;
	case HEADING:
		progStr = PSTR("ENGINEHEADINGSET");
		argc = 1;
		break;
	case EMBUTTON:
		progStr = PSTR("EMBREAKSET");
		argc = 1;
		break;
	case DEADMAN:
		progStr = PSTR("DEADMANSET");
		argc = 2;
		break;
	case HONK:
		progStr = PSTR("HONKED!");
		argc = 2;
		break;
	case PING:
		progStr = PSTR("PING");
		break;
	case HEARTBEAT:
		progStr = PSTR("HEARTBEAT");
		argc = 1;
		break;
	case CSTSTRING:
		progStr = PSTR("CSTSTRING");
		argc = 1;
		break;
	case GEAR:
		progStr = PSTR("GEARSET");
		argc = 1;
		break;
	case UPDATE:
		progStr = PSTR("CARSTATE");
		argc = 3;
		break;
	case PONG:
		progStr = PSTR("PONG");
		break;
	case CARBUTTON:
		progStr = PSTR("CARBTNPRESS");
		break;
	default:
		break;
	}

	strcpy_P(dest,progStr);
	uint8_t pos = 0;
	while(dest[pos] != '\0') pos++;

	for(uint8_t i = 0; i < argc; i++){
		dest[pos++] = ' ';
		for(uint8_t j = 0; msg.args[i][j] != '\0'; j++)
			dest[pos++] = msg.args[i][j];
	}
	dest[pos] = '\0';
}