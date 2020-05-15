#include "message.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static volatile MessageTopic listen_topic;
static volatile MessageCallback callback;
static volatile bool skip;

static char UART_GetChar(){
    while ( !(UCSR0A & (1<<RXC0)) ) {}
	/* Get and return received data from buffer */
	return UDR0;
}

static int UART_PutChar(char c){
	while(!(UCSR0A & (1<<UDRE0))){}
	UDR0 = c;

	return 0;
}

bool readfield(char *buf){
    for(int j = 0; j<ARGSIZE - 1; j++){
        buf[j] = UART_GetChar();
        if(buf[j] == ' '){
            buf[j] = '\0';
            return true;
        } else if (buf[j] == '\n'){
            buf[j] = '\0';
            return false;
        }
    }
}

ISR(USART_RX_vect){
    char buf[ARGSIZE];
    bool done = !readfield(buf);
    uint8_t topic = atoi(buf);
    if(!(topic & listen_topic)){
        while(!done && UART_GetChar() != '\n');
    }
    else {
        Message msg;
        msg.type = topic;
        done = false;
        for(int i = 0; i < MAXARGS && !done; i++){
            done = !readfield(buf);
            strcpy(msg.args[i],buf);
        }
        if(callback != NULL)
            callback(msg);
    }
}

void Message_Init(uint16_t BAUD){
    uint16_t bsd = (F_CPU/16/BAUD) - 1;

    PRR &= ~(1<<PRUSART0);
    UBRR0 = bsd;

    UCSR0B = (1<<TXEN0)| (1<<RXEN0);
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
}
