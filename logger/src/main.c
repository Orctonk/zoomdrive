#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>

void UART_Init(uint16_t baud){
    uint16_t bsd = (F_CPU/16/baud) - 1;

    PRR &= ~(1<<PRUSART0);
    UBRR0 = bsd;

    UCSR0B = (1<<TXEN0)| (1<<RXEN0);
    UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);
}

static int UART_PutChar(char c, FILE * stream){
    if (c == '\a')
    {
      fputs("*ring*\n", stderr);
      return 0;
    }

  if (c == '\n')
    UART_PutChar('\r', stream);
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;

  return 0;
}

static char UART_GetChar(FILE * stream){
    uint8_t c;
  char *cp, *cp2;
  static char b[256];
  static char *rxp;

  if (rxp == 0)
    for (cp = b;;)
      {
	while(!(UCSR0A & (1<<RXC0)));

	c = UDR0;
	/* behaviour similar to Unix stty ICRNL */
	if (c == '\r')
	  c = '\n';
	if (c == '\n')
	  {
	    *cp = c;
	    UART_PutChar(c, stream);
	    rxp = b;
	    break;
	  }
	else if (c == '\t')
	  c = ' ';

	if ((c >= (uint8_t)' ' && c <= (uint8_t)'\x7e') ||
	    c >= (uint8_t)'\xa0')
	  {
	    if (cp == b + 256 - 1)
	      UART_PutChar('\a', stream);
	    else
	      {
		*cp++ = c;
		UART_PutChar(c, stream);
	      }
	    continue;
	  }

	switch (c)
	  {
	  case 'c' & 0x1f:
	    return -1;

	  case '\b':
	  case '\x7f':
	    if (cp > b)
	      {
		UART_PutChar('\b', stream);
		UART_PutChar(' ', stream);
		UART_PutChar('\b', stream);
		cp--;
	      }
	    break;

	  case 'r' & 0x1f:
	    UART_PutChar('\r', stream);
	    for (cp2 = b; cp2 < cp; cp2++)
	      UART_PutChar(*cp2, stream);
	    break;

	  case 'u' & 0x1f:
	    while (cp > b)
	      {
		UART_PutChar('\b', stream);
		UART_PutChar(' ', stream);
		UART_PutChar('\b', stream);
		cp--;
	      }
	    break;

	  case 'w' & 0x1f:
	    while (cp > b && cp[-1] != ' ')
	      {
		UART_PutChar('\b', stream);
		UART_PutChar(' ', stream);
		UART_PutChar('\b', stream);
		cp--;
	      }
	    break;
	  }
      }

  c = *rxp++;
  if (c == '\n')
    rxp = 0;

  return c;
}

static FILE mystdout = FDEV_SETUP_STREAM(UART_PutChar,UART_GetChar,_FDEV_SETUP_RW);

int main(void) {
    UART_Init(4800);
    stdout = stdin = &mystdout;

    char inp[128];

    while(1){
        scanf("%s",inp);
        printf("AVR recieved %s\n",inp);
    }
}