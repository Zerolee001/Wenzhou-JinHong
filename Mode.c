/*
 * Mode.c
 *
 *  Created on: 2014-7-9
 *      Author: Lee
 */

#include <compiler_defs.h>
#include <SI_C8051F850_Register_Enums.h>                // SFR declarations
#include "Mode.h"

SBIT(MODE_IO, SFR_P0, 1);                   // P0.1 KEY1

unsigned char Mode_Check_IO(void)
{
	return MODE_IO;
}
