/*  FILE:			main.c
 *  DESCRIPTION: 
 *      Main source file for the logger component of the zoomdrive project
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */

#include <avr/interrupt.h>
#include <avr/io.h>

#include "message.h"
#include "time.h"
#include "emdriver.h"
#include "sddriver.h"
#include "logger.h"
#include "summer.h"

/*
 * Initialize logger unit, intializing all subcomponents
 */ 
void init(void);

/*
 * Callback for the timer tick event which should be called once every second
 */
void timerCallback(void);

/*
 * Main loop
 */
int main(void) {
	init();	// Initialize
	while(1){	// Main loop
		SD_Check();				// Check SD connection and connect/disconnect
		Time_Update();			// Trigger timer event if condition is met
		Message_Update();		// Handle incoming messages
		EM_Check();				// Check the emergency buttons
		Logger_UpdateDisplay();	// Update the OLED with new info
	} 
}

/*
 * Initialize logger unit, intializing all subcomponents
 */ 
void init(void){
	Message_Init(9600);
	Time_Init();
	// Register timer with frequency of 1 sec
	Time_RegisterTimer(1000,timerCallback);
	EM_Init();
	Logger_Init();
	SD_Init();
	Summer_Init();
	DDRD |= (1<<PIN3);
	sei();
}

/*
 * Callback for the timer tick event which should be called once every second
 */
void timerCallback(void){
	// Send heartbeat
	Message msg;
	msg.type = HEARTBEAT;
	msg.args[0][0] = '0';
	msg.args[0][1] = '\0';
	Message_Send(msg,1);
}