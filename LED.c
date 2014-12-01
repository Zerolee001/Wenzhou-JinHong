#include "ioC8051F850.h"
#include "LED.h"

#define LED1 P1_bit.P10

void LED_Set_Led(unsigned char Index, unsigned char Data)
{
	if(Index == LED_LED1)LED1 = Data;
}

