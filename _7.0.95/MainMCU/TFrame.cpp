#include "TFrame.h"
#include "TAppProcessor.h"
#include "TLcdTrace.h"
//#include "TRespProcessor.h"
#include "TGuiObjectsV2.h"
//------------------------------------------------------------------------------------
volatile uint8_t TFrame::UartBuffer[ USART_BUFFER_SIZE ];
volatile int  TFrame::UartBufferIndx = 0;
volatile bool TFrame::UartBuffNotEmpty = false;
//------------------------------------------------------------------------------------
uint8_t TFrame::xBuffer[ XBUFFER_SIZE ];
uint8_t TFrame::gDataLenght = 0;
uint8_t *TFrame::gData = NULL;
//------------------------------------------------------------------------------------
int16_t TFrame::SampleDataBuffer[MONITORINGSAMPLEDATABUFF_SIZE];
uint8_t TFrame::inSmpDataIndx = 0;
uint8_t TFrame::outSmpDataIndx = 0;
//------------------------------------------------------------------------------------
xTaskHandle TFrame::xhECGDataSend;
xTaskHandle TFrame::xhBPDataSend;
xTaskHandle TFrame::xhCmdProcessor;
int HFH_Event = 0;
//------------------------------------------------------------------------------------
bool TFrame::LSTAT_Stored = false;
//------------------------------------------------------------------------------------
bool TFrame::TurnOffAfterLinkDisconnected = false;
//------------------------------------------------------------------------------------
bool TFrame::StartMonitioring = false;
bool TFrame::StartUpBluetooth = false;

bool TFrame::ResumeFromISR = false;
//------------------------------------------------------------------------------------

void TFrame::ISR_USART_Rcv(void)
{
  UartBuffer[UartBufferIndx] = (uint8_t)USART_ReceiveData(LMX_USART);
  UartBufferIndx++;
  if(UartBufferIndx == (USART_BUFFER_SIZE - 1)) UartBufferIndx = (USART_BUFFER_SIZE - 1);
  UartBuffNotEmpty = true;
//  xTaskResumeFromISR(xhCmdProcessor);
}
//------------------------------------------------------------------------------------

uint8_t TFrame::CommandRequest()
{

  uint8_t UartBufferIndxEnd = UartBufferIndx;
  static uint8_t UartBufferIndxRd = 0;
  static uint8_t FrameBufferIndx = 0;

  static uint8_t FrameReadsCnt = 0;
  static uint8_t FrameDataCnt = 0xFF;

  static bool IsLMXFrameBegin = false;
  static bool IsCPMFrameBegin = false;

  static uint8_t TryingAttemptsToFrameRead = 0;

  if(!UartBuffNotEmpty)
    {
      if(IsLMXFrameBegin || IsCPMFrameBegin)
        {
          TryingAttemptsToFrameRead++;

          if(TryingAttemptsToFrameRead > 100)
            {
              IsLMXFrameBegin = false;
              IsCPMFrameBegin = false;
              FrameBufferIndx = 0;
              FrameReadsCnt = 0;
              FrameDataCnt = 0xFF;
              /* Needs to processing dropped frames */
            }
        }
      return crqNoneFrame;
    }

  TryingAttemptsToFrameRead = 0;

  if(UartBufferIndxRd == UartBufferIndxEnd)
    {
      UartBufferIndxRd = 0;
      UartBufferIndx = 0;
      UartBuffNotEmpty = false;
      return crqNoneFrame;
    }


  for(uint8_t j = UartBufferIndxRd;j < UartBufferIndxEnd;j++)
    {
      if(!IsLMXFrameBegin && !IsCPMFrameBegin)
        {
          if(UartBuffer[UartBufferIndxRd] == 0x02)
            {
              IsLMXFrameBegin = true;
            }
          if(UartBuffer[UartBufferIndxRd] == 0x55)
            {
              IsCPMFrameBegin = true;
            }
        }
      else
        {
          if(IsLMXFrameBegin)
            {
              uint8_t UartDataByte = UartBuffer[UartBufferIndxRd];

              if(FrameReadsCnt == 2) FrameDataCnt = UartDataByte + 4;

              xBuffer[FrameBufferIndx++] = UartDataByte;
              FrameReadsCnt++;
              FrameDataCnt--;

              if(FrameDataCnt == 0)
                {
                  IsLMXFrameBegin = false;
                  if(UartDataByte == 0x03)
                    {
                      UartBufferIndxRd++;
                      FrameReadsCnt = 0;
                      FrameDataCnt = 0xFF;
                      gDataLenght = FrameBufferIndx;
                      FrameBufferIndx = 0;
                      return crqLMXFrame;
                    }
                }
            }
          else
            if(IsCPMFrameBegin)
              {
                uint8_t UartDataByte = UartBuffer[UartBufferIndxRd];

                if(FrameReadsCnt == 1) FrameDataCnt = UartDataByte + 3;

                xBuffer[FrameBufferIndx++] = UartDataByte;
                FrameReadsCnt++;
                FrameDataCnt--;

                if(FrameDataCnt == 0)
                  {
                    IsCPMFrameBegin = false;
                    if(UartDataByte == 0xAA)
                      {
                        UartBufferIndxRd++;
                        FrameReadsCnt = 0;
                        FrameDataCnt = 0xFF;
                        gDataLenght = FrameBufferIndx;
                        FrameBufferIndx = 0;
                        return crqCPMFrame;
                      }
                  }
              }
        }
      UartBufferIndxRd++;
    }


  return crqNoneFrame;
}

/*----------------------------------------------------------------------------------*/
void TFrame::SetState()
{
  if(LMX9838_Ready)
  {

    if(GPIO_ReadInputDataBit(LMX_LSTAT_GPIO_PORT,LMX_LSTAT_GPIO_PIN))  /* если блютуз установили соединение */
    {
      if(LMX9838_GetOnOffState()) LMX9838_Connected = true;
      else LMX9838_Connected = false;
      if(LMX9838_GetOnOffState())
      {
//            GPIO_SetBits( TEST_PORT, TEST_PIN );
        vTaskDelay(100/portTICK_RATE_MS);
        SendFrameToHost(RPL_LINKED_MARK,25,(uint8_t *)TAppProcessor::ufSerialNumber.DeviceName);

        TGuiObjects::ToolbarSet(TBFM_BLUETOOTH,0);
        TGuiObjects::ToolbarSet(TBFM_BLUETOOTHONAIR,1);
      }

    }
    else    /* соединения нет */
    {
      LMX9838_Connected = false;
      if(LMX9838_GetOnOffState())
      {
//            GPIO_ResetBits( TEST_PORT, TEST_PIN );
        TGuiObjects::ToolbarSet(TBFM_BLUETOOTH,1);
        TGuiObjects::ToolbarSet(TBFM_BLUETOOTHONAIR,0);
      }
    }
  }
}
//------------------------------------------------------------------------------------

void TFrame::TASK_CmdProcessor( void *pvParameters )
{
  LSTAT_Stored = 0;

  while(1)
  {
//    if(!LMX9838_GetOnOffState()) vTaskSuspend(NULL);
//    if(!LMX9838_GetOnOffState()) vTaskSuspend(NULL);

     uint8_t CmtType = CommandRequest();

    HFH_Event = 4;
    //проверка запуска задачи по откр. порта с задержкой

    if((CmtType == crqNoneFrame)
      && (GPIO_ReadInputDataBit(LMX_LSTAT_GPIO_PORT,LMX_LSTAT_GPIO_PIN) == 0)
      && (BT_On == true) && StartUpBluetooth)
     {
      SetState();
#ifdef LCD_TRACE
      TLcdTrace::AddLine("TCMD SUSP");
#endif
      if(TurnOffAfterLinkDisconnected)
      {
        TurnOffAfterLinkDisconnected = false;
        if (TAppProcessor::RecordInProgress)
        {
          TurnOffBluetooth();
        }
      }

       vTaskSuspend( NULL ); //TFrame::xhCmdProcessor

      vTaskDelay(10/portTICK_RATE_MS);

      //vTaskDelay( 100/portTICK_RATE_MS );

      SetState();

      #ifdef LCD_TRACE
         TLcdTrace::AddLine("Link Established");
      #endif

      continue;
     }

     if(CmtType == crqLMXFrame)
     {
      uint8_t _type;
       uint8_t Packet = ParseLMXCommand(&_type);
      HFH_Event = 1;
       LMX9838_ParceEvents(Packet,_type,gData);
     }
     else if(CmtType == crqCPMFrame)
    {
       uint8_t _datacnt;
       uint8_t Packet = ParseSysCommand(&_datacnt);
      HFH_Event = 2;
       TAppProcessor::App_ParceEvents(Packet,_datacnt,gData);
    }
    HFH_Event = 3;

    vTaskDelay(2/portTICK_RATE_MS);    /* 10 ms period */
   }
}
//------------------------------------------------------------------------------------

uint8_t TFrame::ParseLMXCommand(uint8_t *Type)
{
  uint8_t retByte = xBuffer[0];

  *Type = xBuffer[1];
//  for(uint8_t i = 0;i < *DataCount;i++) Data[i] = xBuffer[i + 2];
  gData = &xBuffer[2];

  return retByte;
}
//------------------------------------------------------------------------------------

uint8_t TFrame::ParseSysCommand(uint8_t *DataCount)
{
  uint8_t retByte = xBuffer[0];

  *DataCount = xBuffer[1];
//  for(uint8_t i = 0;i < *DataCount;i++) Data[i] = xBuffer[i + 2];
  gData = &xBuffer[2];

  return retByte;
}
//------------------------------------------------------------------------------------

bool TFrame::SendDataFrameToHost( uint8_t smplIndx )
{
  if(!LMX9838_Connected) return false;

  uint8_t oVfByte = 0;


  if (TAppProcessor::cable_state == ct10Leads)
  {
     SendByte(0x5A, false);    /* SBM data frame */
    for(uint8_t i = 0;i < 10;i++)          //!!!  8  !!!
    {
      CharInt rtData;
      int sdbIndx  = i + smplIndx;
      if(sdbIndx < MONITORINGSAMPLEDATABUFF_SIZE)
        rtData.Int = SampleDataBuffer[sdbIndx];
      SendByte(rtData.Char[1], false);
      SendByte(rtData.Char[0], false);
    }

/*    for(uint8_t i = 0;i < 2;i++)
    {
      Pila[i] = Pila[i] + 10*((i + 1) * (i + 1));
      if (Pila[i] > 2048) Pila[i] = -2048;
      CharInt rtData;
      rtData.Int = Pila[i];
      SendByte(rtData.Char[1], false);
      SendByte(rtData.Char[0], false);
    }
*/

//    SendByte(0, false);
//    SendByte(0, false);
//    SendByte(0, false);
//    SendByte(0, false);
    SendByte(21, false);
    SendByte(22, false);
    SendByte(23, false);
    SendByte(24, false);
    SendByte(25, false);
    SendByte(0xA5, true);
  } else {
    SendByte(0x5E, false);    /* SBM data frame */
    for(uint8_t i = 0;i < 3;i++)
    {

      CharInt rtData;

      int sdbIndx  = i + smplIndx;
      if(sdbIndx < MONITORINGSAMPLEDATABUFF_SIZE)
        rtData.Int = SampleDataBuffer[sdbIndx];

      SendByte(rtData.Char[1], false);
      SendByte(rtData.Char[0], false);


    }
    SendByte(oVfByte, false);
    SendByte(0xE5, true);
  }

  return true;
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

bool TFrame::SendFrameToHost(uint8_t FrameID, uint8_t BytesCnt,const uint8_t *Data)
{
  if(!LMX9838_Connected) return false;

  CharInt crcBCC;

  crcBCC.Int = 0;

  SendByte(0x55);

  SendByte(FrameID);

  SendByte(BytesCnt);

  for(uint8_t i = 0;i < BytesCnt;i++)
    {
      SendByte(Data[i]);
      crcBCC.Int += Data[i];
    }

  crcBCC.Int += FrameID;
  crcBCC.Int += BytesCnt;

  SendByte(crcBCC.Char[0]);

  SendByte(0xAA);

  return true;
}
//------------------------------------------------------------------------------------

bool TFrame::SendFrameToHost(uint8_t FrameID, uint8_t BytesCnt, uint8_t Data)
{
  if(!LMX9838_Connected) return false;

  CharInt crcBCC;

  crcBCC.Int = 0;

  SendByte(0x55);

  SendByte(FrameID);

  SendByte(BytesCnt);

  SendByte(Data);
  crcBCC.Int += Data;

  crcBCC.Int += FrameID;
  crcBCC.Int += BytesCnt;

  SendByte(crcBCC.Char[0]);

  SendByte(0xAA);

  return true;
}
//------------------------------------------------------------------------------------

bool TFrame::SendFrameToHost(uint8_t FrameID)
{
  if(!LMX9838_Connected) return false;

  CharInt crcBCC;

  crcBCC.Int = 0;

  SendByte(0x55);

  SendByte(FrameID);

  SendByte(0);

  crcBCC.Int += FrameID;

  SendByte(crcBCC.Char[0]);

  SendByte(0xAA);

  return true;
}
//------------------------------------------------------------------------------------

void TFrame::ClearxBuffer()
{
  for(int i = 0;i < XBUFFER_SIZE;i++) xBuffer[i] = 0;
}

void TFrame::TASK_CheckISRFlag(void *pvParameters)
{
  while(1)
  {
    if (ResumeFromISR)
    {
      vTaskResume(xhCmdProcessor);

      ResumeFromISR = false;
    }

    vTaskDelay(5/portTICK_RATE_MS);

  }
}



/*----------------------------------------------------------------------------------*/
/* @brief  Send data via bluetooth                          */
/* @param  ECG data struct                              */
/* @return  None                                  */
/*----------------------------------------------------------------------------------*/
volatile bool addmutex = true;
void TFrame::AddSampleToMonitoringDataBuffer( EcgData_t *Data )
{
  addmutex = true;

  int32_t sData[10];
  uint8_t i_max = 3;
  uint8_t ch_cnt = 3;
  if(TAppProcessor::cable_state == ct10Leads)
  {
    i_max = 10;
    ch_cnt = 8;
    sData[8] = 0;
    if (Data->Status.ChannelBytes[0] & 0x08) 
    {
      sData[8] = 5000;
    }
    sData[9] = Data->Channel[8].ChannelData;
  }

  for(uint8_t i = 0; i < i_max; i++)
  {
    sData[i] = Data->Channel[i].ChannelData;
  }

  for( uint8_t i = 0; i < i_max; i++ )
  {
    sData[i] = (sData[i] * 1015L) / 1000L;
    if( sData[i] > 32767L ) sData[i] = 32767L;
    else if( sData[i] < -32768L ) sData[i] = -32768L;
    int sdbIndx = inSmpDataIndx + i;
    if(sdbIndx < MONITORINGSAMPLEDATABUFF_SIZE)
    {
      SampleDataBuffer[ sdbIndx ] = (int16_t)sData[ i ];
    }
  }
  inSmpDataIndx += i_max;
  if( inSmpDataIndx >= MONITORINGSAMPLEDATABUFF_SIZE ) 
  {
    inSmpDataIndx = 0;
  }
  addmutex = false;
}
//------------------------------------------------------------------------------------

void TFrame::TASK_SendDataToHost(void *pvParameters)
{
  while(1)
  {
    if(!LMX9838_Connected)
    {
      vTaskDelay( 5/portTICK_RATE_MS );
//      vTaskSuspend(NULL);
      continue;
    }

    if(!addmutex)
    {
      while(outSmpDataIndx != inSmpDataIndx)
        {
          SendDataFrameToHost(outSmpDataIndx);
          if (TAppProcessor::cable_state == ct10Leads) outSmpDataIndx += 10;  //!!!  8  !!!
          else outSmpDataIndx += 3;
          if(outSmpDataIndx >= MONITORINGSAMPLEDATABUFF_SIZE) outSmpDataIndx = 0;
        }
    }
    vTaskDelay( 5/portTICK_RATE_MS );    /* 10 ms period */
  }
}
//------------------------------------------------------------------------------------

void TFrame::ClearMonitoringDataBuffer()
{
  inSmpDataIndx = 0;
  outSmpDataIndx = 0;
  inNIBPSmpDataIndx = 0;
  outNIBPSmpDataIndx = 0;
}
//------------------------------------------------------------------------------------
