#ifndef TIMER_H_
#define TIMER_H_

#define T_LOOP 0x80


void Timer_Initial_Data(void);
void Timer_Set_Flag_100ms_Int(void);
void Timer_Counter_Clear(unsigned char by_Index);
unsigned char Timer_Counter(unsigned char by_Mode,unsigned short w_Dat);

#endif /* TIMER_H_ */
