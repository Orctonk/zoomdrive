/*
 * LCD.h
 * 
 * Date: 2020-05-11
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */


#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>

#define LCD_DDR DDRB
#define LCD_REGISTER_SELECT_PIN PIN3
#define LCD_PORT PORTB
#define LCD_CHIP_SELECT_PIN PIN2

//#include <stdio.h>

/*
 *Initializes the display.
 */
void LCD_init();

/*
 *Takes a given string and write it to the display at the current selected position.
 *
 *string: The string to be written to the display.
 */
void LCD_write_string(const char* string);

/*
 *Takes a given address for the cursor and sets the cursor to that address.
 *
 * cursor_adress: Where the cursor are to be set.
 */
void LCD_set_cursor(uint8_t cursor_adress);

/*
 *Clears the whole display.
 */
void LCD_clear_display();

// /*
//  * static put char function to link printf to LCD.
//  */
// static int LCD_put_char(char c, FILE* stream){
// 	char o[2] = {c, '\0'}; 
// 	LCD_write_string(o);
// 	return 0;
// }


#endif 