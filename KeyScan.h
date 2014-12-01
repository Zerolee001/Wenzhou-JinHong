/*
 * KeyScan.h
 *
 *  Created on: 2014-5-7
 *      Author: zero
 */

#ifndef KEYSCAN_H_
#define KEYSCAN_H_

#define SW_PRESSED           0
#define SW_NOT_PRESSED       1

#define KEY_NONE          0
#define KEY_LEARN         1
#define KEY_CLEAR         2

#define KEY_MODE          (KEY_LEARN | 0x80)

unsigned char KB_Encoder(void);
void KB_Flag_5ms_Inthandle(void);
unsigned char KB_Get_Code(void);

#endif /* KEYSCAN_H_ */
