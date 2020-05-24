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
ISR(TIMER0_COMPA_vect){
	disk_timerproc();	/* Drive timer procedure of low level disk I/O module */
}

// ------------- PUBLIC -------------

// Initializes the SD-card
void SD_Init(void){
    DDRD &= ~(1<<PIN5);
	PORTD |= (1<<PIN5);
	DDRD |= (1<<PIN2);
    SD_DDR |= (1<<SD_CS)|(1<<SD_POWER_PIN);

    // Start 100Hz timer
    OCR0A = F_CPU / 1024 / 100 - 1;
	TCCR0A = (1<<WGM01);
	TCCR0B = (1<<CS02) | (1<<CS00);
	TIMSK0 = _BV(OCIE0A);
}

void SD_Start(void){
    SD_PORT &= ~(1<<SD_POWER_PIN);
    sd_connected = (f_mount(&sd_fs, "", 1) == FR_OK);
}

void SD_Shutdown(void){
    f_unmount("");
    sd_connected = false;

    SD_PORT |= (1<<SD_POWER_PIN);
}

void SD_Check(void){
    if(PIND & (1<<PIN5)){
        if(!SD_IsConnected()){
            SD_Start();
        }
    } else if(SD_IsConnected()){
        SD_Shutdown();
    }

    if(SD_IsConnected()){
        PORTD |= (1<<PIN2);
    } else {
        PORTD &= ~(1<<PIN2);
    }
}

// Writes a formated string to a connected SD-card
// Note that the final string should not be over
// 128 characters long including null terminator
void SD_Write(uint32_t time, uint8_t type, char *msg){
    FRESULT fr;
    FIL logfile;
    // Attempt to open file

    if (sd_connected){
        
        fr = f_open(&logfile, "bil.log", FA_WRITE | FA_OPEN_ALWAYS);
        //sd_connected = (fr == FR_OK);
        if (fr == FR_OK) {
            // Seek to end of the file to append data
            fr = f_lseek(&logfile, f_size(&logfile));
            //sd_connected = (fr == FR_OK);
            if (fr == FR_OK){
                char buf[64];
                sprintf(buf,"%lu %u %s\n",time,type,msg);
                
                f_puts(buf,&logfile);    // Write constructed string to file
            }
        }
        f_close(&logfile);  // Close log file and flush data buffer
    }
}

bool SD_IsConnected(void){
    return sd_connected;
}