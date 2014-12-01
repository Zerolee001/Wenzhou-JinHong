#include "ioC8051F850.h"
#include "Timer.h"

#define T_CHANNEL_MAX 8

static unsigned short w_Timer_Flag;
static unsigned short aw_Timer_Data[T_CHANNEL_MAX];

void Timer_Initial_Data(void)
{
    unsigned char by_Loop;
    w_Timer_Flag=0;
    for(by_Loop=0;by_Loop < T_CHANNEL_MAX;by_Loop++)
        aw_Timer_Data[by_Loop]=0xffff;
}

void Timer_Set_Flag_100ms_Int(void)
{
    w_Timer_Flag = 0xffff;
}

void Timer_Counter_Clear(unsigned char by_Index)
{
    aw_Timer_Data[by_Index]=0;
}

unsigned char Timer_Counter(unsigned char by_Mode,unsigned short w_Dat)
{
    unsigned char b_Out = 0;
    unsigned char by_Channel;
    unsigned short w_Mask;
    
    by_Channel = by_Mode & 0x7f;
    
    w_Mask = 0x01 << by_Channel;
    if((w_Timer_Flag & w_Mask)==0) return(0);
    w_Timer_Flag &= ~w_Mask;//if marked
    
    if(aw_Timer_Data[by_Channel] <= w_Dat) ++aw_Timer_Data[by_Channel];
    if(aw_Timer_Data[by_Channel] == w_Dat) b_Out=1;
    if(aw_Timer_Data[by_Channel] >= w_Dat)
    {
        if(by_Mode & 0x80) aw_Timer_Data[by_Channel]=0;//for auto clear "T_LOOP"
    }
    return(b_Out);
}
