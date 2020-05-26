#ifndef __MESSAGE_H__
#define __MESSAGE_H__

// Defines topic bitfield for messages
typedef enum MessageTopic{
    TOCAR       = 0b00100000,
    FROMCAR     = 0b01000000,
    INFO        = 0b10000000,
} MessageTopic;

// Defines concrete message types for messages, each belonging to a topic
typedef enum MessageType{
    ENGINE_POWER    = (TOCAR)       |   0,
    HEADING         = (TOCAR)       |   1,
    EMBUTTON        = (TOCAR)       |   2,
    DEADMAN         = (TOCAR)       |   3,
    HONK            = (TOCAR)       |   4,
    PING            = (TOCAR)       |   5,
    HEARTBEAT       = (TOCAR)       |   6,
    CSTSTRING       = (TOCAR)       |   7,
    GEAR            = (TOCAR)       |   8,
    UPDATE_STATE    = (FROMCAR)     |   9,
    PONG            = (FROMCAR)     |   10,
    CARBUTTON       = (FROMCAR)     |   11,
    COLLISION       = (FROMCAR)     |   12,
    UPDATE_SENSORS  = (FROMCAR)     |   13,
    UPDATE_EM       = (FROMCAR)     |   14
} MessageType;

#ifndef MESSAGE_ONLY_ENUM
#include <stdint.h>

// Define max message size
#define MAXARGS 3   // Message argument count
#define ARGSIZE 32  // Message argument max length

// Struct representing a message
typedef struct Message {
    MessageType type;               // The type of the message
    char args[MAXARGS][ARGSIZE];    // The arguments of the message
} Message;

// Function signature for a message received callback
typedef void (*MessageCallback)(Message);

// Initializes the message module setting the BAUD rate of the serial
// communication to @BAUD
void Message_Init(uint16_t BAUD);

// Sends message stored in @msg, assumes that @msg contains @argc arguments 
void Message_Send(Message msg, uint8_t argc);

// Polls the receive buffer for new incoming messages and calls callbacks
// as appropriate
void Message_Update(void);

// Overrides the current message callback with @cb or removes 
// callbacks if @cb is NULL
// Only triggers callbacks with messages whose topic-bit is set in @topic
void Message_Register(MessageTopic topic, MessageCallback cb);
#endif

#endif