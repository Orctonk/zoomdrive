/*  FILE:			emdriver.h
 *  DESCRIPTION: 
 *      Interface for the emergenyc break component
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */
#ifndef __EMDRIVER_H__
#define __EMDRIVER_H__

#include <avr/io.h>

#define EM_DDR DDRD         // The data direction register for emergency module
#define EM_PORT PORTD       // The port of the emergency module
#define EM_PIN PIND         // The pins of the emergency module
#define EM_BUTTON_PIN PIN7  // The pin of the emergency button
#define EM_RELEASE_PIN PIN6 // The pin of the release button

/* 
 * Initializes the emergency break component. 
 */
void EM_Init(void);

/* 
 * Checks the emergency break and release buttons and sends messages when
 * the conditions are met. 
 */
void EM_Check(void);

#endif // __EMDRIVER_H__
