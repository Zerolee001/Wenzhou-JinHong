
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <compiler_defs.h>
#include <SI_C8051F850_Register_Enums.h>                // SFR declarations
#include "main.h"
#include "LED.h"
#include "Timer.h"
#include "KeyScan.h"
#include "Mode.h"
#include "TCM300.h"
#include "Switch.h"
#include "F85x_FlashUtils.h"

unsigned char u8Address = 0;

#define INFO_ADDRESS          0x1C00

LOCATED_VARIABLE_NO_INIT(flash_write_array[512], U8, SEG_CODE, INFO_ADDRESS);

static void Main_Initial(void);
static void Main_Learn(void);
static void Main_Work(void);

//-----------------------------------------------------------------------------
// TIMER2_ISR
//-----------------------------------------------------------------------------
//
// TIMER2 ISR Content goes here. Remember to clear flag bits:
// TMR2CN::TF2H (Timer # High Byte Overflow Flag)
// TMR2CN::TF2L (Timer # Low Byte Overflow Flag)
//
// 5 millisecond timer2 interrupt
//
//-----------------------------------------------------------------------------
INTERRUPT (TIMER2_ISR, TIMER2_IRQn)
{
	TMR2CN_TF2H = 0;                              // Clear Timer2 interrupt flag
	Timer_5ms_IntHandler();
	KB_Flag_5ms_Inthandle();
}


//-----------------------------------------------------------------------------
// main() Routine
// ----------------------------------------------------------------------------
//
// Note: the software watchdog timer is not disabled by default in this
// example, so a long-running program will reset periodically unless
// the timer is disabled or your program periodically writes to it.
//
// Review the "Watchdog Timer" section under the part family's datasheet
// for details. To find the datasheet, select your part in the
// Simplicity Launcher and click on "Data Sheet".
//
//-----------------------------------------------------------------------------
int main (void)
{
	Main_Initial_IO();

	Timer_Counter_Clear();

	LED_Set_Led(LED_LED1,LED_ON);
	LED_Set_Led(LED_LED2,LED_ON);
	while(Timer_Counter(2000) == 0);
	LED_Set_Led(LED_LED1,LED_OFF);
	LED_Set_Led(LED_LED2,LED_OFF);

	WDTCN = 100;  //Reset time is about 2 seconds
    WDTCN = 0xA5; //Reset WDT

	while (1)
	{
		switch(u8Address)
		{
		    case 0:
		    {
		    	Main_Initial();
		    	break;
		    }
		    case 1:
		    {
		    	Main_Work();
		    	break;
		    }
		    case 2:
		    {
		    	Main_Learn();
		    	break;
		    }
		    default:u8Address = 0;
		}
	}
}

void Main_Initial(void)
{
	unsigned char Blink = 0;
	unsigned char SwitchID[4];

	FLASH_Read(&SwitchID[0], INFO_ADDRESS, 4);
	if(SwitchID[0] != 0xFF || SwitchID[1] != 0xFF || SwitchID[2] != 0xFF || SwitchID[3] != 0xFF)
	{
		u8Address = 1;
		return;
	}
	FLASH_Read(&SwitchID[0], INFO_ADDRESS + 4, 4);
    if(SwitchID[0] != 0xFF || SwitchID[1] != 0xFF || SwitchID[2] != 0xFF || SwitchID[3] != 0xFF)
	{
		u8Address = 1;
		return;
	}

	Timer_Counter_Clear();

	while(1)
	{
		if(Timer_Counter(200))
		{
			Blink = !Blink;
			LED_Set_Led(LED_LED1,Blink);
			LED_Set_Led(LED_LED2,Blink);
			Timer_Counter_Clear();
		}

		if(KB_Get_Code() == KEY_MODE)
		{
			u8Address = 2;
			break;
		}

		WDTCN = 0xA5;                   // Reset the WDT
	}
}

void Main_Learn(void)
{
	unsigned char SwitchID[8];
    unsigned char Blink = 0;
    unsigned char Key;
    unsigned char State = 0;
    unsigned char Message[6];
    unsigned char *pData;
    unsigned char Loop = 0;
    unsigned char Index = 0;

	Timer_Counter_Clear();

	while(1)
	{
		if(Timer_Counter(200))
		{
			Blink = !Blink;
			Timer_Counter_Clear();
		}

		Key = KB_Get_Code();

		if(Key == KEY_MODE)
		{
			u8Address = 0;
			return;
		}
		if(Key == KEY_LEARN)
		{
			if(Index == 0)Index = 1;
			else Index = 0;
			State = 0;
		}

		switch(State)
		{
		    case 0:
		    {
		    	if(Index == 0)
		    	{
		    		LED_Set_Led(LED_LED1,Blink);
		    		LED_Set_Led(LED_LED2,LED_OFF);
		    	}
		    	else
		    	{
		    		LED_Set_Led(LED_LED1,LED_OFF);
		    		LED_Set_Led(LED_LED2,Blink);
		    	}

		    	if(TCM_Check_Message_Flag())
		    	{
		    		pData = TCM_Get_Message_Data(0);
		    		for(Loop = 0; Loop < 6; Loop++)
		    		{
		    			Message[Loop] = pData[Loop];
		    		}
		    		TCM_Clear_Message_Flag();

		    		if(Message[0] == EEP_PROG && Message[1] == 0x10)
		    		{
		    			FLASH_Read(&SwitchID[0], INFO_ADDRESS, 8);
		    			FLASH_PageErase(INFO_ADDRESS);
		    			for(Loop = 0; Loop < 4; Loop++)
		    			{
		    				SwitchID[Index * 4 + Loop] = Message[2+Loop];
		    			}
		    			FLASH_Write(INFO_ADDRESS, &SwitchID[0], 8);

		    			FLASH_Read(&SwitchID[0], INFO_ADDRESS + Index * 4, 4);

		    			if(SwitchID[0] == Message[2] && SwitchID[1] == Message[3] && SwitchID[2] == Message[4] && SwitchID[3] == Message[5])
		    			{
		    				State = 1;
		    			}
		    		}
		    	}
		    	break;
		    }
		    case 1:
		    {   // in this stage, ignore the message --- need to add the process
		    	if(TCM_Check_Message_Flag())
		    	{
		    		TCM_Clear_Message_Flag();
		    	}
		    	if(Index == 0)
		    	{
		    	    LED_Set_Led(LED_LED1,LED_ON);
		    	    LED_Set_Led(LED_LED2,LED_OFF);
		    	}
		    	else
		    	{
		    		LED_Set_Led(LED_LED1,LED_OFF);
		    		LED_Set_Led(LED_LED2,LED_ON);
		    	}
		    	break;
		    }
		}

		WDTCN = 0xA5;                   // Reset the WDT
	}
}

void Main_Work(void)
{
	unsigned char SwitchID[8];
	unsigned char Message[6];
	unsigned char *pData;
	unsigned char Key;
	unsigned char Loop = 0;
	unsigned char Index = 0;

	Timer_Counter_Clear();

	LED_Set_Led(LED_LED1,LED_OFF);
	LED_Set_Led(LED_LED2,LED_OFF);

	while(1)
	{
		Key = KB_Get_Code();
		if(Key == KEY_MODE)
		{
			u8Address = 2;// if in work mode, go to Leran mode
			return;
		}

		FLASH_Read(&SwitchID[0], INFO_ADDRESS, 8);

		if(TCM_Check_Message_Flag())
		{
			pData = TCM_Get_Message_Data(0);
			for(Loop = 0; Loop < 6; Loop++)
			{
				Message[Loop] = pData[Loop];
			}
			TCM_Clear_Message_Flag();

			if(Message[0] == EEP_PROG)
			{
			    if(SwitchID[0] == Message[2] && SwitchID[1] == Message[3] && SwitchID[2] == Message[4] && SwitchID[3] == Message[5])
			    {
			    	//IO set to low(switch ON) when press, and set to high(switch OFF) when release
			    	if(Mode_Check_IO() == SWITCH_PULSE)
			    	{
			    		if(Message[1] == 0x10) //Message[1] is the value, we didn't use state byte to check
			    		{
			    			Switch_Set_Switch(0, 0);
			    			LED_Set_Led(LED_LED1,LED_ON);
			    		}
			    		else
			    		{
			    			Switch_Set_Switch(0, 1);
			    			LED_Set_Led(LED_LED1,LED_OFF);
			    		}
			    	}
			    	//if in toggle mode, press once, toggle once. Press action include Press and release
			    	if(Mode_Check_IO() == SWITCH_TOGGLE)
			    	{
			    		if(Message[1] == 0x10)
			    		{
			    			if(Switch_Get_Switch(0) == 0)
			    			{
			    				Switch_Set_Switch(0, 1);
			    				LED_Set_Led(LED_LED1,LED_OFF);
			    			}
			    			else
			    			{
			    				Switch_Set_Switch(0, 0);
			    				LED_Set_Led(LED_LED1,LED_ON);
			    			}
			    		}
			    	}
			    }
			    if(SwitchID[4] == Message[2] && SwitchID[5] == Message[3] && SwitchID[6] == Message[4] && SwitchID[7] == Message[5])
			    {
			    	//IO set to low(switch ON) when press, and set to high(switch OFF) when release
			    	if(Mode_Check_IO() == SWITCH_PULSE)
			    	{
			    	    if(Message[1] == 0x10) //Message[1] is the value, we didn't use state byte to check
			    	    {
			    			Switch_Set_Switch(1, 0);
			    			LED_Set_Led(LED_LED2,LED_ON);
			    		}
			    		else
			    		{
			    			Switch_Set_Switch(1, 1);
			    			LED_Set_Led(LED_LED2,LED_OFF);
			    		}
			        }
			    	//if in toggle mode, press once, toggle once. Press action include Press and release
			    	if(Mode_Check_IO() == SWITCH_TOGGLE)
			    	{
			    		if(Message[1] == 0x10)
			    		{
			    			if(Switch_Get_Switch(1) == 0)
			    			{
			    				 Switch_Set_Switch(1, 1);
			    				 LED_Set_Led(LED_LED2,LED_OFF);
			    			}
			    			else
			    			{
			    				  Switch_Set_Switch(1, 0);
			    				  LED_Set_Led(LED_LED2,LED_ON);
			    			}
			    	    }
			        }
			    }
			}
		}

		WDTCN = 0xA5;                   // Reset the WDT
	}
}

void Main_Initial_IO(void)
{
	// Disable global interrupts
	IE_EA = 0;

	//Disable Watchdog with key sequence
	WDTCN = 0xDE; //First key
	WDTCN = 0xAD; //Second key

	// CLKSL (Clock Source Select) = HFOSC (Clock derived from the Internal
	//     High-Frequency Oscillator.)
	// CLKDIV (Clock Source Divider) = SYSCLK_DIV_1 (SYSCLK is equal to
	//     selected clock source divided by 1.)
	CLKSEL = CLKSEL_CLKSL__HFOSC | CLKSEL_CLKDIV__SYSCLK_DIV_1;//defalut is DIV2

	// $[P0MDOUT - Port 0 Output Mode]
	/*
	// B0 (Port 0 Bit 0 Output Mode) = OPEN_DRAIN
	//     (P0.0 output is open-drain.)
	// B1 (Port 0 Bit 1 Output Mode) = OPEN_DRAIN
	//     (P0.1 output is open-drain.)
	// B2 (Port 0 Bit 2 Output Mode) = OPEN_DRAIN
	//     (P0.2 output is open-drain.)
	// B3 (Port 0 Bit 3 Output Mode) = OPEN_DRAIN1
	//     (P0.3 output is open-drain.)
	// B4 (Port 0 Bit 4 Output Mode) = PUSH_PULL
	//     (P0.4 output is push-pull.)
	// B5 (Port 0 Bit 5 Output Mode) = OPEN_DRAIN
	//     (P0.5 output is open-drain.)
	// B6 (Port 0 Bit 6 Output Mode) = OPEN_DRAIN
	//     (P0.6 output is open-drain.)
	// B7 (Port 0 Bit 7 Output Mode) = OPEN_DRAIN
	//     (P0.7 output is open-drain.)
	*/
	P0MDOUT = P0MDOUT_B0__OPEN_DRAIN | P0MDOUT_B1__OPEN_DRAIN | P0MDOUT_B2__OPEN_DRAIN
		 | P0MDOUT_B3__OPEN_DRAIN | P0MDOUT_B4__PUSH_PULL | P0MDOUT_B5__OPEN_DRAIN
		 | P0MDOUT_B6__OPEN_DRAIN | P0MDOUT_B7__OPEN_DRAIN;

	// $[P1MDIN - Port 1 Input Mode]
	// [P1MDIN - Port 1 Input Mode]$

	// $[P1 - Port 1 Pin Latch]
	// [P1 - Port 1 Pin Latch]$

	// $[P1MASK - Port 1 Mask]
	// [P1MASK - Port 1 Mask]$

	// $[P1MAT - Port 1 Match]
	// [P1MAT - Port 1 Match]$

	// $[P1MDOUT - Port 1 Output Mode]
	P1MDOUT = P1MDOUT_B0__PUSH_PULL | P1MDOUT_B1__PUSH_PULL | P1MDOUT_B2__OPEN_DRAIN
			 | P1MDOUT_B3__OPEN_DRAIN | P1MDOUT_B4__OPEN_DRAIN | P1MDOUT_B5__PUSH_PULL
			 | P1MDOUT_B6__OPEN_DRAIN | P1MDOUT_B7__OPEN_DRAIN;

	P2MDOUT = P2MDOUT_B1__PUSH_PULL;

	/*
	// URT0E (UART I/O Output Enable) = ENABLED (UART TX, RX routed to Port
	//     pins P0.4 and P0.5.)
	// SPI0E (SPI I/O Enable) = DISABLED (SPI I/O unavailable at Port pins.)
	// SMB0E (SMBus0 I/O Enable) = DISABLED (SMBus0 I/O unavailable at Port
	//     pins.)
	// CP0E (Comparator0 Output Enable) = DISABLED (CP0 unavailable at Port
	//     pin.)
	// CP0AE (Comparator0 Asynchronous Output Enable) = DISABLED
	//     (Asynchronous CP0 unavailable at Port pin.)
	// CP1E (Comparator1 Output Enable) = DISABLED (CP1 unavailable at Port
	//     pin.)
	// CP1AE (Comparator1 Asynchronous Output Enable) = DISABLED
	//     (Asynchronous CP1 unavailable at Port pin.)
	// SYSCKE (SYSCLK Output Enable) = DISABLED (SYSCLK unavailable at Port
	//     pin.)
	*/
	XBR0 = XBR0_URT0E__ENABLED | XBR0_SPI0E__DISABLED | XBR0_SMB0E__DISABLED
		 | XBR0_CP0E__DISABLED | XBR0_CP0AE__DISABLED | XBR0_CP1E__DISABLED
		 | XBR0_CP1AE__DISABLED | XBR0_SYSCKE__DISABLED;

	// $[XBR1 - Port I/O Crossbar 1]
	// [XBR1 - Port I/O Crossbar 1]$

	// $[XBR2 - Port I/O Crossbar 2]
	/*
	// WEAKPUD (Port I/O Weak Pullup Disable) = PULL_UPS_ENABLED (Weak
	//     Pullups enabled (except for Ports whose I/O are configured for analog
	//     mode).)
	// XBARE (Crossbar Enable) = ENABLED (Crossbar enabled.)
	*/
	XBR2 = XBR2_WEAKPUD__PULL_UPS_ENABLED | XBR2_XBARE__ENABLED;
	// [XBR2 - Port I/O Crossbar 2]$

	// $[PRTDRV - Port Drive Strength]
	// [PRTDRV - Port Drive Strength]$

	//Stop Timers
	TCON &= ~(TCON_TR0__BMASK | TCON_TR1__BMASK);

	// $[TH0 - Timer 0 High Byte]
	// [TH0 - Timer 0 High Byte]$

	// $[TL0 - Timer 0 Low Byte]
	// [TL0 - Timer 0 Low Byte]$

	// $[TH1 - Timer 1 High Byte]
	/*
	// TH1 (Timer 1 High Byte) = 150
	*/
	TH1 = (0x2B << TH1_TH1__SHIFT);
	// [TH1 - Timer 1 High Byte]$

	// $[TL1 - Timer 1 Low Byte]
	/*
	// TL1 (Timer 1 Low Byte) = 150
	*/
	TL1 = (0x2B << TL1_TL1__SHIFT);

	// $[CKCON - Clock Control]
	/*
	// SCA (Timer 0/1 Prescale) = SYSCLK_DIV_12 (System clock divided by 12.)
	// T0M (Timer 0 Clock Select) = PRESCALE (Counter/Timer 0 uses the clock
	//     defined by the prescale field, SCA.)
	// T2MH (Timer 2 High Byte Clock Select) = EXTERNAL_CLOCK (Timer 2 high
	//     byte uses the clock defined by the T2XCLK bit in TMR2CN.)
	// T2ML (Timer 2 Low Byte Clock Select) = EXTERNAL_CLOCK (Timer 2 low
	//     byte uses the clock defined by the T2XCLK bit in TMR2CN.)
	// T3MH (Timer 3 High Byte Clock Select) = EXTERNAL_CLOCK (Timer 3 high
	//     byte uses the clock defined by the T3XCLK bit in TMR3CN.)
	// T3ML (Timer 3 Low Byte Clock Select) = EXTERNAL_CLOCK (Timer 3 low
	//     byte uses the clock defined by the T3XCLK bit in TMR3CN.)
	// T1M (Timer 1 Clock Select) = SYSCLK (Timer 1 uses the system clock.)
	*/
	CKCON = CKCON_T0M__PRESCALE | CKCON_T1M__SYSCLK | CKCON_T2MH__EXTERNAL_CLOCK
		 | CKCON_T2ML__EXTERNAL_CLOCK | CKCON_T3MH__EXTERNAL_CLOCK | CKCON_T3ML__EXTERNAL_CLOCK;

	// $[TMOD - Timer 0/1 Mode]
	/*
	// T0M (Timer 0 Mode Select) = MODE0 (Mode 0, 13-bit Counter/Timer)
	// CT0 (Counter/Timer 0 Select) = TIMER (Timer Mode. Timer 0 increments
	//     on the clock defined by T0M in the CKCON register.)
	// GATE0 (Timer 0 Gate Control) = DISABLED (Timer 0 enabled when TR0 = 1
	//     irrespective of /INT0 logic level.)
	// T1M (Timer 1 Mode Select) = MODE2 (Mode 2, 8-bit Counter/Timer with
	//     Auto-Reload)
	// CT1 (Counter/Timer 1 Select) = TIMER (Timer Mode. Timer 1 increments
	//     on the clock defined by T1M in the CKCON register.)
	// GATE1 (Timer 1 Gate Control) = DISABLED (Timer 1 enabled when TR1 = 1
	//     irrespective of /INT1 logic level.)
	*/
	TMOD = TMOD_T0M__MODE0 | TMOD_CT0__TIMER | TMOD_GATE0__DISABLED
		 | TMOD_T1M__MODE2 | TMOD_CT1__TIMER | TMOD_GATE1__DISABLED;

	// $[TCON - Timer 0/1 Control]
	/*
	// TR1 (Timer 1 Run Control) = RUN (Start Timer 1 running.)
	*/
	TCON |= TCON_TR1__RUN;

	//Timer2 interrupt Triggered when timer overflowed
	//Stop Timer
	TMR2CN &= ~(TMR2CN_TR2__BMASK);

	// $[TMR2CN - Timer 2 Control]
	// [TMR2CN - Timer 2 Control]$

	// $[TMR2H - Timer 2 High Byte]
	/*
	// TMR2H (Timer 2 High Byte) = 255
	*/
	TMR2H = (255 << TMR2H_TMR2H__SHIFT);
	// [TMR2H - Timer 2 High Byte]$

	// $[TMR2L - Timer 2 Low Byte]
	/*
	// TMR2L (Timer 2 Low Byte) = 255
	*/
	TMR2L = (255 << TMR2L_TMR2L__SHIFT);
	// [TMR2L - Timer 2 Low Byte]$

	// $[TMR2RLH - Timer 2 Reload High Byte]
	/*
	// TMR2RLH (Timer 2 Reload High Byte) = 156
	*/
	TMR2RLH = (216 << TMR2RLH_TMR2RLH__SHIFT);
	// [TMR2RLH - Timer 2 Reload High Byte]$

	// $[TMR2RLL - Timer 2 Reload Low Byte]
	/*
	// TMR2RLL (Timer 2 Reload Low Byte) = 79
	*/
	TMR2RLL = (32 << TMR2RLL_TMR2RLL__SHIFT);
	// [TMR2RLL - Timer 2 Reload Low Byte]$

	// $[TMR2CN]
	/*
	// TR2 (Timer 2 Run Control) = RUN (Start Timer
	//     2 running.)
	*/
	TMR2CN |= TMR2CN_TR2__RUN;
	// [TMR2CN]$

	// $[SCON0 - UART0 Serial Port Control]
	/*
	// REN (Receive Enable) = RECEIVE_ENABLED (UART0 reception
	//     enabled.)
	*/
	SCON0 |= SCON0_REN__RECEIVE_ENABLED;
	SCON0_RI = 0;
	SCON0_TI = 0;

	// $[EIE1 - Extended Interrupt Enable 1]
	// [EIE1 - Extended Interrupt Enable 1]$

	// $[EIP1 - Extended Interrupt Priority 1]
	// [EIP1 - Extended Interrupt Priority 1]$

	// $[IE - Interrupt Enable]
	/*
	// EA (All Interrupts Enable) = DISABLED (Disable all interrupt sources.)
	// EX0 (External Interrupt 0 Enable) = DISABLED (Disable external
	//     interrupt 0.)
	// EX1 (External Interrupt 1 Enable) = DISABLED (Disable external
	//     interrupt 1.)
	// ESPI0 (SPI0 Interrupt Enable) = DISABLED (Disable all SPI0
	//     interrupts.)
	// ET0 (Timer 0 Interrupt Enable) = DISABLED (Disable all Timer 0
	//     interrupt.)
	// ET1 (Timer 1 Interrupt Enable) = DISABLED (Disable all Timer 1
	//     interrupt.)
	// ET2 (Timer 2 Interrupt Enable) = ENABLED (Enable interrupt requests
	//     generated by the TF2L or TF2H flags.)
	// ES0 (UART0 Interrupt Enable) = ENABLED (Enable UART0 interrupt.)
	*/
	IE = IE_EA__DISABLED | IE_EX0__DISABLED | IE_EX1__DISABLED | IE_ESPI0__DISABLED
			| IE_ET0__DISABLED | IE_ET1__DISABLED | IE_ET2__ENABLED | IE_ES0__ENABLED;
	// [IE - Interrupt Enable]$

	// $[IP - Interrupt Priority]
	/*
	// PX0 (External Interrupt 0 Priority Control) = LOW (External Interrupt
	//     0 set to low priority level.)
	// PX1 (External Interrupt 1 Priority Control) = LOW (External Interrupt
	//     1 set to low priority level.)
	// PSPI0 (Serial Peripheral Interface (SPI0) Interrupt Priority Control)
	//     = LOW (SPI0 interrupt set to low priority level.)
	// PT0 (Timer 0 Interrupt Priority Control) = LOW (Timer 0 interrupt set
	//     to low priority level.)
	// PT1 (Timer 1 Interrupt Priority Control) = LOW (Timer 1 interrupt set
	//     to low priority level.)
	// PT2 (Timer 2 Interrupt Priority Control) = LOW (Timer 2 interrupt set
	//     to low priority level.)
	// PS0 (UART0 Interrupt Priority Control) = HIGH (UART0 interrupt set to
	//     high priority level.)
	*/
	IP = IP_PX0__LOW | IP_PX1__LOW | IP_PSPI0__LOW | IP_PT0__LOW | IP_PT1__LOW
		 | IP_PT2__LOW | IP_PS0__HIGH;

	// [VDM0CN - Supply Monitor Control]$
	VDM0CN |= VDM0CN_VDMEN__ENABLED;

	//Reset when the voltage is not stable
	RSTSRC = 0x02;

	// Ensable global interrupts
	IE_EA = 1;
}
