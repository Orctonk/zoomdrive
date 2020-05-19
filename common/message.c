#include "message.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static volatile MessageTopic listen_topic;
static volatile MessageCallback callback;
static volatile bool skip;

static char UART_GetChar(void){
    while ( !(UCSR0A & (1<<RXC0)) ) {}
	/* Get and return received data from buffer */
	return UDR0;
}

static int UART_PutChar(char c){
	while(!(UCSR0A & (1<<UDRE0))){}
	UDR0 = c;

	return 0;
}

static void UART_ReadLine(char *buf){
    for(int j = 0; j<ARGSIZE*4 - 1; j++){
        buf[j] = UART_GetChar();
        if(buf[j] == '\n'){
            return;
        }
    }
}

int readfield(const char* src, int start, char *dest){
    for(int j = 0; j<ARGSIZE - 1; j++){
        dest[j] = src[start+j];
        if(dest[j] == ' '){
            dest[j] = '\0';
            return start + j + 1;
        } else if(dest[j] == '\n'){
            dest[j] = '\0';
            return -1;
        }
    }
}

ISR(USART_RX_vect){
    char buf[ARGSIZE*4];
    char arg[ARGSIZE];
    UART_ReadLine(buf);
    int pos = readfield(buf,0,arg);
    uint8_t topic = atoi(arg);
    if(topic & listen_topic){
        Message msg;
        msg.type = topic;
        for(int i = 0; i < MAXARGS && pos != -1; i++){
            pos = readfield(buf,pos,msg.args[i]);
        }
        if(callback != NULL)
            callback(msg);
    }
}

void Message_Init(uint16_t BAUD){
    uint16_t bsd = (F_CPU/16/BAUD) - 1;

    PRR &= ~(1<<PRUSART0);
    UBRR0 = bsd;

    UCSR0B = (1<<TXEN0);
    UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);

    UCSR0B |= (1<<RXCIE0);
    sei();
}

void Message_Send(Message msg, uint8_t argc){
    char buf[4];
    itoa(msg.type,buf,10);
    char *c = &buf[0];
    while((*c) != '\0'){
        UART_PutChar((*c));
        c++;
    }
    for(int i = 0; i < argc; i++){
        if(msg.args[i][0] != '\0'){
            UART_PutChar(' ');
            c = &msg.args[i][0];
            while((*c) != '\0'){
                UART_PutChar((*c));
                c++;
            }
        }
    }
    UART_PutChar('\n');
}  

void Message_Register(MessageTopic topic, MessageCallback cb){
    listen_topic = topic;
    callback = cb;
    UCSR0B |= (1<<RXEN0);
}
