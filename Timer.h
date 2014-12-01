/*
 * Timer.h
 *
 *  Created on: 2014-5-15
 *      Author: zero
 */

#ifndef TIMER_H_
#define TIMER_H_

void Timer_Counter_Clear(void);
unsigned char Timer_Counter(unsigned short Time);
void Timer_5ms_IntHandler(void);

#endif /* TIMER_H_ */
