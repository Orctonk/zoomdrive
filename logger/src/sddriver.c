/*  FILE:           sddriver.c
 *  DESCRIPTION: 
 *      Implementation of the SD-card driver using the FAT filesystem
 *      as well as SPI disk I/O.
 *  AUTHOR:         Filip Henningsson, c18fhn@cs.umu.se
 */

#include <avr/interrupt.h>
#include <stdio.h>

#include "sddriver.h"
#include "FatFs/ff.h"
#include "FatFs/diskio.h"

// ------------- PRIVATE -------------

// SD-card state
static FATFS sd_fs;
static bool sd_connected;

// 100Hz clock to trigger disk I/O internal timer
ISR(TIMER1_COMPA_vect){
	disk_timerproc();	/* Drive timer procedure of low level disk I/O module */
    //PORTD ^= (1<<PIN2);
}

// ------------- PUBLIC -------------

// Initializes the SD-card
void SD_Init(void){
    SD_DDR |= (1<<SD_CS);
    SD_PORT |= (1<<SD_CS);

    // Start 100Hz timer
    OCR1A = F_CPU / 1024 / 100 - 1;
	TCCR1A = 0;
	TCCR1B = (1<<CS12) | (1<<CS10) | (1<<WGM12);
	TIMSK1 = _BV(OCIE1A);

    sei();

    sd_connected = (f_mount(&sd_fs, "", 1) == FR_OK);
}

// Writes a formated string to a connected SD-card
// Note that the final string should not be over
// 128 characters long including null terminator
void SD_Write(char *fmt, ...){
    FRESULT fr;
    FIL logfile;
    // Attempt to open file
    if (sd_connected){
        PORTD |= (1<<PIN2);
        fr = f_open(&logfile, "bil.log", FA_WRITE | FA_OPEN_ALWAYS);
        if (fr == FR_OK) {
            // Seek to end of the file to append data
            fr = f_lseek(&logfile, f_size(&logfile));
            if (fr == FR_OK){
                // Create string from format string and varargs
                char buffer[128];
                va_list args;
                va_start(args,fmt);
                vsnprintf(buffer,128,fmt,args);
                f_puts(buffer,&logfile);    // Write constructed string to file
            }
        }
        f_close(&logfile);  // Close log file and flush data buffer
    }
}