#pragma once



#define F_CPU 1000000UL
#define BAUD 4800UL
#define UBBR_VAL ((F_CPU / (16UL * BAUD)) - 1)


#define BIT_SET(REG, BIT) (REG & (1 << BIT))

