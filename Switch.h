/*
 * Switch.h
 *
 *  Created on: 2014-7-11
 *      Author: Lee
 */

#ifndef SWITCH_H_
#define SWITCH_H_

#define SWITCH_SWITCH1        0
#define SWITCH_SWITCH2        1

void Switch_Set_Switch(unsigned char Index, unsigned char Mode);
unsigned char Switch_Get_Switch(unsigned char Index);

#endif /* SWITCH_H_ */
