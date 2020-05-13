/*
 * LCD.c
 * 
 * Date: 2020-05-12
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */


#include "LCD.h"
#include "SPI.h"

#include <util/delay.h>


#define LED_PORT PORTC
#define LED_PIN PIN0 
#define LED_DDR DDRC

/*
 * Transmits data to the LCD.
 *
 * temp: Data to be printed.
 */
void LCD_transmit(uint8_t temp){
	LCD_PORT &= ~(1<< LCD_CHIP_SELECT_PIN);
	SPI_transmit_recieve(temp);
	LCD_PORT |= (1<< LCD_CHIP_SELECT_PIN);
}

/*
 *Initializes the display.
 */
void LCD_init(){
    DDRC |= (1<< LCD_REGISTER_SELECT_PIN);
	LCD_DDR |= (1<< LCD_CHIP_SELECT_PIN);
	_delay_ms(45);
	PORTC &= ~(1<< LCD_REGISTER_SELECT_PIN);
	//Function set
	LCD_transmit(0x39); 
	_delay_us(30);
	//Bias set
	LCD_transmit(0x1D);
	_delay_us(30);
	//Power control 
	LCD_transmit(0x50);
	_delay_us(30);
	//Follower control
	LCD_transmit(0x6C);
	_delay_us(30);
	//Contrast set
	LCD_transmit(0x78);
	_delay_us(30);
	//Function set
	LCD_transmit(0x38);
	_delay_us(30);
	//Display ON/OFF
	LCD_transmit(0x0F);
	_delay_us(30);
	//Clear display
	LCD_transmit(0x01);
	_delay_ms(2);
	//Entry mode set
	LCD_transmit(0x06);
	_delay_us(30);	
}

/*
 *Takes a given string and write it to the display at the current selected position.
 *
 *string: The string to be written to the display.
 */
void LCD_write_string(const char* string){
	PORTC |= (1<< LCD_REGISTER_SELECT_PIN);
	for(int i = 0; string[i] != '\0'; i++){
		char c = string[i]; 
		LCD_transmit(c);
	}
	
	PORTC &= ~(1<< LCD_REGISTER_SELECT_PIN);
}

/*
 *Takes a given address for the cursor and sets the cursor to that address.
 *
 * cursor_adress: Where the cursor are to be set.
 */
void LCD_set_cursor(uint8_t cursor_adress) {
	cursor_adress |= (1<<7);
	LCD_transmit(cursor_adress);
	_delay_us(30);
}

/*
 *Clears the whole display.
 */
void LCD_clear_display(){
	LCD_transmit(0x01);
	_delay_ms(2);
}

