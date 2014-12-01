/*
 * LED.c
 *
 *  Created on: 2014-5-7
 *      Author: zero
 */

#include <compiler_defs.h>
#include <SI_C8051F850_Register_Enums.h>                // SFR declarations
#include "LED.h"

SBIT(LED1, SFR_P1, 0);                   // P1.0 LED1
SBIT(LED2, SFR_P1, 1);                   // P1.1 LED2

void LED_Set_Led(unsigned char Index, unsigned char Data)
{
	if(Index == LED_LED1)LED1 = Data;
	if(Index == LED_LED2)LED2 = Data;
}

