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
#include "main.h"
#include "TSPISRAM.h"
#include "ae_ext_cm.h"
#include "flash_if.h"
//------------------------------------------------------------------------------------
/*extern*/ pFunction Jump_To_Application;
/*extern*/ uint32_t JumpAddress;

CharLong codebuff[1024];

//------------------------------------------------------------------------------------
bool IAP_Init()
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  uint16_t  mark = RTC_ReadBackupRegister(RTC_BKP_DR2); 
  
  if(mark == BOOTLOADER_IN_MARKER)
  {
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);
    RTC_WriteBackupRegister(RTC_BKP_DR2,BOOTLOADER_OUT_MARKER);

    return true;
  }
  else 
  {
    return false;
  }
}
//------------------------------------------------------------------------------------


void IAP_Start()
{
  if (IAP_Init())
  { 
    CharLong codesize;
    uint32_t sramaddr = 0;
    uint32_t flashaddr = 0x08006000;
    uint32_t buffsize;

    //Init SPI SRAM
    TSPISRAM::PHYInit();

    TSPISRAM::ReadSeq(sramaddr, 4, &codesize.Char[0]);
    sramaddr += 4;

    FLASH_If_Init();
    
    while (codesize.Long != 0)
    {
      if (codesize.Long > 1024 ) buffsize = 1024;
      else buffsize = codesize.Long;
      TSPISRAM::ReadSeq(sramaddr, buffsize, &codebuff[0].Char[0]);
      codesize.Long -= buffsize;
      sramaddr += buffsize;
      FLASH_ErasePage(flashaddr);
      if (FLASH_If_Write(&flashaddr, (uint32_t *) &codebuff[0].Long, (uint16_t) buffsize/4)  != 0)
      {
        return;
      }
    }
    NVIC_SystemReset();
  }
  else
  { 
    uint32_t *read_crc = (uint32_t*)0x0800FFFC;
    uint32_t* begin = (uint32_t *)0x08006000;      // получаем начало прошивки
    uint32_t* end = (uint32_t *)0x0800FFFC;        // получаем конец прошивки
    if(chekCRC(begin, end, read_crc[0])) 
    { 
      /* Jump to user application */
      JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
      Jump_To_Application = (pFunction) JumpAddress;
      
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
      
      /* Jump to application */
      Jump_To_Application();
    }
  }

  while (1)
  {}
}
//------------------------------------------------------------------------------------
