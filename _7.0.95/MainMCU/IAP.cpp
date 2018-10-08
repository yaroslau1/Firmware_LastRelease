/*
    Title:
    Author:   VN Asliuk(Cardian)
    Date:     22.09.2010 12:04:42
    Purpose:
    Software:
    Hardware:
    Note:
*/
//------------------------------------------------------------------------------------
#include "IAP.h"
#include "TLMX9838.h"
#include "TLcd.h"
#include "TLcdTrace.h"
#include "common.h"
#include "main.h"
#include "THardware.h"
//------------------------------------------------------------------------------------
extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;
//------------------------------------------------------------------------------------
#define IAP_DELAY_TICKS 2
volatile int gl = 0;
bool BackUpLoading = false;
int  UsartRcv = 0;
//------------------------------------------------------------------------------------

void IAP_Delay()
{
  gl = 0;
	for(int i = 0;i < IAP_DELAY_TICKS;i++)
	{
//  	for(int j = 0;j < 300000;j++)
  	for(int j = 0;j < 500000;j++)
    {
      gl++;
		  __NOP();
    }
	}
}
//------------------------------------------------------------------------------------

void __set_MSP2(uint32_t mainStackPointer)
{
	asm(" msr msp, r0 \n"
      "	bx lr         ");
}

//------------------------------------------------------------------------------------

bool IAP_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE );
	RCC_AHB1PeriphClockCmd(UB1_BUTTON_CLK, ENABLE );

	GPIO_InitStructure.GPIO_Pin = UB1_BUTTON_PIN;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init( UB1_BUTTON_PORT, &GPIO_InitStructure );
        GPIO_Init( UB2_BUTTON_PORT, &GPIO_InitStructure );

  
	uint16_t  mark = RTC_ReadBackupRegister(RTC_BKP_DR2); 
  
	if(mark == BOOTLOADER_IN_MARKER)
	{
	  /* Allow access to BKP Domain */
	  PWR_BackupAccessCmd(ENABLE);

	  /* Disable the Tamper Pin */
	  RTC_TamperCmd(RTC_Tamper_1, DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
	                                 functionality must be disabled */
		RTC_WriteBackupRegister(RTC_BKP_DR2,BOOTLOADER_OUT_MARKER);
		BackUpLoading = true;
		return true;
	}
  

//	if(GPIO_ReadInputDataBit(UB1_BUTTON_PORT, UB1_BUTTON_PIN) == 0) return true;
//	else return false;
        if(GPIO_ReadInputDataBit(UB1_BUTTON_PORT, UB1_BUTTON_PIN) == 0){
          if(GPIO_ReadInputDataBit(UB2_BUTTON_PORT, UB2_BUTTON_PIN) == 0){
            return false;
          } else
          return true;
        }
        else{
          return false;
        }
        
}
//------------------------------------------------------------------------------

bool IAP_TransportInit()
{
  TLMX9838::BluetoothPHYInit();
  TLcd::LcdPHYInit();
  TLcd::LcdEnable( true );
  TLcd::LCD_Init();
  TLcdTrace::AddLine("    IAP Ver 3.2.0   ");
  TLMX9838::TurnOnBluetooth();
  TLcdTrace::AddLine("                    ");
  TLcdTrace::AddLine("Turn on Bluetooth   ");

  if(!BackUpLoading)
  {
    TLcdTrace::AddLine("Wait host connect   ");
    int h;
    for(h = 0;h < 100;h++)
    {
      IAP_Delay();
      if(GPIO_ReadInputDataBit(LMX_LSTAT_GPIO_PORT,LMX_LSTAT_GPIO_PIN) == 1)
      {
        break;
      }
    }
    if(h >= 99) 
      return 0;
  }
  TLcdTrace::AddLine("Wait host disconnect");

  //Wait for host will connected
  int h;
  for(h = 0;h < 100;h++)
  {
    IAP_Delay();
    if(GPIO_ReadInputDataBit(LMX_LSTAT_GPIO_PORT,LMX_LSTAT_GPIO_PIN) == 0)
    {
      break;
    }
  }
    if(h >= 99)
      return 0;

  TLcdTrace::AddLine("Wait host connect   ");
  TLcdTrace::AddLine("Waiting for 1 min...");

  //Wait for host will connected
  for(int j = 0;j < 100;j++)
  {
    IAP_Delay();
    if(GPIO_ReadInputDataBit(LMX_LSTAT_GPIO_PORT,LMX_LSTAT_GPIO_PIN) == 1)
    {
      while(1)
      {
        if( GPIO_ReadInputDataBit(LMX_LSTAT_GPIO_PORT, LMX_LSTAT_GPIO_PIN ))
          break;
      }
      TLcdTrace::AddLine("Host connected!     ");
      return 1;
    }
    if( GPIO_ReadInputDataBit( UB1_BUTTON_PORT, UB1_BUTTON_PIN ) == 0 ) 
      return 0;
  }
  return 0;
}
//------------------------------------------------------------------------------

void IAP_Start()
{
	if(IAP_Init() == true)
		{
	    /* If Key is pressed */
	    /* Execute the IAP driver in order to re-program the Flash */
	    if(IAP_TransportInit())
	    	{
  				FLASH_Unlock();

				SerialPutString("\r\n======================================================================");
				SerialPutString("\r\n=              (C) COPYRIGHT 2010 ST Microelectronics,CARDIAN        =");
				SerialPutString("\r\n=     In-Application Programming Application  (Version 3.2.0)        =");
				SerialPutString("\r\n=                                   By MCD Application Team          =");
				SerialPutString("\r\n======================================================================");
				SerialPutString("\r\nTo download a binary file via HyperTerminal to the STM32F10x’s");
				SerialPutString("\r\n   internal Flash memory, do as follows:");
				SerialPutString("\r\n1. Press 1 on the keyboard to select the Download Image");
				SerialPutString("\r\n		To the STM32F10x Internal Flash menu");
				SerialPutString("\r\n2. Select Send File in the Transfer menu");
				SerialPutString("\r\n3. In the Filename field, type the name and the path");
				SerialPutString("\r\n   of the binary file you want to download");
				SerialPutString("\r\n4. From the protocol list, select the Ymodem protocol");
				SerialPutString("\r\n5. Click on the Send button");
				SerialPutString("\r\n\r\n");
			    Main_Menu ();
			  }
	TLcdTrace::AddLine("Timeout             ");
	TLcdTrace::AddLine("Reboot your device  ");
		}
  /* Keep the user application running */
  else
  {
    /* Test if user code is programmed starting from address "ApplicationAddress" */
    if ((((*(__IO uint32_t*)ApplicationAddress) & 0xFFFE0000 ) == 0x20000000) || (((*(__IO uint32_t*)ApplicationAddress) & 0xFFFF0000 ) == 0x10000000))
    {
      /* Jump to user application */
      JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);
      Jump_To_Application = (pFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) ApplicationAddress);
      Jump_To_Application();
    }
    else
    {
	TLcd::LcdPHYInit();
	TLcd::LcdEnable( true );
	TLcd::LCD_Init();
	TLcdTrace::AddLine("    IAP Ver 3.2.0   ");
	TLcdTrace::AddLine("                    ");
	TLcdTrace::AddLine("Error in firmware   ");
	TLcdTrace::AddLine("Call for service    ");
	TLcdTrace::AddLine("+375 (17) 363-41-38 ");
    }
  }

  while (1)
  {}
}
//------------------------------------------------------------------------------------
