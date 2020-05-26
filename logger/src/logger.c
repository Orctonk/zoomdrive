#include "logger.h"
#include "sddriver.h"
#include "oled/u8g.h"
#include "time.h"

#include <stdlib.h>
#include <string.h>

#define STATUS_BTN_PRESS() (!(PINB & (1<<PIN6)))

#include "bitmaps.h"

typedef struct SystemStatus {
    int8_t carSpeed;
    int8_t carHeading;
	char carDistFront[6];
	char carDistBack[6];
    uint8_t carGear;
    bool embreaked;
    uint32_t remote1_lasthb;
    uint32_t remote2_lasthb;
} SystemStatus;

u8g_t u8g;
char logs[6][22] = {"1","2","3","4","5","6"};
uint8_t nextlog = 1;
SystemStatus status;

void msgcat(char *dest, Message msg);
void messageCallback(Message msg);

void Logger_Init(void){
    status.carHeading = 0;
    status.carSpeed = 0;
    status.carGear = 1;
    status.embreaked = false;
    status.remote1_lasthb = 0;
    status.remote2_lasthb = 0;

    u8g_InitSPI(&u8g, &u8g_dev_ssd1306_128x64_2x_hw_spi, PN(1, 5), PN(1, 3), PN(1, 2), PN(1, 1), U8G_PIN_NONE);
    DDRB &= ~(1<<PIN6);
	PORTB |= (1<<PIN6);
    Message_Register(0xff,messageCallback);
}

void Logger_UpdateDisplay(void) {
	bool displayStatus = STATUS_BTN_PRESS();
    uint32_t t = Time_GetMillis();
	u8g_FirstPage(&u8g);
	do {
		u8g_SetFont(&u8g, u8g_font_6x10r);
		if(displayStatus){
			u8g_DrawStr(&u8g, 0, 10, "Status:");
            if(status.embreaked)
                u8g_DrawStr(&u8g,0,20, "BREAKED!");
            u8g_DrawStr(&u8g,67,10, "SD:");
            u8g_DrawStr(&u8g,85,10,SD_IsConnected() ? "Y" : "N");
            u8g_DrawStr(&u8g,102,10, "R1:");
            u8g_DrawStr(&u8g,120,10,t - status.remote1_lasthb > 3000 ? "N" : "Y");
            u8g_DrawStr(&u8g,102,20, "R2:");
            u8g_DrawStr(&u8g,120,20,t - status.remote2_lasthb > 3000 ? "N" : "Y");
            u8g_DrawBitmapP(&u8g,26,32,8,28,carimage);
			if(status.carSpeed < 0){
				u8g_DrawBitmapP(&u8g,26,40,1,11,backarrow);
				if(status.carHeading == 1)
					u8g_DrawBitmapP(&u8g,27,55,1,4,turnarrowright);
				else if(status.carHeading == -1)
					u8g_DrawBitmapP(&u8g,27,33,1,4,turnarrowleft);
			}
			else if(status.carSpeed > 0){
				for(uint8_t i = 0; i < status.carGear; i++)
			//	for(uint8_t i = 0; i < 3; i++)
					u8g_DrawBitmapP(&u8g,84 + 5*i,40,1,11,forwardarrow);
				if(status.carHeading == 1)
					u8g_DrawBitmapP(&u8g,83,55,1,4,turnarrowright);
				else if(status.carHeading == -1)
					u8g_DrawBitmapP(&u8g,83,33,1,4,turnarrowleft);
			}
			u8g_DrawBitmapP(&u8g,0,40,3,11,backdistarrow);
			u8g_DrawStr(&u8g,0,60, status.carDistBack);
			u8g_DrawBitmapP(&u8g,104,40,3,11,frontdistarrow);
			u8g_DrawStr(&u8g,104,60, status.carDistFront);
		}
		else{
			for(uint8_t i = 0; i < 6; i++){
				u8g_DrawStr(&u8g, 0, 10*(6-i), logs[(nextlog + i) % 6]);
			}
		}
	} while ( u8g_NextPage(&u8g) );
}

void messageCallback(Message msg) {
	msgcat(logs[nextlog], msg);

    switch(msg.type){
    case UPDATE_EM:
        status.embreaked = msg.args[0][0] != '0';
        break;
	case UPDATE_STATE:
		status.carSpeed = atoi(msg.args[0]);
		status.carHeading = atoi(msg.args[1]);
		status.carGear = atoi(msg.args[2]);
		break;
	case UPDATE_SENSORS:
		strcpy(status.carDistFront,msg.args[0]);
		strcpy(status.carDistBack,msg.args[1]);
		break;
    case ENGINE_POWER:
        status.carSpeed = atoi(msg.args[0]);
        break;
    case HEADING:
        status.carHeading = atoi(msg.args[0]);
        break;
    case GEAR:
        status.carGear = atoi(msg.args[0]);
        break;
    case EMBUTTON:
        status.embreaked = msg.args[0][0] != '0';
        break;
    case HEARTBEAT:
        if(msg.args[0][0] == '1')
            status.remote1_lasthb = Time_GetMillis();
        else if(msg.args[0][0] == '2')
            status.remote2_lasthb = Time_GetMillis();
        break;
    default:
        break;
    }

	if(SD_IsConnected()){
		uint32_t t = Time_GetMillis();
		SD_Write(t, msg.type, logs[nextlog]);
	}

	nextlog = (nextlog + 1) % 6;
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
	case UPDATE_STATE:
		progStr = PSTR("CARSTATE");
		argc = 3;
		break;
	case PONG:
		progStr = PSTR("PONG");
		break;
	case CARBUTTON:
		progStr = PSTR("CARBTNPRESS");
		argc = 1;
		break;
	case COLLISION:
		progStr = PSTR("COLLISION");
		argc = 2;
		break;
	case UPDATE_SENSORS:
		progStr = PSTR("SENSORS");
		argc = 2;
		break;
	case UPDATE_EM:
		progStr = PSTR("EMSTATE");
		argc = 1;
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