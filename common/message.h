#ifndef __MESSAGE_H__
#define __MESSAGE_H__

typedef enum MessageTopic{
    TOCAR       = 0b00100000,
    FROMCAR     = 0b01000000,
    INFO        = 0b10000000,
} MessageTopic;

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
    EMSTATE         = (FROMCAR)     |   9,
    SPEED           = (FROMCAR)     |   10,
    DISTANCE        = (FROMCAR)     |   11,
    PONG            = (FROMCAR)     |   12,
    CARBUTTON       = (FROMCAR)     |   13
    
} MessageType;

#ifndef MESSAGE_ONLY_ENUM
#include <stdint.h>

#define MAXARGS 3
#define ARGSIZE 32

typedef struct Message {
    MessageType type;
    char args[MAXARGS][ARGSIZE];
} Message;

typedef void (*MessageCallback)(Message);

void Message_Init(uint16_t BAUD);
void Message_Send(Message msg, uint8_t argc);
void Message_Register(MessageTopic topic,MessageCallback cb);
#endif

#endif