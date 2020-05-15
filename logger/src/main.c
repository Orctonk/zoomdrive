#include <avr/interrupt.h>
#include <avr/io.h>
#include "oled/u8g.h"
#include "message.h"
#include "time.h"
#include "string.h"
#include "stdlib.h"
#include <stdio.h>

u8g_t u8g;
char log[6][22] = {"1","2","3","4","5","6"};
int nextlog = 1;

void doLog(const char *msg){
	uint32_t t = Time_GetMillis();
	snprintf(log[nextlog],"%s",msg);
	nextlog = (nextlog + 1) % 6;
}

void mqttCallback(Message msg){
	time t = Time_Get();

}

void draw(void) {
    u8g_SetFont(&u8g, u8g_font_6x10);
    for(int i = 0; i < 6; i++){
      	u8g_DrawStr(&u8g, 0, 10*(6-i), log[(nextlog + i) % 6]);
    }
}

int main(void)
{
	Time_Init();
	sei();
	/*
		CS: PORTB, Bit 2 --> PN(1,2)
		A0: PORTB, Bit 1 --> PN(1,1)
		SCK: PORTB, Bit 5 --> PN(1,5)
		MOSI: PORTB, Bit 3 --> PN(1,3)
	*/
	u8g_InitSPI(&u8g, &u8g_dev_ssd1306_128x64_2x_hw_spi, PN(1, 5), PN(1, 3), PN(1, 2), PN(1, 1), U8G_PIN_NONE);
	uint8_t secs = 0;
	for(;;){
		time t = Time_Get();
		if(t.secs != secs){
			secs = t.secs;
			doLog("hej");
		}
		u8g_FirstPage(&u8g);
		do {
			draw();
		} while ( u8g_NextPage(&u8g) );
		u8g_Delay(100);
	} 
}