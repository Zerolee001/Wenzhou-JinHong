/*
 * Switch.c
 *
 *  Created on: 2014-7-11
 *      Author: Lee
 */

#include <compiler_defs.h>
#include <SI_C8051F850_Register_Enums.h>                // SFR declarations

SBIT(SWITCH1, SFR_P1, 5);                   // P1.5 SWITCH1
SBIT(SWITCH2, SFR_P2, 1);                   // P2.1 SWITCH2

void Switch_Set_Switch(unsigned char Index, unsigned char Mode)
{
	if(Index == 0)SWITCH1 = Mode;
	if(Index == 1)SWITCH2 = Mode;
}

unsigned char Switch_Get_Switch(unsigned char Index)
{
	if(Index == 0)return (SWITCH1);
	if(Index == 1)return (SWITCH2);

	return 0;
}
