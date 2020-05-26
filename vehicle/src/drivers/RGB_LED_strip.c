/*
 * RGB_LED_strip.c
 * 
 * Date: 2020-05-20
 * 
 * Author: Mimmi Edholm 
 * 
 * 
 */

#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "RGB_LED_strip.h"
#include "summer.h"

#define LED_COUNT 20

typedef struct rgb_color{
  uint8_t red, green, blue;
} rgb_color;

rgb_color colors[LED_COUNT];

void __attribute__((noinline)) led_strip_write(rgb_color * colors, uint16_t count);

/*
 * Initialize 
 */
void LED_strip_init(void){
    STRIP_PORT &= ~(1<<STRIP_PIN);
    STRIP_DDR |= (1<<STRIP_PIN);

    for(int i = 0; i < LED_COUNT; i++){
        colors[i] = (rgb_color){0,0,0};
    }

    led_strip_write(colors,2);
}

/*
 * Turn on or off the right turn signal.
 * 
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */
void turn_signal_right(bool lamp_switch){
    if(lamp_switch){
        colors[0] = (rgb_color){255,191,0};
        colors[1] = (rgb_color){255,191,0};

        colors[6] = (rgb_color){255,191,0};
        colors[7] = (rgb_color){255,191,0};
    } else{
        colors[0] = (rgb_color){0,0,0};
        colors[1] = (rgb_color){0,0,0};

        colors[6] = (rgb_color){0,0,0};
        colors[7] = (rgb_color){0,0,0};
    }
    led_strip_write(colors,8);
}

/*
 * Turn on or off the left turn signal.
 * 
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */     
void turn_signal_left(bool lamp_switch){
    if(lamp_switch){
        colors[2] = (rgb_color){255,126,0};
        colors[3] = (rgb_color){255,126,0};

        colors[4] = (rgb_color){255,191,0};
        colors[5] = (rgb_color){255,191,0};
    } else{
        colors[2] = (rgb_color){0,0,0};
        colors[3] = (rgb_color){0,0,0};

        colors[4] = (rgb_color){0,0,0};
        colors[5] = (rgb_color){0,0,0};
    }
    led_strip_write(colors,8);
}

/*
 * Will turn on the back lights.
 *  
 * lamp_switch: If 1, turn lamp on. If 0, turn lamp off.
 */
void backing_lights(bool lamp_switch){
if(lamp_switch){
        colors[6] = (rgb_color){255,191,0};
        colors[7] = (rgb_color){255,191,0};

        colors[4] = (rgb_color){255,191,0};
        colors[5] = (rgb_color){255,191,0};
    } else{
        colors[6] = (rgb_color){0,0,0};
        colors[7] = (rgb_color){0,0,0};

        colors[4] = (rgb_color){0,0,0};
        colors[5] = (rgb_color){0,0,0};
    }
    led_strip_write(colors,8);
}

/*
 * Will make the LED strip blue.
 */
void LED_blue(void){
    for(int i = 8; i < LED_COUNT; i++){
        colors[i] = (rgb_color){0, 0, 255};
    }
    led_strip_write(colors, LED_COUNT);
     
}

/*
 * Will make the LED strip yellow.
 */
void LED_yellow(void){
    for(int i = 8; i < LED_COUNT; i++){
        colors[i] = (rgb_color){255,191,0};
    }
    led_strip_write(colors, LED_COUNT);
     
}

/*
 * Will make the LED strip red.
 */
void LED_red(void){
    for(int i = 8; i < LED_COUNT; i++){
        colors[i] = (rgb_color){ 255, 0, 0 };
    }
    led_strip_write(colors, LED_COUNT);
     
}
/*
 * Will make the LED strip green.
 */
void LED_green(void){
    for(int i = 8; i < LED_COUNT; i++){
        colors[i] = (rgb_color){0, 255, 0 };
    }
    led_strip_write(colors, LED_COUNT);
     
}

/*
 * Will give the LED strip a colour based on the parameter. 
 */
void LED_lights(int i){
    switch (i)
    {
    case MELODY_HEMGLASS:
        LED_blue();
        break;
    case MELODY_HERR_GURKA:
        LED_green();
        break;
    case MELODY_TWINKLE:
        LED_yellow();
        break; 
    case MELODY_USSR:
        LED_red();
    default:
        break;
    }
}
/************************************/
 /*Following drivers got from Pololu*/

void __attribute__((noinline)) led_strip_write(rgb_color * colors, uint16_t count)
{
  // Set the pin to be an output driving low.
    STRIP_PORT &= ~(1<<STRIP_PIN);
    STRIP_DDR |= (1<<STRIP_PIN);

  cli();   // Disable interrupts temporarily because we don't want our pulse timing to be messed up.
  while (count--)
  {
    // Send a color to the LED strip.
    // The assembly below also increments the 'colors' pointer,
    // it will be pointing to the next color at the end of this loop.
    asm volatile (
        "ld __tmp_reg__, %a0+\n"
        "ld __tmp_reg__, %a0\n"
        "rcall send_led_strip_byte%=\n"  // Send red component.
        "ld __tmp_reg__, -%a0\n"
        "rcall send_led_strip_byte%=\n"  // Send green component.
        "ld __tmp_reg__, %a0+\n"
        "ld __tmp_reg__, %a0+\n"
        "ld __tmp_reg__, %a0+\n"
        "rcall send_led_strip_byte%=\n"  // Send blue component.
        "rjmp led_strip_asm_end%=\n"     // Jump past the assembly subroutines.

        // send_led_strip_byte subroutine:  Sends a byte to the LED strip.
        "send_led_strip_byte%=:\n"
        "rcall send_led_strip_bit%=\n"  // Send most-significant bit (bit 7).
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"  // Send least-significant bit (bit 0).
        "ret\n"

        // send_led_strip_bit subroutine:  Sends single bit to the LED strip by driving the data line
        // high for some time.  The amount of time the line is high depends on whether the bit is 0 or 1,
        // but this function always takes the same time (2 us).
        "send_led_strip_bit%=:\n"
#if F_CPU == 8000000
        "rol __tmp_reg__\n"                      // Rotate left through carry.
#endif
        "sbi %2, %3\n"                           // Drive the line high.

#if F_CPU != 8000000
        "rol __tmp_reg__\n"                      // Rotate left through carry.
#endif

#if F_CPU == 16000000
        "nop\n" "nop\n"
#elif F_CPU == 20000000
        "nop\n" "nop\n" "nop\n" "nop\n"
#elif F_CPU != 8000000
#error "Unsupported F_CPU"
#endif

        "brcs .+2\n" "cbi %2, %3\n"              // If the bit to send is 0, drive the line low now.

#if F_CPU == 8000000
        "nop\n" "nop\n"
#elif F_CPU == 16000000
        "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
#elif F_CPU == 20000000
        "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
        "nop\n" "nop\n"
#endif

        "brcc .+2\n" "cbi %2, %3\n"              // If the bit to send is 1, drive the line low now.

        "ret\n"
        "led_strip_asm_end%=: "
        : "=b" (colors)
        : "0" (colors),         // %a0 points to the next color to display
          "I" (_SFR_IO_ADDR(STRIP_PORT)),   // %2 is the port register (e.g. PORTC)
          "I" (STRIP_PIN)     // %3 is the pin number (0-8)
    );

    // Uncomment the line below to temporarily enable interrupts between each color.
    //sei(); asm volatile("nop\n"); cli();
  }
  sei();          // Re-enable interrupts now that we are done.
  _delay_us(80);  // Send the reset signal.
}
