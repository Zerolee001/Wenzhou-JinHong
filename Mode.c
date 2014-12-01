#include "ioC8051F850.h"
#include "Mode.h"

#define MODE_IO  P0_bit.P01

unsigned char Mode_Check_IO(void)
{
	return MODE_IO;
}
