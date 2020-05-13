#pragma once

uint8_t SPI_TransmitRecieve(uint8_t data);

void writeData(uint8_t data);

void writeString(char* string);

void clearDisplay(void);

void moveCursor(uint8_t data);

void writeTemp(int, int, int, int, int);

void lcdInit(void);