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
    EMSTATE         = (FROMCAR)     |   6,
    SPEED           = (FROMCAR)     |   7,
    DISTANCE        = (FROMCAR)     |   8,
    PONG            = (FROMCAR)     |   9,
    CARBUTTON       = (FROMCAR)     |   10,
    HEARTBEAT       = (INFO)        |   11
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
void Message_Send(Message msg);
void Message_Register(MessageTopic topic,MessageCallback cb);
#endif

#endif