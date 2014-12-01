#include "ioC8051F850.h"

#define SWITCH1  P1_bit.P15
#define SWITCH2  P2_bit.P21


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
