/*
 * TCM300.h
 *
 *  Created on: 2014-5-7
 *      Author: zero
 */

#ifndef TCM300_H_
#define TCM300_H_

#define   BUFFER_LENGTH          14

#define   GET_SYNC_STATE         1
#define   GET_HEADER_STATE       2
#define   CHECK_CRC8H_STATE      3
#define   GET_DATA_STATE         4
#define   CHECK_CRC8D_STATE      5


#define   SER_SYNCH_CODE         0x55
#define   SER_HEADER_NR_BYTES    0x04

#define   ADD_DATA_CHOICE            0x00
#define   ADD_DATA_DATA              0x01
#define   ADD_DATA_ID                0x02
#define   ADD_DATA_STATE             0x06

#define   EEP_PROG        0xF6

unsigned char TCM_Check_Message_Flag(void);
void TCM_Clear_Message_Flag(void);
unsigned char* TCM_Get_Message_Data(unsigned char Index);
unsigned char TCM_Get_State(void);

#endif /* TCM300_H_ */
