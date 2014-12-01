//-----------------------------------------------------------------------------
// F85x_FlashPrimitives.c
//-----------------------------------------------------------------------------
// Copyright 2014 Silicon Laboratories, Inc.
// http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
//
// Program Description:
//
// This program contains several useful utilities for writing and updating
// flash memory.
//
// Target:         C8051F85x/86x
// Tool chain:     Keil C51 8.00
// Command Line:   None
//
//
// Release 1.0
//    -Initial Revision (TP)
//    -19 FEB 2013
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "ioC8051F850.h"
#include "F85x_FlashPrimitives.h"

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------

void          FLASH_ByteWrite (FLADDR addr, char byte);
U8 FLASH_ByteRead  (FLADDR addr);
void          FLASH_PageErase (FLADDR addr);

//-----------------------------------------------------------------------------
// FLASH_ByteWrite
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) FLADDR addr - address of the byte to write to
//                    valid range is from 0x0000 to 0x1FFF for 8 kB devices
//                    valid range is from 0x0000 to 0x0FFF for 4 kB devices
//                    valid range is from 0x0000 to 0x07FF for 2 kB devices
//   2) char byte - byte to write to flash.
//
// This routine writes <byte> to the linear flash address <addr>.
//
//-----------------------------------------------------------------------------
void FLASH_ByteWrite (FLADDR addr, char byte)
{
   char EA_SAVE = IE_bit.EA;//IE_EA;                   // Preserve IE_EA
   //char xdata * data pwrite;           // Flash write pointer
   char __xdata * pwrite;           // Flash write pointer

   //IE_EA = 0;                             // Disable interrupts
   IE_bit.EA = 0;

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source

   //pwrite = (char xdata *) addr;
   pwrite = (char __xdata *) addr;

   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x01;                      // PSWE = 1 which enables writes

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source
   *pwrite = byte;                     // Write the byte

   PSCTL &= ~0x01;                     // PSWE = 0 which disable writes

   //IE_EA = EA_SAVE;                       // Restore interrupts
   IE_bit.EA = EA_SAVE;
}

//-----------------------------------------------------------------------------
// FLASH_ByteRead
//-----------------------------------------------------------------------------
//
// Return Value :
//      U8 - byte read from flash
// Parameters   :
//   1) FLADDR addr - address of the byte to read to
//                    valid range is from 0x0000 to 0x1FFF for 8 kB devices
//                    valid range is from 0x0000 to 0x0FFF for 4 kB devices
//                    valid range is from 0x0000 to 0x07FF for 2 kB devices
//
// This routine reads a <byte> from the linear flash address <addr>.
//
//-----------------------------------------------------------------------------
U8 FLASH_ByteRead (FLADDR addr)
{
   char EA_SAVE =IE_bit.EA;//IE_EA;                   // Preserve IE_EA
   //char code * data pread;             // Flash read pointer
   char __code * pread;
   
   U8 byte;

   //IE_EA = 0;                             // Disable interrupts
   IE_bit.EA = 0;

   //pread = (char code *) addr;
   pread = (char __code *) addr;

   byte = *pread;                      // Read the byte

   //IE_EA = EA_SAVE;                       // Restore interrupts
   IE_bit.EA = EA_SAVE;
   
   return byte;
}

//-----------------------------------------------------------------------------
// FLASH_PageErase
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) FLADDR addr - address of any byte in the page to erase
//                    valid range is from 0x0000 to 0x1FFF for 8 kB devices
//                    valid range is from 0x0000 to 0x0FFF for 4 kB devices
//                    valid range is from 0x0000 to 0x07FF for 2 kB devices
//
// This routine erases the flash page containing the linear flash address
// <addr>.  Note that the page of flash page containing the Lock Byte cannot be
// erased if the Lock Byte is set.
//
//-----------------------------------------------------------------------------
void FLASH_PageErase (FLADDR addr)
{
   char EA_SAVE = IE_bit.EA;//IE_EA;                   // Preserve IE_EA
   //char xdata * data pwrite;           // Flash write pointer
   char __xdata * pwrite;

   //IE_EA = 0;                             // Disable interrupts
   IE_bit.EA = 0;

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source

   //pwrite = (char xdata *) addr;
   pwrite = (char __xdata *) addr;

   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x03;                      // PSWE = 1; PSEE = 1

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source
   *pwrite = 0;                        // Initiate page erase

   PSCTL &= ~0x03;                     // PSWE = 0; PSEE = 0

   //IE_EA = EA_SAVE;                       // Restore interrupts
   IE_bit.EA = EA_SAVE;
}

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------