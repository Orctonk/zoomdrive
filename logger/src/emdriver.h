#ifndef __EMDRIVER_H__
#define __EMDRIVER_H__

#include <avr/io.h>

#define EM_DDR DDRD
#define EM_PORT PORTD
#define EM_PIN PIND
#define EM_BUTTON_PIN PIN7
#define EM_RELEASE_PIN PIN6

void EM_Init(void);
void EM_Check(void);

#endif // __EMDRIVER_H__
