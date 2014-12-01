/*
 * LED.h
 *
 *  Created on: 2014-5-7
 *      Author: zero
 */

#ifndef IOCONTROL_H_
#define IOCONTROL_H_

#define LED_ON               0
#define LED_OFF              1

#define LED_LED1             1
#define LED_LED2             2



void LED_Set_Led(unsigned char Index, unsigned char Data);

#endif /* IOCONTROL_H_ */
