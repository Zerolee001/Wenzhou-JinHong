#include "ioC8051F850.h"
#include "KeyScan.h"

unsigned char w_KB_Time = 0;
unsigned char Time_Flag = 0;

#define C_TRUE_TIME 40 //5ms interrupt, 40*5 = 200ms
#define C_HOLD_TIME 200//5ms interrupt, 200*5 = 1000ms
#define C_MAX_TIME  250 //

#define KEY1  P0_bit.P00

unsigned char KB_Encoder(void)
{
	unsigned char by_Dat = KEY_NONE;

	if(KEY1 == SW_PRESSED)by_Dat = KEY_LEARN;

	return by_Dat;
}

void KB_Flag_5ms_Inthandle(void)
{
	Time_Flag = 1;
}

unsigned char KB_Get_Code(void)
{
    unsigned char by_Dat = KEY_NONE;
    unsigned char by_Out = KEY_NONE;

    if(Time_Flag == 0)return 0;
    Time_Flag = 0;

    by_Dat = KB_Encoder();
    //*******************************************************************
    if(by_Dat == KEY_NONE)
    {
        w_KB_Time = 0;
        return 0;
    }
    //*******************************************************************
    if(w_KB_Time < C_MAX_TIME)
    {
        ++w_KB_Time;
    }
    //*******************************************************************
    if(w_KB_Time == C_TRUE_TIME)
    {
    	by_Out = by_Dat;
    }

    if(w_KB_Time == C_HOLD_TIME)
    {
    	if(by_Dat == KEY_LEARN)
    	{
    		by_Out =  by_Dat | 0x80;
    	}
    }
    return by_Out;
}
