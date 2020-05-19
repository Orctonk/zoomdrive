#include "message.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// ------------- PRIVATE -------------

static volatile MessageTopic listen_topic;  // The topic(s) which trigger callbacks
static volatile MessageCallback callback;   // The registered callback or NULL if none
static volatile char receive_buffer[256];   // Circular UART receive buffer
static volatile uint16_t readPos = 0;       // Current receive buffer read pos
static volatile uint16_t writePos = 0;      // Current receive buffer write pos

// Increments @x and loops around to 0 if the new value is outside of buffer
#define INCREMENT_LOOP(x) (x = ((x + 1) % 256))

// UART receive byte interrupt
ISR(USART_RX_vect){
    // Store received value into buffer
    receive_buffer[writePos] = UDR0;
    // Update write pos
    INCREMENT_LOOP(writePos);
}

// Sends a single byte over UART
static void UART_PutChar(char c){
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
}

// Attempts to read a message from the recieve buffer into @msg
// Returns true if read is successful.
// If read fails due to read catching up to last read UART byte
// the reading is cancelled and the readPos is reset
// If read fails due to message corruption readmsg forwards the readpos
// to the next newline
bool readmsg(Message *msg){
    // Save readPos in case it needs to be reverted
    uint16_t savedReadPos = readPos;

    // Read topic
    char buf[4];
    bool goodread = false;  // Topic is read
    bool done = false;      // Newline is read
    for(int i = 0; i < 4; i++){
        // If read catches up
        if(readPos == writePos){
            // Reset readPos
            readPos = savedReadPos;
            return false;
        }

        // Read single char and increment
        buf[i] = receive_buffer[INCREMENT_LOOP(readPos)];

        if(buf[i] == ' '){  // Topic is read and there's more in message
            buf[i] = '\0';
            goodread = true;
            break;
        } else if(buf[i] == '\n'){  // Topic is read and there's nothing left
            buf[i] = '\0';
            goodread = true;
            done = true;
            break;
        }
    }
    // Forward readPos if topic failed to be read
    if(!goodread){
        while(receive_buffer[INCREMENT_LOOP(readPos)] != '\n');
        return false;
    }

    // Convert topic to int
    msg->type = atoi(buf);
    
    // Read args until message end
    for(int i = 0;i < MAXARGS && !done; i++){
        for(int j = 0; j<ARGSIZE - 1; j++){
            // If read catches up
            if(readPos == writePos){
                // Reset readPos
                readPos = savedReadPos;
                return false;
            }

            // Read receive buffer into args
            msg->args[i][j] = receive_buffer[INCREMENT_LOOP(readPos)];
            if(msg->args[i][j] == ' '){
                msg->args[i][j] = '\0';
                break;
            } else if(msg->args[i][j] == '\n'){
                msg->args[i][j] = '\0';
                done = true;
                break;
            }
        }
    }

    // Message contains too many args, forward readpos
    if(!done){
        while(receive_buffer[INCREMENT_LOOP(readPos)] != '\n');
        return false;
    }

    // Read succeeded
    return true;
}

// ------------- PUBLIC -------------

// Initializes the message module setting the BAUD rate of the serial
// communication to @BAUD
void Message_Init(uint16_t BAUD){
    uint16_t bsd = (F_CPU/16/BAUD) - 1;
    UBRR0 = bsd;

    UCSR0B = (1<<TXEN0) | (1<<RXEN0);
    UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);

    UCSR0B |= (1<<RXCIE0);
}

// Sends message stored in @msg, assumes that @msg contains @argc arguments 
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

// Polls the receive buffer for new incoming messages and calls callbacks
// as appropriate
void Message_Update(void){
    Message msg;
    while(readmsg(&msg))
        if(msg.type & listen_topic)
            callback(msg);
}

// Overrides the current message callback with @cb or removes 
// callbacks if @cb is NULL
// Only triggers callbacks with messages whose topic-bit is set in @topic
void Message_Register(MessageTopic topic, MessageCallback cb){
    listen_topic = topic;
    callback = cb;
}
