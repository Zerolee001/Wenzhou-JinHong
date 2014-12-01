/*
 * Timer.c
 *
 *  Created on: 2014-5-15
 *      Author: zero
 */

#include <compiler_defs.h>
#include <SI_C8051F850_Register_Enums.h>                // SFR declarations
#include "Timer.h"

unsigned short u8TimeFlag = 0;

void Timer_Counter_Clear(void)
{
	IE_EA = 0;
	u8TimeFlag = 0;
	IE_EA = 1;
}

unsigned char Timer_Counter(unsigned short Time)
{
	if(u8TimeFlag >= Time) return 1;
	return 0;
}

//Max delay time = 1250ms or 1.2S
void Timer_5ms_IntHandler(void)
{
	if(u8TimeFlag < 60000)u8TimeFlag = u8TimeFlag + 5;
	else u8TimeFlag = 60000;
}
