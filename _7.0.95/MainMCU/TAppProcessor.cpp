/************************************************************************************/
/* Title:    Main application command and task processor              */
/* Author:    Cyber Genius                            */
/* Version:    0.1                                  */
/* Date:    10.07.2012                              */
/************************************************************************************/
/*                                          */
/************************************************************************************/
#include "TAppProcessor.h"
#include "TSDProcessor.h"
#include "TAudio.h"
#include "TDevice.h"
#include "TFlash.h"
#include "TString.h"
#include "TLcdTrace.h"
//#include "dosfs.h"
#include "TPoolingNand.h"
#include "TGuiObjectsV2.h"
//#include "TADS1292.h"
#include "TInterpreter.h"
#include "TRtc.h"
#include "EcgMcu.h"
#include "TSpiSram.h"
#include "TSound.h"

//#define MSG_ERROR "Произошла ошибка.\nПерезапустите\nрегистратор.\nПри повторении\nобратитесь в\nсервисный центр."

/*----------------------------------------------------------------------------------*/
extern BYTE CardType;

extern void SystemInit();

extern TBitMap const* SysFont;

xTaskHandle TAppProcessor::xKeyProcessor;
xTaskHandle TAppProcessor::xScreenHandler;
xTaskHandle TAppProcessor::xLEDblink;

int DTstamp = 0;

volatile int HFH_ev = 0;

bool SD_On_Sound = true;

bool SD_Off_Sound = true;

bool buttonState = false;

bool TAppProcessor::StartNow = false;

bool TAppProcessor::ECGRecordingStarted = false;

/*----------------------------------------------------------------------------------*/
stufSerialNumber TAppProcessor::ufSerialNumber =
{
  "Cardian KR01-SNXXXX-XXXX",
  2000,
  732872,
  "XXXX",
  "XXXX"
};

uint8_t  TAppProcessor::BatVoltageLevel = 0;
uint16_t TAppProcessor::BatVoltageValue = 0;
uint16_t TAppProcessor::LiVoltageValue = 0;
uint16_t TAppProcessor::AnalogVoltageValue = 0;
uint16_t TAppProcessor::SysVoltageValue = 0;

stPatientCardV1 TAppProcessor::PatientCard =
{
  CARD_VERSION,
  DEVICE_VERSION,
  "none",
  "01.01.2001",
  0,
  "/",
  "none",
  "none",
  "none",
  0,
  0,
  1,
  0x00010001,
  0
};

stUFramDeviceMap TAppProcessor::UFrameReqDeviceMap;
TTimeStampEV TAppProcessor::DeviceDateTime =
{
  0,
  1
};

stSetup TAppProcessor::Setup = {0,0,0,0,0,0,0,0};

volatile int mutx = 0;
//xSemaphoreHandle  f_mutex;

FATFS FATFS_Obj;
FIL TrvFile;
FIL PatCardFile;

int sndTakeN = 0;

BYTE led_mode = 0;

bool TAppProcessor::StartRecording = false;
bool TAppProcessor::AudioRec = false;

int TAppProcessor::AX_X_Coord = 0;
int TAppProcessor::AX_Y_Coord = 0;
int TAppProcessor::AX_Z_Coord = 0;

int TAppProcessor::cable_state = ctNoCable;
uint8_t TAppProcessor::ecg_format = 7;

volatile bool TAppProcessor::audio_rec_start = false;



volatile uint8_t TAppProcessor::AX_CoordWr[3] = {0,0,0};

BYTE karta[1024];
const char inst1[]="KP-01      V 3.0";  //Версия 2.0(10 разрядов), 3.0(11 разрядов)
const char inst2_3[]="00000003";
const char inst3_3[]="Отведений-3,разрядов-12";
const char inst4[]="KP-01";
//const char inst5[]="ЭКГ по 2 груд.отвед."; //20
const char inst5_3[]="ЭКГ по 3 ортог.отвед";
//const char Fio[]="KR02 Test                      ";
//const char Year[]="01.01.1977       ";
const char vozr[]="53               ";
//const char Data[]="16.05.2011       ";
//const char Time[]="15:59:03         ";
const char dlit_v[]="04:09:26         ";
const char exDate[]="01.01.2014       ";
const char exTime[]="09:00:00         ";

bool TAppProcessor::StartECGRecording = false;
bool TAppProcessor::RecordInProgress = false;

bool TAppProcessor::ExitFromRecordMode = true;
bool TAppProcessor::ErrorInRecordMode = true;

const BYTE _ff_array[64] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
                            0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

void TAppProcessor::RCC_Configuration_Main(void)
{   
//  /* Setup the microcontroller system. Initialize the Embedded Flash Interface,  
//     initialize the PLL and update the SystemFrequency variable. */
//  SystemInit();

  RCC_ClearFlag();

  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable Prefetch Buffer */
  FLASH_PrefetchBufferCmd(ENABLE);

  /* Flash 2 wait state */
  FLASH_SetLatency(FLASH_Latency_0);
  
  /* HCLK = SYSCLK */
  RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
  /* PCLK2 = HCLK */
  RCC_PCLK2Config(RCC_HCLK_Div1); 

  /* PCLK1 = HCLK */
  RCC_PCLK1Config(RCC_HCLK_Div1);

  /* PLLCLK = 16MHz / 16 * 192 / 8 = 24 MHz */
  RCC_PLLConfig(RCC_PLLSource_HSI, 16, 192, 8, 4);

  /* Enable PLL */ 
  RCC_PLLCmd(ENABLE);

  /* Wait till PLL is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {
  }

  /* Select PLL as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

  /* Wait till PLL is used as system clock source */
  while(RCC_GetSYSCLKSource() != 0x08)
  {
  }

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);

  /* Configure HCLK clock as SysTick clock source. */
  SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );
}

/*----------------------------------------------------------------------------------*/
/* @brief  Обработка команд блютуза                        */
/* @param  None                                  */
/* @return  None                                  */
/*----------------------------------------------------------------------------------*/


void TAppProcessor::App_ParceEvents( uint8_t pckt, uint8_t _datalen, uint8_t *_gdata )
{
  uint8_t c_s;
        c_s = (uint8_t)cable_state;
  HFH_ev = 0;
  
  switch( pckt )
  {
    case CMD_NULL_COMMAND:
//      SendFrameToHost( RPL_NULL );
      SendFrameToHost( RPL_NULL, 1, c_s );
      break;

    case CMD_UNIFIED_FRAME:
      HFH_ev = 1;
      UnifiedFrameRcvHandler();
      break;

    case CMD_RESET:
#ifdef LCD_TRACE
      TLcdTrace::AddLine("RESET COMMAND");
#endif
      SendFrameToHost(RPL_RESET);
      vTaskDelay(2000/portTICK_RATE_MS); 
      TDevice::SystemReset();
      break;
    case CMD_GO_BLD:
#ifdef LCD_TRACE
      TLcdTrace::AddLine("GOTO BOOTLOADER");
      TLcdTrace::AddLine("Reseting...");
#endif
      SendFrameToHost(RPL_BLD);
      RTC_WriteBackupRegister(RTC_BKP_DR2,BOOTLOADER_IN_MARKER);
      vTaskDelay(1000/portTICK_RATE_MS); 
      TDevice::SystemReset();
      break;

    default:
      SendFrameToHost( RPL_UNDEFINED_CMD );
      break;
  }
}

/*----------------------------------------------------------------------------------*/
/* @brief  None                                  */
/* @param  None                                  */
/* @return  None                                  */
/* @info    Format of UFrame                            */
/*       0 - Frame type - always 1                        */
/*      1 - Sets Hi byte                            */
/*      2 - Sets Lo byte                            */
/*      3 - Req Hi byte                             */
/*      4 - Req Lo byte                              */
/*      -- blocks where 0 - block lenght in bytes 1... block array        */
/*----------------------------------------------------------------------------------*/
void TAppProcessor::UnifiedFrameRcvHandler( void )
{
  /*************************/
  /* Confirmation of setup */
  SendFrameToHost(RPL_UNIFIED_CONFIRM);

  stUFrameHostSetsMap UFrameHostSetsMap;
  BYTE gDataIndx = 0;
  CharInt uMap;

  uMap.Char[0] = gData[HOST_SETS_MAP_HIBYTE];
  uMap.Char[1] = gData[HOST_SETS_MAP_LOBYTE];
  UFrameHostSetsMap.u._word.value = uMap.Int;

  uMap.Char[0] = gData[DIVICE_REQUEST_MAP_HIBYTE];
  uMap.Char[1] = gData[DIVICE_REQUEST_MAP_LOBYTE];
  UFrameReqDeviceMap.u._word.value = uMap.Int;

  gDataIndx = UFRAME_BLOCKS_BEGIN;

  HFH_ev = 3;


/* Setting patient action */
  if(UFrameHostSetsMap.u._bits.fmSetPatientAction)  /* 0002 */
  {
    __no_operation();  /* Empty */
  }

  /* Setting maximum record lenght */
  if(UFrameHostSetsMap.u._bits.fmSetRecordLenght)    /* 0004 */
  {
    __no_operation();  /* Empty */
  }

  /* Setting travel file */
  if(UFrameHostSetsMap.u._bits.fmSetTravelFile)    /* 0008 */
  {
//    TSDProcessor::ReplaceECGFile = true;
    TSDProcessor::NoAskReplaceECGFileQuestion = true;  /* Да переписать и не спрашивать больше */
  }

  HFH_ev = 4;
//  if(!Setup.u._bits.UseWriteToCard && !Setup.u._bits.UseMonitoring)
  {
    /* Setting serial number of device */
    if(UFrameHostSetsMap.u._bits.fmSerialNumber)  /* 0200 */
    {
      HFH_ev = 5;
      gDataIndx = ReadBlockIntoBuffer( (uint8_t *)&ufSerialNumber, gDataIndx );
      
      TFlash::SF_UpdatePage();
      TFlash::SF_StoreDataArray( SERIAL_NUM, (uint8_t *)&ufSerialNumber, sizeof(stufSerialNumber));
      TFlash::SF_CommitPage();      
      
      PatientCard.Token.Bits._DeviceSerialID = ufSerialNumber.Serial;
    }

    /* Setting device Date/Time */
    if(UFrameHostSetsMap.u._bits.fmDateTime)    
    {
      HFH_ev = 6;
      gDataIndx = ReadBlockIntoBuffer( (uint8_t *)&DeviceDateTime, gDataIndx );
      
      //send date/time to f0 controller
    
      BYTE EnRtc[4];
      
      /*
      int DTstamp = ((ts.Date - ts2010.Date) * 3600 * 24) + ((ts.Time - ts2010.Time)/1000);
        if(DTstamp < 0) DTstamp = 0;
      */
      
      TInterpreter::IntDateTime = DeviceDateTime;
      
      DTstamp = ((DeviceDateTime.Date - 733773) * 3600 * 24) + ((DeviceDateTime.Time)/1000);
        //if(DTstamp < 0) DTstamp = 0;
      
      EnRtc[3] = ((DTstamp & 0xFF000000) >> 24);
      EnRtc[2] = ((DTstamp & 0x00FF0000) >> 16);
      EnRtc[1] = ((DTstamp & 0x0000FF00) >> 8);
      EnRtc[0] = (DTstamp & 0x000000FF);
            
      TInterpreter::SendFrameToHost(CMD_SET_RTC,4,EnRtc);
    }

    /* Setting patient card (without RecordLenght,DateTimeOfStart,Token,StartPosition) */
    if(UFrameHostSetsMap.u._bits.fmPatientCard)    /* 0400 */
    {
      HFH_ev = 7;
      gDataIndx = ReadBlockIntoBuffer( (uint8_t *)&PatientCard, gDataIndx );    /* Принимает карту пациента по блютузу */
    }

    /* Setting BT configurations */
    if(UFrameHostSetsMap.u._bits.fmBTConfiguration)  /* 1000 */
    {
      HFH_ev = 8;
      gData[gDataIndx++];    /* Skip data count 7 - default */

      for(BYTE i = 0;i < 6;i++)
      {
        TLMX9838::BDAddress[i] = gData[gDataIndx++];
      }

      TLMX9838::RemoteBTPort = gData[gDataIndx++];      /* BD Port */
    }

    /* Setting notes */
    if(UFrameHostSetsMap.u._bits.fmNotes)      /* 2000 */
    {
      __no_operation();  /* Empty */
    }

    /* Setting ADC Calibrations */
    if(UFrameHostSetsMap.u._bits.fmCalibrations)  /* 4000 */
    {
      __no_operation();  /* Empty */
    }

    /* Testing device - direct signal settings */
    if(UFrameHostSetsMap.u._bits.fmTestingSignals) /* 8000 */
    {
      __no_operation();  /* Empty */
    }

    /* Clear log */
    if(UFrameHostSetsMap.u._bits.fmClearLog)     /* 0001 */
    {
      __no_operation();  /* Empty */
    }

    /* Voice */
    if(UFrameHostSetsMap.u._bits.fmVoice)     /* 0010 */
    {
      HFH_ev = 9;
      TAudio::VoiceRec = true;
    }
    else
    {
      TAudio::VoiceRec = false;
    }

    /* Axelerometer */
    if(UFrameHostSetsMap.u._bits.fmAxel)     /* 0020 */
    {
      __no_operation();  /* Empty */
    }

    /* Respiration */
    if(UFrameHostSetsMap.u._bits.fmResp)     /* 0040 */
    {
        TInterpreter::SendFrameToHost(CMD_RESP_ON);
    }
    else
    {
        TInterpreter::SendFrameToHost(CMD_RESP_OFF);
    }

    if(UFrameHostSetsMap.u._bits.fmStartNow)     /* 0080 */
    {
        TAppProcessor::StartNow = true;
    }



  }

  /* Setting device mode */
  if( UFrameHostSetsMap.u._bits.fmSetup )        /* 0100 */
  {
    HFH_ev = 10;
    gDataIndx++;  /* Skip size byte */

    Setup.u._word.value = gData[gDataIndx++];

    /* Setup Handler */
    if( Setup.u._bits.EP_Enable )
    {
    HFH_ev = 11;
      if( Setup.u._bits.UseMonitoring )
      {
    HFH_ev = 12;
        StartMonitioring = true;
        if (TFrame::StartMonitioring /*|| TSDProcessor::SDStartWrPool*/ || TGuiObjects::EcgOnScreen) TInterpreter::SetStartUSARTMonitioring(true);
        else TInterpreter::SetStartUSARTMonitioring(false);

    HFH_ev = 30;
//        vTaskResume( xhECGDataSend );
    HFH_ev = 31;
//        TInterpreter::SendFrameToHost(CMD_START_MONITORING);

      }
      else
      {
    HFH_ev = 13;
        StartMonitioring = false;
        if (TFrame::StartMonitioring /*|| TSDProcessor::SDStartWrPool*/ || TGuiObjects::EcgOnScreen) TInterpreter::SetStartUSARTMonitioring(true);
        else TInterpreter::SetStartUSARTMonitioring(false);

//        vTaskSuspend( xhECGDataSend );
      
//        TInterpreter::SendFrameToHost(CMD_STOP_MONITORING);
      }

      if( Setup.u._bits.UseWriteToCard )        /* Write to Card enable */
      {
    HFH_ev = 14;
        if(TSDProcessor::SDPresent) 
        {
    HFH_ev = 15;
          StartECGRecording = true;
          TFrame::TurnOffAfterLinkDisconnected = true;
        }
        
//        if( TSDProcessor::StartWriting() == 0 )    /* начало записи 0 - ошибка */
//        {
//          StartMonitioring = false;
//          vTaskSuspend( xhECGDataSend );
//          TSDProcessor::StopWriting();
//        }
      }
      else
      {
    HFH_ev = 16;
        StartECGRecording = false;
        TFrame::TurnOffAfterLinkDisconnected = false;
//        TDevice::SystemReset();

//        TSDProcessor::StopWriting();
      }
    }
    else
    {
    HFH_ev = 17;
      StartMonitioring = false;
      if (TFrame::StartMonitioring /*|| TSDProcessor::SDStartWrPool*/ || TGuiObjects::EcgOnScreen) TInterpreter::SetStartUSARTMonitioring(true);
      else TInterpreter::SetStartUSARTMonitioring(false);

      StartECGRecording = false;
      TFrame::TurnOffAfterLinkDisconnected = false;
//      vTaskSuspend( xhECGDataSend );
//      TSDProcessor::StopWriting();
    }
  }

  
  /****************************/
  /* Request flags processing */
  /****************************/
  gDataIndx = 2;

  if(!Setup.u._bits.UseWriteToCard && !Setup.u._bits.UseMonitoring)
  {
    HFH_ev = 18;
    /* Request Device Log */
    if(UFrameReqDeviceMap.u._bits.fmrLog)  /* 0002 */
    {
      __no_operation();  /* Empty */
    }
    
    /* Request Test Signals */
    if(UFrameReqDeviceMap.u._bits.fmrTestingSignals)  /* 8000 */
    {
      __no_operation();  /* Empty */
    }
    
    /* Request device serial number */
    if( UFrameReqDeviceMap.u._bits.fmrSerialNumber )  /* 0200 */
    {
    HFH_ev = 19;
      gData[gDataIndx++] = sizeof(ufSerialNumber);
      gDataIndx = UploadBlockIntoBuffer( (uint8_t *)&ufSerialNumber, gDataIndx, sizeof(ufSerialNumber) );
    }
    
    /* Request BT configurations */
    if(UFrameReqDeviceMap.u._bits.fmrBTConfiguration)  /* 1000 */
    {
    HFH_ev = 20;
      gData[gDataIndx++] = 6+1;  /* +1; */
      /* BD Address return */
      gDataIndx = UploadBlockIntoBuffer( BDAddress, gDataIndx, 6 );
      /* BD Port */
      gData[gDataIndx++] = RemoteBTPort;
    }
    
    /* Request Device Notes */
    if(UFrameReqDeviceMap.u._bits.fmrNotes)        /* 2000 */
    {
      __no_operation();  /* Empty */
    }
    
    /* Request ADC Calibrations */
    if(UFrameReqDeviceMap.u._bits.fmrCalibrations)    /* 4000 */
    {
      __no_operation();  /* Empty */
    }
  }

  /* Request device mode */
  if(UFrameReqDeviceMap.u._bits.fmrSetup)      /* 0100 */
  {
    HFH_ev = 21;
    gData[gDataIndx++] = 1;
    gData[gDataIndx++] = Setup.u._word.value;
  }
  
  /* Request Patient Card */
  if(UFrameReqDeviceMap.u._bits.fmrPatientCard)  /* 0400 */
  {
    HFH_ev = 22;
    gData[gDataIndx++] = sizeof(PatientCard);
    gDataIndx = UploadBlockIntoBuffer( (uint8_t *)&PatientCard, gDataIndx, sizeof(PatientCard) );
  }
  
  /* Request Device Date/Time */
  if(UFrameReqDeviceMap.u._bits.fmrDateTime)    /* 0800 */
  {
    HFH_ev = 23;
    gData[gDataIndx++] = sizeof(DeviceDateTime);
    gDataIndx = UploadBlockIntoBuffer( (BYTE *)&DeviceDateTime, gDataIndx, sizeof(DeviceDateTime) );
  }
  
  /* Request Device FW Info */
  if( UFrameReqDeviceMap.u._bits.fmrInfo )    /* 0001 */
  {    
    HFH_ev = 24;
    gData[gDataIndx++] = 2 + 2 + 1 + 1 + 2 + 4 + 1 + 4;
    gData[gDataIndx++] = FW_RELEASE & 0x00FF;
    gData[gDataIndx++] = (FW_RELEASE & 0xFF00)>>8;
    gData[gDataIndx++] = 0 & 0x00FF;
    gData[gDataIndx++] = (0 & 0xFF00)>>8;
    
    if( TSDProcessor::SDPresent == true )
      gData[gDataIndx++] = 0;    /* card present */
    else
      gData[gDataIndx++] = 2;    /* no card present */
    
    gData[gDataIndx++] = TSDProcessor::FATFS_Obj.csize;
    gData[gDataIndx++] = TSDProcessor::FATFS_Obj.database;
    gData[gDataIndx++] = TSDProcessor::FATFS_Obj.database >> 8;
    
    CharLong recLenght;
    recLenght.Long = 291762034L;
    gData[gDataIndx++] = recLenght.Char[0];
    gData[gDataIndx++] = recLenght.Char[1];
    gData[gDataIndx++] = recLenght.Char[2];
    gData[gDataIndx++] = recLenght.Char[3];
    
    /* Send ECG file travel info */
    if( TSDProcessor::NoAskReplaceECGFileQuestion == true )
      gData[gDataIndx++] = trverrGood;
    else{
//      if(TSDProcessor::Def == true )
//        TSDProcessor::TrMode = trverrGood;  
      gData[gDataIndx++] = TSDProcessor::TrMode;
    }
    
    CharLong trToken;
    
    trToken.Long = 0x00000001;
    gData[gDataIndx++] = trToken.Char[0];
    gData[gDataIndx++] = trToken.Char[1];
    gData[gDataIndx++] = trToken.Char[2];
    gData[gDataIndx++] = trToken.Char[3];
  }
  
  /* Request Device Batteries Voltages */
  if( UFrameReqDeviceMap.u._bits.fmrBatteries )    /* 0004 */
  {
    HFH_ev = 25;
      
    gData[gDataIndx++] = 2 + 2;

    CharInt tVB;
    tVB.Int = (uint16_t) ((uint32_t)(BatVoltageValue * 5539) >> 16);  //5484
    gData[gDataIndx++] = tVB.Char[0];
    gData[gDataIndx++] = tVB.Char[1];
    tVB.Int = (uint16_t) ((uint32_t)(BatVoltageValue * 10582) >> 16);  //9828
    gData[gDataIndx++] = tVB.Char[0];
    gData[gDataIndx++] = tVB.Char[1];
  }
  
  /* Request Device Write Started */
  if( UFrameReqDeviceMap.u._bits.fmrWrStarted )    /* 0008 */
  {
    HFH_ev = 27;
      
    if( ECGRecordingStarted == true )
      gData[gDataIndx++] = 0xFF;
    else
      gData[gDataIndx++] = 0x00;
  }

  uMap.Int = UFrameReqDeviceMap.u._word.value;
  gData[0] = uMap.Char[0];
  gData[1] = uMap.Char[1];
    HFH_ev = 26;

  SendFrameToHost( RPL_UNIFIED_FRAME, gDataIndx, gData );
  
}



/*----------------------------------------------------------------------------------*/
/* @brief  Read data from BT recieve buffer                    */
/* @param  None                                  */
/* @return  None                                  */
/*----------------------------------------------------------------------------------*/
uint8_t TAppProcessor::ReadBlockIntoBuffer( uint8_t *Buffer, uint8_t Indx )
{
  uint8_t BlockDataCount;
  
  BlockDataCount = gData[Indx++];
  
  for( uint8_t i = 0;i < BlockDataCount;i++ )
  {
    uint8_t bT = gData[Indx++];
    Buffer[i] = bT;
  }
  return Indx;
}



/*----------------------------------------------------------------------------------*/
/* @brief  Загружает данные в буффер блютуза для отправки              */
/* @param  None                                  */
/* @return  None                                  */
/*----------------------------------------------------------------------------------*/
BYTE TAppProcessor::UploadBlockIntoBuffer( uint8_t *Buffer, uint8_t Indx, uint8_t DataCount )
{
  for(BYTE i = 0;i < DataCount;i++)
  {
    gData[Indx++] = Buffer[i];
  }
  return Indx;
}

/*
void RTC_TimeStampToTDMY(uint32_t ttime,int *year,int *month, int *day,int *hours,int *minutes,int *seconds)
{
  //find year
  uint32_t yearIndx = 0;
  int i;
  for(i = 0;i < 27;i++)
  {
    yearIndx += (YearsDaysArray[i]*86400L);
    if(yearIndx > ttime) break;
  }
  yearIndx -= (YearsDaysArray[i]*86400L);
  *year = 2010 + i;
  i *= 12;//get month start Indx
  int j;
  uint32_t monthIndx = yearIndx;
  for(j = 0;j < 12;j++)
  {
    monthIndx += (YearsMonthArray[i++]*86400L);
    if(monthIndx > ttime) break;
  }
  monthIndx -= (YearsMonthArray[i - 1]*86400L);
  *month = j + 1;
  *day = (ttime - monthIndx) / (86400L);
  int daysec = *day * 86400L;
  *day += 1;
  monthIndx += daysec;
  daysec = (ttime - monthIndx);
  *hours = daysec / 3600L;
  int minsec = *hours * 3600L;
  monthIndx += minsec;
  minsec = (ttime - monthIndx);
  *minutes = minsec / 60L;
  *seconds = minsec % 60L;
}
*/


/*----------------------------------------------------------------------------------*/
/* @brief  Задача инициализации                      */
/* @param  None                                  */
/* @return  None                                  */
/*----------------------------------------------------------------------------------*/
void TAppProcessor::TASK_Init( void *pvParameters )
{
 
  
  TSound::SoundInit();
  TSound::PlaySound(_modiStartUp);
  TDevice::LedOn();
  GPIO_SetBits( TEST_PORT, TEST_PIN );
  
  
  
  while( 1 )
  {  
    if(GPIO_ReadInputDataBit(UB2_BUTTON_PORT, UB2_BUTTON_PIN) == 0)
    {
      
    }
  }
}



/*----------------------------------------------------------------------------------*/

void waitForSelectButtonPressed(){
  vTaskDelay(200/portTICK_RATE_MS);
   while(!buttonState){
                    if(GPIO_ReadInputDataBit(UB2_BUTTON_PORT, UB2_BUTTON_PIN) == 0)
                        {
                           buttonState = true;
                        }
                }
                buttonState = false;
                TLcdTrace::Clear();
}

/*----------------------------------------------------------------------------------*/
/* @brief  Задача инициализации-2                      */
/* @param  None                                  */
/* @return  None                                  */
/*----------------------------------------------------------------------------------*/

void TAppProcessor::TASK_StartUp(void *pvParameters)
{
      bool buttonState = false;
      bool start = false;
      if(!start){
                    if(GPIO_ReadInputDataBit(UB2_BUTTON_PORT, UB2_BUTTON_PIN) == 0)
                        {
                          
                           start = true;
                        
                        }
      }
      
      
      if(start){
  
	#ifdef LCD_TRACE
		char str1[10];
		char str2[20];
	#endif

	TSound::SoundInit();
	TSound::PlaySound(_modiStartUp);	
	
	

	TFlash::SF_Init();	

    #ifdef LCD_TRACE
		TLcdTrace::AddLine("Starting up system...");
		//TLcdTrace::AddLine("(C) CARDIAN 2016 BRU");
		strcpy(str2,"Firmware Ver:");
		itos((int)FW_RELEASE,str1,6,DECIMAL);
		strcat(str2,str1);
		#ifdef POWER_5V
			strcat(str2," 5V");
		#else
			strcat(str2," 3V");
		#endif
		#ifdef PUMP_CAP
			strcat(str2,"C");
		#endif
		TLcdTrace::AddLine(str2);
		
		#ifdef TWAIN
			strcpy(str2,"ECG Driver Ver:");
			itos((int)FW_ECG_RELEASE,str1,6,DECIMAL);
			strcat(str2,str1);
			TLcdTrace::AddLine(str2);
		#endif		

		stufSerialNumber *sn;
		sn = (stufSerialNumber*)TFlash::SF_GetDataPtr(SERIAL_NUM);
		int u = 0;
		int h = 0;
		for(u = 0;u < 11;u++)
		{
			str2[u] = (char)sn->DeviceName[h++];
		}
		str2[u] = 0;	
		TLcdTrace::AddLine(str2);
		strcpy(str2,(const char*)&sn->DeviceName[h]);
		TLcdTrace::AddLine(str2);
	#endif		

         vTaskDelay(1000/portTICK_RATE_MS);

	#ifdef LCD_TRACE
		TLcdTrace::AddLine("Here and bellow");
                TLcdTrace::AddLine("push button to continue...");
                waitForSelectButtonPressed();
	#endif

	

	#ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"Start GUI Task.");
                waitForSelectButtonPressed();
	#endif
	TGuiObjects::GUIObj_Init();

  
//  TLcdTrace::AddLine("START_UP_SYSTEM:");
//  itos((int)TFlash::SF_GetData8(START_UP_SYSTEM),str2,19,DECIMAL);
//	TLcdTrace::AddLine(str2);
      
  
	#ifdef GUI_ENABLED
      
    if(TFlash::SF_GetData8(START_UP_SYSTEM) == 0x55)
		{
			#ifdef LCD_TRACE
					TLcdTrace::AddLine("GUI Init...");
					          
          vTaskDelay(200/portTICK_RATE_MS);
			#endif
		
			TGui::GUI_Init();
			TGuiObjects::GUIDesktopShowProgressBar();
		}
	#endif		
	
	#ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"PWR Managment config...");
	#endif
	
		
	#ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"RT Clocks config...");
                TRtc::Rtc_Init();
                itos((int)TRtc::RTC_TimeRead(),str2,19,DECIMAL);
		TLcdTrace::AddLine(str2);
                
                waitForSelectButtonPressed();
	#endif		
		

	#ifdef LCD_TRACE
                TLcdTrace::AddLine(1,"Start ADC Init...");
                TADS1298::ADS1298_Init();
                TEcgProcessor::GetConvertResult();
                TInterpreter::SetStartUSARTMonitioring(true);
                
                 /* начать снятие ЭКГ сигнала с отведений */
/*#ifdef LCD_TRACE
    TLcdTrace::AddLine(1,"Start ECG.");
#endif //LCD_TRACE
    if( TDevice::DEVICE->Ecg.Status == true )
    {
      xTaskCreate( TEcgProcessor::TASK_WrProcessing, "ECGWrProcessing",
        configMINIMAL_STACK_SIZE+0, NULL,
        tskIDLE_PRIORITY+3, &TEcgProcessor::xhECGWrProcessing);


//#ifdef USE_ADS1292R
//      TADS1292::StartConversation();
//#endif // USE_ADS1292R
//      TADS1298::StartConversation();

    }*/   
                vTaskDelay(500/portTICK_RATE_MS);
                itos((int)TEcgProcessor::EcgData.Channel[3].ChannelBytes[1],str2,19,DECIMAL);
                TLcdTrace::AddLine(str2);
		TLcdTrace::AddLine(1,"ADC Init Complete!");
                
                itos((int)TAppProcessor::SysVoltageValue,str2,19,DECIMAL);
                TLcdTrace::AddLine(str2);
                vTaskDelay(500/portTICK_RATE_MS);
                itos((int)TAppProcessor::SysVoltageValue,str2,19,DECIMAL);
                TLcdTrace::AddLine(str2);
                
               waitForSelectButtonPressed();
               
                itos((int)TAppProcessor::AnalogVoltageValue,str2,19,DECIMAL);
                TLcdTrace::AddLine(str2);
                itos((int)TAppProcessor::LiVoltageValue,str2,19,DECIMAL);
                TLcdTrace::AddLine(str2);
                itos((int)TAppProcessor::BatVoltageValue,str2,19,DECIMAL);
                TLcdTrace::AddLine(str2);
                
                waitForSelectButtonPressed();
                
                
                TInterpreter::SetStartUSARTMonitioring(false); 
                
                waitForSelectButtonPressed();
	#endif
		
		

	#ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"Pressure Sensor Init...");
	#endif
	
		
	#ifdef ENABLE_WATCHDOG
	  IWDG_ReloadCounter();
	#endif

	#ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"Bluetooth config...");
	#endif		
	
	xTaskCreate(TFrame::TASK_CmdProcessor,
							(signed char *) "CMD",
							configMINIMAL_STACK_SIZE+200,
							NULL,
							tskIDLE_PRIORITY+1,
							&TFrame::xhCmdProcessor);

	TLMX9838::BluetoothInit();
	
	#ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"NAND Init...");
	#endif

	
		
	#ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"EEPROM(93Cxx) Init...");
                
                waitForSelectButtonPressed();
	#endif
	
		
	#ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"Pump&Valve Init...");
	#endif
	
//  TLMX9838::TurnOffBluetooth();
//  TWinstarLcd::LcdBkLightOff();
	
		
//  TWinstarLcd::LcdBkLightOn();
  	
	#ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"P Processor Init...");
	#endif
	

	#ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"T Sensor & BattZond...");
	#endif
	

	#ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"DataRead Init...");
	#endif
	

	#ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"DataWrite Init...");
                
                waitForSelectButtonPressed();
                
	#endif
	TNandWrite::NW_Init();

	#ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"Nibp Store Init...");
	#endif
	

	#ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"Virtual Machine Init.");
	#endif
	

    #ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"StartUp Completed.");
	#endif
			
    #ifdef LCD_TRACE
		TLcdTrace::AddLine(1,"Free heap size:");
		itos((int)xPortGetFreeHeapSize(),str2,19,DECIMAL);
		TLcdTrace::AddLine(str2);
	#endif
                waitForSelectButtonPressed();
                
                xTaskCreate( TAppProcessor::TASK_Loading, "Startup", configMINIMAL_STACK_SIZE+128, NULL,
                              tskIDLE_PRIORITY+0, NULL );
      }
      else{
            xTaskCreate( TAppProcessor::TASK_Loading, "Startup", configMINIMAL_STACK_SIZE+128, NULL,
                              tskIDLE_PRIORITY+0, NULL );
            xTaskCreate( TFrame::TASK_CheckISRFlag, "CheckISR", configMINIMAL_STACK_SIZE+128, NULL,
		tskIDLE_PRIORITY+2, NULL );
  
  xTaskCreate( TInterpreter::TASK_CmdProcessor, "InterpProcessor", 
				configMINIMAL_STACK_SIZE+1, NULL, 
				tskIDLE_PRIORITY+2, NULL ); 
  
  //vTaskStartScheduler();
      }

		
	vTaskSuspend(NULL);
}
//------------------------------------------------------------------------------------

/*----------------------------------------------------------------------------------*/
/* @brief  Задача запуска                              */
/* @param  None                                  */
/* @return  None                                  */
/*----------------------------------------------------------------------------------*/
void TAppProcessor::TASK_Loading( void *pvParameters )
{
  int prevCableCheck = ct5Leads; 
  char str2[20];
  
  SCB_DEMCR  |= 0x01000000;
  DWT_CYCCNT  = 0;
  DWT_CONTROL|= 1; // enable the counter

  vTaskDelay(100/portTICK_RATE_MS);

  TSound::SoundInit();
  TSound::PlaySound(_modiStartUp);
 
  while( 1 )
  {
    // загрузка заставки
//    if( TDevice::DEVICE->Lcd.Status == true )
//    {
//      TGui::GUI_PutBitmap( 0, 0, &_start_BMP, LCD_PIXEL_ON );
//      vTaskResume( TLcd::xLcdInvalidate );
//    }
    if(!ExitFromRecordMode)
    {  
//      TLcdTrace::AddLine("Starting up system...");
//      TLcdTrace::AddLine("CARDIAN 2014");
//      TLcdTrace::AddLine(FW_RELEASE_STR);

      stufSerialNumber *sn;
      sn = (stufSerialNumber*)TFlash::SF_GetDataPtr(SERIAL_NUM);
      if(sn->DeviceName[0] != 0xFF)
      {  
        int u = 0;
        int h = 0;
        for(u = 0;u < 13;u++)
        {
          str2[u] = (char)sn->DeviceName[h++];
        }
        str2[u] = 0;  
//        TLcdTrace::AddLine(str2);
        strcpy(str2,(const char*)&sn->DeviceName[h]);
//        TLcdTrace::AddLine(str2);
//        TLcdTrace::AddLine(" ");
      }
      vTaskDelay(800/portTICK_RATE_MS); 
    }
    
#ifndef DISABLE_GUI
    TLcdTrace::DisableLcdTrace = true;
#endif //DISABLE_GUI

    TLcdTrace::AddLine(1,"Start GUI Task");
    TGuiObjects::GUIDesktopShowNone();
    TGuiObjects::GUIObj_Init();    
    vTaskDelay(200/portTICK_RATE_MS); 
    TGuiObjects::GUIDesktopShowWait();
    vTaskDelay( 100 / portTICK_RATE_MS );

    // Подключение к ECGMCU
    TLcdTrace::AddLine(1,"Reseting ECGMCU...");
    for(int i = 0; i < 3; i++)
    {
      TInterpreter::SendFrameToHost(CMD_STOP_MONITORING);
      vTaskDelay( 200 / portTICK_RATE_MS );
    }
    TLcdTrace::AddLine(1,"Connection to ECGMCU...");
    int h;
    for(h = 0;h < 10;h++)
    {
      TInterpreter::SendFrameToHost(CMD_NULL_CMD);
      vTaskDelay( 500 / portTICK_RATE_MS );
      if(TInterpreter::Fx_Online) break;
    }
    if(h >= 9) 
    {
      TLcdTrace::AddLine(1,"FAIL");
      TGuiObjects::GUIDesktopShowError();
//      TGuiObjects::MessageBox(MSG_ERROR, false, "Да", "Нет", 30);
      while(1) 
      {  
        vTaskDelay( 30000 / portTICK_RATE_MS );
        PWR_EnterSTANDBYMode();
      }  
    }
    else
    {
      TLcdTrace::AddLine(1,"OK");
      vTaskDelay( 1200 / portTICK_RATE_MS );
    }


    TLcdTrace::AddLine(1,"Getting power status...");
    for(h = 0;h < 25;h++)
    {
      TInterpreter::SetStartUSARTMonitioring(true);
      vTaskDelay( 200 / portTICK_RATE_MS );
      TInterpreter::SetStartUSARTMonitioring(false);
      if(TAppProcessor::SysVoltageValue > 100) break;
    }
    if(h >= 25) 
    {
      TLcdTrace::AddLine(1,"FAIL");
      TGuiObjects::GUIDesktopShowError();
//      TGuiObjects::MessageBox(MSG_ERROR, false, "Да", "Нет", 30);
      while(1) 
      {  
        vTaskDelay( 30000 / portTICK_RATE_MS );
        PWR_EnterSTANDBYMode();
      }  
    }
    else
    {
      TLcdTrace::AddLine(1,"OK");
    }

    
    
    
    
    
    
    
    
    

    if(TAppProcessor::SysVoltageValue < 3100)
    {
      TLcdTrace::AddLine(1,"Waiting Li-Pol charging...");
      TGuiObjects::GUIDesktopShowCharge();
      for(h = 0;h < 900;h++)
      {
        TInterpreter::SetStartUSARTMonitioring(true);
        vTaskDelay( 100 / portTICK_RATE_MS );
        TInterpreter::SetStartUSARTMonitioring(false);
        vTaskDelay( 100 / portTICK_RATE_MS );
        if(TAppProcessor::SysVoltageValue > 3100) break;
        vTaskDelay( 800 / portTICK_RATE_MS );
      }
      if(h >= 899) 
      {
        TLcdTrace::AddLine(1,"FAIL");
        TGuiObjects::GUIDesktopShowError();
//        TGuiObjects::MessageBox(MSG_ERROR, false, "Да", "Нет", 30);
        while(1) 
        {  
          vTaskDelay( 30000 / portTICK_RATE_MS );
          PWR_EnterSTANDBYMode();
        }  
      }
      else
      {
        TLcdTrace::AddLine(1,"OK");
      }
    }

//    TInterpreter::SendFrameToHost(CMD_STOP_MONITORING);

    TLcdTrace::AddLine(1,"Getting ECGMCU CRC...");
    for(h = 0;h < 10;h++)
    {
      TInterpreter::SendFrameToHost(CMD_GETFLASHCRC);
      vTaskDelay( 500 / portTICK_RATE_MS );
      if(TInterpreter::Fx_CRC != 0) break;
    }
    if(h >= 9) 
    {
      TLcdTrace::AddLine(1,"FAIL");
      TGuiObjects::GUIDesktopShowError();
//      TGuiObjects::MessageBox(MSG_ERROR, false, "Да", "Нет", 30);
      while(1) 
      {  
        vTaskDelay( 30000 / portTICK_RATE_MS );
        PWR_EnterSTANDBYMode();
      }  
    }
    else
    {
      TLcdTrace::AddLine(1,"OK");
    }
    
#ifndef ECGMCUDONTFWUPDATE
    if(TInterpreter::Fx_CRC != ECGMCUFW_bin[ECGMCUFW_size/4-1])
    {
      TLcdTrace::AddLine(1,"ECGMCU FW updating...");
      TGuiObjects::GUIDesktopShowFirmwareUpdate();
      vTaskDelay( 100 / portTICK_RATE_MS );
      TSPISRAM::WriteSeq( 0, 4, (uint8_t *) &ECGMCUFW_size );
      TSPISRAM::WriteSeq( 4, ECGMCUFW_size, (uint8_t *) &ECGMCUFW_bin[0] );
      TInterpreter::SendFrameToHost(CMD_BOOTLOAD);

      for(h = 0;h < 10;h++)
      {
        TInterpreter::SendFrameToHost(CMD_NULL_CMD);
        vTaskDelay( 200 / portTICK_RATE_MS );
        if(TInterpreter::Fx_Online) break;
      }
      if(h >= 9) 
      {
        TLcdTrace::AddLine(1,"FAIL");
        TGuiObjects::GUIDesktopShowError();
//        TGuiObjects::MessageBox(MSG_ERROR, false, "Да", "Нет", 30);
        while(1) 
        {  
          vTaskDelay( 30000 / portTICK_RATE_MS );
          PWR_EnterSTANDBYMode();
        }  
      }
      else
      {
          TLcdTrace::AddLine(1,"OK");
      }
    }
#endif //ECGMCUDONTFWUPDATE

    TGuiObjects::GUIDesktopShowProgressBar();
    vTaskDelay( 100 / portTICK_RATE_MS );

    /* Включение блютуза */
    if( TDevice::DEVICE->Bluetooth.Status == true )
    {
      /* задача отсылки данных через блютуз */
      xTaskCreate( TFrame::TASK_SendDataToHost, "EDS",
        configMINIMAL_STACK_SIZE, NULL,
        tskIDLE_PRIORITY+2, &TFrame::xhECGDataSend);

      /* задача обработки команд с блютуза */
      xTaskCreate( TFrame::TASK_CmdProcessor, "CmdProcessor", 
        configMINIMAL_STACK_SIZE+64, NULL, 
        tskIDLE_PRIORITY+3, &TFrame::xhCmdProcessor );

      /* инициализация блютуза */
      TLMX9838::BluetoothInit();


#ifdef LCD_TRACE
      if(TLMX9838::LMX9838_Ready)
      {
        TLcdTrace::AddLine("LMX READY");
      }
      else
      {    
        TLcdTrace::AddLine("LMX FAIL");
      }   
#endif //LCD_TRACE
    }
    
    if( TDevice::DEVICE->Ecg.Status == true ){
    }
    
    vTaskDelay( 1000 / portTICK_RATE_MS );

    /* начать снятие ЭКГ сигнала с отведений */
#ifdef LCD_TRACE
    TLcdTrace::AddLine(1,"Start ECG.");
#endif //LCD_TRACE
    if( TDevice::DEVICE->Ecg.Status == true )
    {
      xTaskCreate( TEcgProcessor::TASK_WrProcessing, "ECGWrProcessing",
        configMINIMAL_STACK_SIZE+0, NULL,
        tskIDLE_PRIORITY+3, &TEcgProcessor::xhECGWrProcessing);

/*
#ifdef USE_ADS1292R
      TADS1292::StartConversation();
#endif // USE_ADS1292R
      TADS1298::StartConversation();
*/
    }

    /* Инициализация записи во флэш */
    TFlash::SF_Init();
//  TScreen::TimerTrigger = TFlash::SF_GetData16( LCDOFF_TIMER );    /* загрузка из флеш значения */
    
    TNandWrite::NW_Init();
    TLcdTrace::AddLine("NW_Init.");
    
    TPoolingNand::PO_Init();
    TLcdTrace::AddLine("PO_Init.");
        
    TAudio::AUDIO_PHYInit();
    
    TLcdTrace::AddLine("Loading SN.");
    stufSerialNumber *kr02_serialname = 
      (stufSerialNumber*)TFlash::SF_GetDataPtr(SERIAL_NUM);
    const uint8_t *sSn = (uint8_t*)kr02_serialname;
    uint8_t *dSn = (uint8_t*)&ufSerialNumber;

    for(int s = 0;s < sizeof(stufSerialNumber);s++) dSn[s] = sSn[s]; 
          
    TGuiObjects::GUIDesktopShowTime();
    
    while(1)
    {  
      
//      vTaskDelay(20000/portTICK_RATE_MS); 
//      PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
//      RCC_Configuration_Main();
//      SystemInit();
//      TFrame::TurnOnBluetooth();
  
      
      if( disk_initialize(0) == RES_OK )
      {
        TLcdTrace::AddLine("CARD INIT OK.");
        TLcdTrace::AddLineX("CARD Type ",CardType);
          
        TGuiObjects::ToolbarSet(TBFM_MEMCARD,1);
        
//      TGuiObjects::ToolbarSet(TBFM_STORED,1);
          
        while(StartRecording != true) 
        {
          static bool check_trv = true;
          
          if( disk_initialize(0) != RES_OK )
          {
            TGuiObjects::ToolbarSet(TBFM_MEMCARD,0);
            
            if (SD_Off_Sound)
            {
               TSound::PlaySound(_modiShortBeep);
        
               SD_Off_Sound = false;
               
               SD_On_Sound = true;
            }
                        
            TGuiObjects::ToolbarSet(TBFM_STORED,0);
            TSDProcessor::SDPresent = false;
            check_trv = true;
          }
          else
          {
            TGuiObjects::ToolbarSet(TBFM_MEMCARD,1);
            
            if (SD_On_Sound)
            {
               TSound::PlaySound(_modiShortBeep);
        
               SD_On_Sound = false;
               
               SD_Off_Sound = true;
            }            
            
            TSDProcessor::SDPresent = true;
            if(check_trv)
            {
              TLcdTrace::AddLine("Check Trv File");
              if( f_mount( 0, &FATFS_Obj ) == FR_OK )
              {
                if(f_open( &TrvFile,"KR01.TRE",FA_OPEN_EXISTING|FA_READ) == FR_OK )    /* File NOT exists */
                {
                  unsigned int BwW;
                  f_read( &TrvFile, TSDProcessor::EcgBuffer_1, 5, &BwW );
                  if( TSDProcessor::EcgBuffer_1[0] == 1 )    
                  {
                    TSDProcessor::TrMode = trverrNotRead;
                    TGuiObjects::ToolbarSet(TBFM_STORED,1);
                  }
                  else
                  {
                    TSDProcessor::TrMode = trverrGood;
                    TGuiObjects::ToolbarSet(TBFM_STORED,0);
                  }
                  f_close( &TrvFile );
                }
//                else TGuiObjects::ToolbarSet(TBFM_ERROR,1);
              }
            }
            check_trv = false;
          }
          
          cable_state = CableCheck();
          
          if(cable_state != prevCableCheck) 
          {
            ChangeCableIcon(cable_state);
            prevCableCheck = cable_state; 
          }
          vTaskDelay(1500/portTICK_RATE_MS);
        }

        RTC_WriteBackupRegister(RTC_BKP_DR3,0xAAAA);
        
        TNandWrite::FlashCardRemoved = false;
        
//        if( f_mount( 0, &FATFS_Obj ) == FR_OK )
        {
          unsigned int BwW;
          TLcdTrace::AddLine("Drive mounted");       
          
          TLcdTrace::AddLine("Creating File");
          FIL CodedFile;
//          f_rename( "KR01.RAW", "KR01.BAK" );

//          static portTickType LastTick = xTaskGetTickCount();
//          f_unlink( "KR01.RAW" );
          f_open( &CodedFile, "KR01.RAW", FA_CREATE_ALWAYS | FA_WRITE );
          f_close( &CodedFile );
//          uint32_t time = xTaskGetTickCount() - LastTick;
//          float fadxc = (float)time / 1000.;
//          char *ss = ftoa(fadxc,3);
//          TFont::DrawText(12, 205-SysFont[0].biHeight/2, ss, 80, algLeft);  

          
          TLcdTrace::AddLine("Create Trv File");
          f_open( &TrvFile, "KR01.TRE", FA_CREATE_ALWAYS | FA_WRITE );    /* Create file */           
          memset( TSDProcessor::EcgBuffer_1, 0x00, 5 );
          TSDProcessor::EcgBuffer_1[0] = 1;
          f_write( &TrvFile, TSDProcessor::EcgBuffer_1, 5, &BwW );    /* Write 1 in first byte, mean ECG file was read */
          TSDProcessor::TrMode = trverrGood;
          TGuiObjects::ToolbarSet(TBFM_STORED,0);
          f_close( &TrvFile );
          
          TLcdTrace::AddLine("PCard load.");
          int i = 0;
          for(i=0;i<1024;i++) karta[i]=0;
          
          for(i=0;i<16;i++)  karta[i] = inst1[i];
          for(i=17;i<25;i++) karta[i] = inst2_3[i-17]; //3отв
          for(i=32;i<55;i++) karta[i] = inst3_3[i-32]; //3отв
          for(i=56;i<76;i++) karta[i] = PatientCard.Name[i-56];//Fio[i-56];
          karta[116] = PatientCard.Sex;//'М';
          for(i=117;i<127;i++) karta[i] = PatientCard.DateOfBirth[i-117];//Year[i-117];
          for(i=136;i<140;i++) karta[i] = vozr[i-136];
          for(i=140;i<150;i++) karta[i] = exDate[i-140];
          for(i=150;i<158;i++) karta[i] = exTime[i-150];
          for(i=166;i<171;i++) karta[i] = inst4[i-166]; 
          for(i=174;i<182;i++) karta[i] = dlit_v[i-174];           
          for(i=182;i<202;i++) karta[i] = inst5_3[i-182]; //3отв -8
          karta[239]=0x17; karta[240] = 0;  //Время начала сна
          karta[259]=0x7; karta[260] = 0;   //Время окончания сна
          
          PatientCard.DateTimeOfStart.Date = DeviceDateTime.Date;
          PatientCard.DateTimeOfStart.Time = DeviceDateTime.Time;

          switch(cable_state)
          {
#ifdef USE_ADS1292R
            case ct5Leads:
              ecg_format = 1;
              break;
            case ct7Leads:
              ecg_format = 1;
              break;
#else //USE_ADS1292R
            case ct5Leads:
              ecg_format = 0;
              break;
            case ct7Leads:
              ecg_format = 0;
              break;
#endif // USE_ADS1292R
            case ct10Leads:
              ecg_format = 2;
              break;
            default:
              ecg_format = 7;
              break;
          }
          PatientCard.ECGAdvancedOption = cable_state | (ecg_format << 3);


//          if (cable_state == ct10Leads) PatientCard.ECGAdvancedOption |= 0x10;
//          else PatientCard.ECGAdvancedOption |= 0x08;

          TLcdTrace::AddLine("Save PCard to SD.");
          f_open( &PatCardFile, "KR01.PAT", FA_CREATE_ALWAYS | FA_WRITE );    /* Create file */           
          f_write( &PatCardFile, (BYTE*)&PatientCard, 245, &BwW );    /* Write 1 in first byte, mean ECG file was read */
          f_close( &PatCardFile );
        
          TLcdTrace::AddLine("Write patient card.");
          wfType pc_recType;
          *((uint32_t *)&pc_recType) = 0;
          pc_recType.IsDummy = 1;
          pc_recType.Take = 0;
          TPoolingNand::SetupPool(1,pc_recType);    
          
          for(int j = 0;j < 32;j++) //place racord
          {  
            TPoolingNand::AddDataToPool(1,(uint8_t*)&_ff_array[0],64,0);
            vTaskDelay(5/portTICK_RATE_MS);
          }
          TPoolingNand::FinishPool(1);

          vTaskDelay(200/portTICK_RATE_MS);
          
          *((uint32_t *)&pc_recType) = 0;
          pc_recType.IsPatientCard = 1;
          pc_recType.Take = 0;
          TPoolingNand::SetupPool(1,pc_recType);    

          TPoolingNand::AddDataToPool(1,(uint8_t*)&PatientCard,sizeof(stPatientCardV1),0);
          TPoolingNand::AddDataToPool(1,(uint8_t*)&karta[0],1024,0);
          TPoolingNand::FinishPool(1);
          
          vTaskDelay(200/portTICK_RATE_MS);
                    
          wfType recTypeEcg;
          *((uint32_t *)&recTypeEcg) = 0;
          recTypeEcg.IsECG = 1;
          recTypeEcg.Take = 0;
          TPoolingNand::SetupPool(1,recTypeEcg);  
          TLcdTrace::AddLine("EcgPool Setup.");
          
          TSDProcessor::SDStartWrPool = true;
          
          // Send "CMD_SET_LEDMODE", LedMode = 2 - HR Indication  
          BYTE EnMode[1];
          
          EnMode[0] = 2;          
          
          TInterpreter::SendFrameToHost(CMD_SET_LEDMODE,1,EnMode);
          vTaskDelay(1/portTICK_RATE_MS);
          TInterpreter::SendFrameToHost(CMD_SET_LEDMODE,1,EnMode);
          vTaskDelay(1/portTICK_RATE_MS);
          TInterpreter::SendFrameToHost(CMD_SET_LEDMODE,1,EnMode);
          
//        if (TFrame::StartMonitioring /*|| TSDProcessor::SDStartWrPool*/ || TGuiObjects::EcgOnScreen) TInterpreter::SetStartUSARTMonitioring(true);
//          else TInterpreter::SetStartUSARTMonitioring(false);
          
          TInterpreter::SetStartECGWrite(true);
          vTaskDelay(1/portTICK_RATE_MS);
          TInterpreter::SetStartECGWrite(true);
          vTaskDelay(1/portTICK_RATE_MS);
          TInterpreter::SetStartECGWrite(true);
          
          TPoolingNand::Pool1WRConfirm = false;
          vTaskDelay(100/portTICK_RATE_MS);
          int start_timer = 0;
          while(!TPoolingNand::Pool1WRConfirm)
          {
            start_timer++;
            if(start_timer == 200)
            {
              TSound::PlaySound(_modiStopMeasureError);
              vTaskDelay(2000/portTICK_RATE_MS);
              TDevice::SystemReset();
            }
            vTaskDelay(200/portTICK_RATE_MS);
          }

          TSound::PlaySound(_modiSetUp);
          ECGRecordingStarted = true;
         
          if(TGuiObjects::ButtonModeStart) 
            TLMX9838::TurnOffBluetooth();
          TGuiObjects::ButtonModeStart = false;

//        TAudio::AUDIO_PHYInit();
//
          TGuiObjects::StandbyLedEnable = false;
          
          RecordInProgress = true;
          
          RTC_WriteBackupRegister(RTC_BKP_DR3,0x5555);
          
          while(TSDProcessor::SDPresent)
          {  
            if(AudioRec)
            {
              TLcdTrace::AddLine("Au Rec Start.");
              wfType recType;
              *((uint32_t *)&recType) = 0;
              recType.IsSND = 1;
              recType.Take = sndTakeN++;
              TPoolingNand::SetupPool(2,recType);    
              
              audio_rec_start = true;

              while(AudioRec)
              {
                vTaskDelay(100/portTICK_RATE_MS);
              }
              TPoolingNand::FinishPool(2);  
              TLcdTrace::AddLine("Rec Stop.");

              audio_rec_start = false;

            }
            vTaskDelay(50/portTICK_RATE_MS);
          }
          TPoolingNand::FinishPool(1);  
          TLMX9838::TurnOnBluetooth();
        }
      }
      else
      {
//#ifdef LCD_TRACE
//        TLcdTrace::AddLine("CARD Not Present.");
//#endif
      }
      
      cable_state = CableCheck();
      
      if(cable_state != prevCableCheck) 
      {
        ChangeCableIcon(cable_state);
        prevCableCheck = cable_state; 
      }
          
      vTaskDelay(1000/portTICK_RATE_MS);
    }    
    
//    vTaskDelete( NULL );
  }
}




/*----------------------------------------------------------------------------------*/
/* @brief  Задача мигания светодиодом                      */
/* @param  None                                  */
/* @return  None                                  */
/*----------------------------------------------------------------------------------*/
void TAppProcessor::TASK_LEDblink( void *pvParameters )
{
  static uint8_t blink_cnt = 2;
  
  while( 1 )
  {
    if(TEcgProcessor::HR_Led_Enable) vTaskSuspend( NULL );
    vTaskDelay( 3000 / portTICK_RATE_MS );
    if(TSDProcessor::SDPresent) blink_cnt = 4;
    else blink_cnt = 2;
    for(int i = 0; i < blink_cnt; i++)
    {  
      if(!TEcgProcessor::HR_Led_Enable) TDevice::LedOn();
      vTaskDelay( 20 / portTICK_RATE_MS );
      TDevice::LedOff();
      vTaskDelay( 230 / portTICK_RATE_MS );
    }
  }
}



/*----------------------------------------------------------------------------------*/

int TAppProcessor::CableCheck()
{
  if (!TInterpreter::StartUSARTMonitioring)
  {
    TInterpreter::SetStartUSARTMonitioring(true);
    vTaskDelay( 100 / portTICK_RATE_MS );
    if (TFrame::StartMonitioring /*|| TSDProcessor::SDStartWrPool*/ || TGuiObjects::EcgOnScreen) TInterpreter::SetStartUSARTMonitioring(true);
    else TInterpreter::SetStartUSARTMonitioring(false);
  }
  switch(TEcgProcessor::EcgData.Status.ChannelBytes[0] & 0x03)
  {
    case 0:
      return ct10Leads;
    case 1:
      return ct7Leads;
    case 2:
      return ct5Leads;
    default:
    case 3:
      return ctNoCable;
  }
}
/*----------------------------------------------------------------------------------*/

void TAppProcessor::ChangeCableIcon(int NewState)
{

  TGuiObjects::ToolbarSet(TBFM_NOLEAD,0);
  TGuiObjects::ToolbarSet(TBFM_LEAD5,0);
  TGuiObjects::ToolbarSet(TBFM_LEAD7,0);
  TGuiObjects::ToolbarSet(TBFM_LEAD10,0);
  switch(NewState)
  {
    case ct5Leads:
      TGuiObjects::ToolbarSet(TBFM_LEAD5,1);
      break;
    case ct7Leads:
      TGuiObjects::ToolbarSet(TBFM_LEAD7,1);
      break;
    case ct10Leads:
      TGuiObjects::ToolbarSet(TBFM_LEAD10,1);
      break;
    default:
      TGuiObjects::ToolbarSet(TBFM_NOLEAD,1);
      break;
  }
  TGuiObjects::ShowToolbar();
}
