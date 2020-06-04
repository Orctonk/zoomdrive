/*  FILE:			logger.c
 *  DESCRIPTION: 
 *      Implementation of the logger with OLED display and SD card logging.
 * 		Uses the u8glib library https://github.com/olikraus/u8glib for the OLED display.
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */

#include "logger.h"
#include "sddriver.h"
#include "oled/u8g.h"
#include "time.h"
#include "summer.h"
#include "bitmaps.h"

#include <stdlib.h>
#include <string.h>

// Check if button is pressed
#define STATUS_BTN_PRESS() (!(PINB & (1<<PIN6)))

// Represents the loggers status, keeping track of the cars sensors and engine.
typedef struct SystemStatus {
    int8_t carSpeed;
    int8_t carHeading;
	uint8_t carDistFront;
	uint8_t carDistBack;
    uint8_t carGear;
    bool embreaked;
	char odo[8];
	uint32_t car_lastupdate;
    uint32_t remote1_lasthb;
    uint32_t remote2_lasthb;
} SystemStatus;

u8g_t u8g;										// u8glib display struct
char logs[6][22] = {"1","2","3","4","5","6"};	// The six last messages
uint8_t nextlog = 1;							// The next message to overwrite
SystemStatus status;							// The current status of the system

/*
 * Writes string representation of the message into the destination buffer
 * 
 * dest:	The destination buffer
 * msg:		The message to be written
 */
void msgcat(char *dest, Message msg);

/*
 * Callback for message received event
 * 
 * msg:		The received message
 */
void messageCallback(Message msg);

/*
 * Initializes the logger and the OLED, initializing the system status
 */
void Logger_Init(void){
    status.carHeading = 0;
    status.carSpeed = 0;
    status.carGear = 1;
	status.odo[0] = '\0';
	status.carDistBack = 0;
	status.carDistFront = 0;
    status.embreaked = false;
    status.remote1_lasthb = 0;
    status.remote2_lasthb = 0;

    u8g_InitSPI(&u8g, &u8g_dev_ssd1306_128x64_2x_hw_spi, PN(1, 5), PN(1, 3), PN(1, 2), PN(1, 1), U8G_PIN_NONE);
    DDRB &= ~(1<<PIN6);
	PORTB |= (1<<PIN6);
    Message_Register(0xff,messageCallback);
}

/*
 * Updates the OLED display based on whether status button is pressed
 */
void Logger_UpdateDisplay(void) {
	// Store button state for use in draw loop
	bool displayStatus = STATUS_BTN_PRESS();
	// Store system time for use in draw loop	
    uint32_t t = Time_GetMillis();
	u8g_FirstPage(&u8g);
	do {
		u8g_SetFont(&u8g, u8g_font_6x10r);
		if(displayStatus){
			u8g_DrawStr(&u8g, 0, 10, "Status:");
            if(status.embreaked)
                u8g_DrawStr(&u8g,0,20, "BREAKED!");
			// Connectivity display
			// SD
            u8g_DrawStr(&u8g,67,10, "SD:");
            u8g_DrawStr(&u8g,85,10,SD_IsConnected() ? "Y" : "N");
			// Remote 1
            u8g_DrawStr(&u8g,102,10, "R1:");
            u8g_DrawStr(&u8g,120,10,t - status.remote1_lasthb > 3000 ? "N" : "Y");
			// Remote 2
            u8g_DrawStr(&u8g,102,20, "R2:");
            u8g_DrawStr(&u8g,120,20,t - status.remote2_lasthb > 3000 ? "N" : "Y");
			// Car
            u8g_DrawBitmapP(&u8g,26,32,8,28,carimage);
			if(t - status.car_lastupdate >= 5000){
				u8g_DrawBitmapP(&u8g,29,32,8,28,cardccross);
			} else {	// If car is connected
				if(status.carSpeed < 0){	// Reversing
					u8g_DrawBitmapP(&u8g,26,40,1,11,backarrow);
					if(status.carHeading == 1)
						u8g_DrawBitmapP(&u8g,27,55,1,4,turnarrowright);
					else if(status.carHeading == -1)
						u8g_DrawBitmapP(&u8g,27,33,1,4,turnarrowleft);
				}
				else if(status.carSpeed > 0){ // Forward
					for(uint8_t i = 0; i < status.carGear; i++)
						u8g_DrawBitmapP(&u8g,84 + 5*i,40,1,11,forwardarrow);
					if(status.carHeading == 1)
						u8g_DrawBitmapP(&u8g,83,55,1,4,turnarrowright);
					else if(status.carHeading == -1)
						u8g_DrawBitmapP(&u8g,83,33,1,4,turnarrowleft);
				}
				else {	// Standing still
					if(status.carHeading == 1){ // Spin right
						u8g_DrawBitmapP(&u8g,83,55,1,4,turnarrowright);
						u8g_DrawBitmapP(&u8g,27,33,1,4,turnarrowleft);
					}
					else if(status.carHeading == -1){	// Spin left
						u8g_DrawBitmapP(&u8g,83,33,1,4,turnarrowleft);
						u8g_DrawBitmapP(&u8g,27,55,1,4,turnarrowright);
					}
				}
				// Draw sensor data
				char buf[6];
				// Odometer
				u8g_DrawStr(&u8g,0,30, "ODO:");
				u8g_DrawStr(&u8g,24,30, status.odo);
				// Back distance
				itoa(status.carDistBack,buf,10);
				u8g_DrawBitmapP(&u8g,0,40,3,11,backdistarrow);
				u8g_DrawStr(&u8g,0,60, buf);
				// Front distance
				itoa(status.carDistFront,buf,10);
				u8g_DrawBitmapP(&u8g,104,40,3,11,frontdistarrow);
				u8g_DrawStr(&u8g,104,60, buf);
			}
		}
		else{
			// Print rolling log based on next messag eto overwrite
			for(uint8_t i = 0; i < 6; i++){
				u8g_DrawStr(&u8g, 0, 10*(6-i), logs[(nextlog + i) % 6]);
			}
		}
	} while ( u8g_NextPage(&u8g) );
}

/*
 * Callback for message received event
 * 
 * msg:		The received message
 */
void messageCallback(Message msg) {
	// Log message to OLED
	msgcat(logs[nextlog], msg);

	// Update car connectivity if message is from car
	if(msg.type & FROMCAR)
		status.car_lastupdate = Time_GetMillis();

	// Handle message based on type
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
		status.carDistFront = atoi(msg.args[0]);
		status.carDistBack = atoi(msg.args[1]);
		strcpy(status.odo,msg.args[2]);
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
	case HONK:
		if(msg.args[0][0] == '2')
			Summer_PlayMelody(msg.args[1][0] - '0');
    default:
        break;
    }

	// Log to SD if connected
	if(SD_IsConnected()){
		uint32_t t = Time_GetMillis();
		SD_Write(t, msg.type, logs[nextlog]);
	}

	// Move log head
	nextlog = (nextlog + 1) % 6;
}

/*
 * Writes string representation of the message into the destination buffer
 * 
 * dest:	The destination buffer
 * msg:		The message to be written
 */
void msgcat(char *dest, Message msg){
	int argc = 0;
	switch(msg.type){
	case ENGINE_POWER:
		strcpy_P(dest,PSTR("ENGINEPOWERSET"));
		argc = 1;
		break;
	case HEADING:
		strcpy_P(dest,PSTR("ENGINEHEADINGSET"));
		argc = 1;
		break;
	case EMBUTTON:
		strcpy_P(dest,PSTR("EMBREAKSET"));
		argc = 1;
		break;
	case DEADMAN:
		strcpy_P(dest,PSTR("DEADMANSET"));
		argc = 2;
		break;
	case HONK:
		strcpy_P(dest,PSTR("HONKED!"));
		argc = 2;
		break;
	case PING:
		strcpy_P(dest,PSTR("PING"));
		break;
	case HEARTBEAT:
		strcpy_P(dest,PSTR("HEARTBEAT"));
		argc = 1;
		break;
	case CSTSTRING:
		strcpy_P(dest,PSTR("CSTSTRING"));
		argc = 1;
		break;
	case GEAR:
		strcpy_P(dest,PSTR("GEARSET"));
		argc = 1;
		break;
	case UPDATE_STATE:
		strcpy_P(dest,PSTR("CARSTATE"));
		argc = 3;
		break;
	case PONG:
		strcpy_P(dest,PSTR("PONG"));
		break;
	case CARBUTTON:
		strcpy_P(dest,PSTR("CARBTNPRESS"));
		argc = 1;
		break;
	case COLLISION:
		strcpy_P(dest,PSTR("COLLISION"));
		argc = 2;
		break;
	case CMD:
		strcpy_P(dest,PSTR("MANEUVER"));
		argc = 1;
		break;
	case UPDATE_SENSORS:
		strcpy_P(dest,PSTR("SENSORS"));
		argc = 3;
		break;
	case UPDATE_EM:
		strcpy_P(dest,PSTR("EMSTATE"));
		argc = 1;
		break;
	default:
		break;
	}
	
	// Find end
	uint8_t pos = 0;
	while(dest[pos] != '\0') pos++;

	// Add arguments
	for(uint8_t i = 0; i < argc; i++){
		dest[pos++] = ' ';
		for(uint8_t j = 0; msg.args[i][j] != '\0'; j++)
			dest[pos++] = msg.args[i][j];
	}
	// Terminate
	dest[pos] = '\0';
}