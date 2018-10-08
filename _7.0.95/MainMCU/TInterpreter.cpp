/************************************************************************************/
/* @title		  Interpreter.h     										*/
/* @author		Cyber Genius													*/
/* @version		0.1.1																  */
/* @date		  13.11.2015														*/
/************************************************************************************/

//------------------------------------------------------------------------------------

#include "main.h"

#include "TInterpreter.h"

#include "TRtc.h"

#include "TEcgProcessor.h"
#include "TRespProcessor.h"
#include "TSDProcessor.h"
#include "TGuiObjectsV2.h"
#include "TSound.h"

#ifdef LCD_TRACE
   #include "TLcdTrace.h"
#endif

//------------------------------------------------------------------------------------

bool TInterpreter::Fx_Online = false;
bool TInterpreter::StartUSARTMonitioring = false;
bool TInterpreter::StartECGWrite = false;

uint32_t TInterpreter::Fx_CRC = 0;

bool TInterpreter::Uart_Connected = false;

volatile uint8_t TInterpreter::IterpBuffer[ USART_BUFFER_SIZE ];
volatile uint8_t TInterpreter::IterpBufferIndx = 0;
volatile bool TInterpreter::IterpBuffNotEmpty = false;

uint8_t TInterpreter::xBuffer[ XBUFFER_SIZE ];
BYTE *TInterpreter::gData = NULL;
uint8_t TInterpreter::gDataLenght = 0;

uint32_t tempvar = 0;

TTimeStampEV TInterpreter::IntDateTime;
//------------------------------------------------------------------------------------

void TInterpreter::UartPHYInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // RCC config
  UART_GPIO_RCC_ENABLE;
  UART_RCC_ENABLE;

  GPIO_InitStructure.GPIO_Pin = UART_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  GPIO_PinAFConfig(UART_TX_PORT, UART_TX_PINSOURCE, UART_AF);

  GPIO_Init(UART_TX_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = UART_RX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  GPIO_PinAFConfig(UART_RX_PORT, UART_RX_PINSOURCE, UART_AF);

	GPIO_Init(UART_RX_PORT, &GPIO_InitStructure);

  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure one bit for preemption priority */
	//NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	/* Enable the USART3 Interrupt */

  NVIC_InitStructure.NVIC_IRQChannel = UART_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 11;                     //10
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate =  256000;   //
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  //USART_OverSampling8Cmd(INTER_PORT, ENABLE);

	USART_Init(INTER_PORT, &USART_InitStructure);

  USART_ITConfig(INTER_PORT, USART_IT_RXNE, ENABLE);

  TurnOnUart();
}
//------------------------------------------------------------------------------------

void TInterpreter::UartInit()
{
  UartPHYInit();
}
//------------------------------------------------------------------------------------

void TInterpreter::SetStartUSARTMonitioring(bool state)
{
  StartUSARTMonitioring = state;
  if (StartUSARTMonitioring)
  {
    SendFrameToHost(CMD_START_MONITORING);
    SendFrameToHost(CMD_START_MONITORING);
  }
  else 
  {
    SendFrameToHost(CMD_STOP_MONITORING);
    SendFrameToHost(CMD_STOP_MONITORING);
  }
}
//------------------------------------------------------------------------------------

void TInterpreter::SetStartECGWrite(bool state)
{
  StartECGWrite = state;
  if (StartECGWrite)
  {
    SendFrameToHost(CMD_START_ECGWRITE);
    SendFrameToHost(CMD_START_ECGWRITE);
  }
  else 
  {
    SendFrameToHost(CMD_STOP_ECGWRITE);
    SendFrameToHost(CMD_STOP_ECGWRITE);
  }
}
//------------------------------------------------------------------------------------

void TInterpreter::TurnOnUart()
{
	USART_Cmd(INTER_PORT, ENABLE);

  Uart_Connected = true;
}

void TInterpreter::TurnOffUart()
{
  USART_Cmd(INTER_PORT, DISABLE);

  Uart_Connected = false;
}

void TInterpreter::SendByte(BYTE data)
{
  USART_SendData(INTER_PORT, data);
  while(USART_GetFlagStatus(INTER_PORT, USART_FLAG_TXE) == RESET) __no_operation();//taskYIELD(); //1093
}
//------------------------------------------------------------------------------------

bool TInterpreter::ReceiveByte(BYTE *bt)
{
  if(USART_GetFlagStatus(INTER_PORT, USART_FLAG_RXNE) == RESET)
  	*bt = USART_ReceiveData(INTER_PORT);
  else return false;

  return true;
}
//------------------------------------------------------------------------------------

void TInterpreter::ISR_USART_Rcv(uint16_t data)
{
  if (!StartUSARTMonitioring)
  {
    TInterpreter::IterpBuffer[TInterpreter::IterpBufferIndx] = (uint8_t)data;
    TInterpreter::IterpBufferIndx++;
    if(TInterpreter::IterpBufferIndx == (USART_BUFFER_SIZE - 1)) TInterpreter::IterpBufferIndx = (USART_BUFFER_SIZE - 1);
    IterpBuffNotEmpty = true;
  }
  else
  {
    uint8_t rcvData = (uint8_t)data;
    TInterpreter::IterpBuffer[TInterpreter::IterpBufferIndx] = rcvData;
    TInterpreter::IterpBufferIndx++;

    static bool StartMarker = false;

    if(!StartMarker)
    {
      if(rcvData == 0x55)
      {
        StartMarker = true;
        TInterpreter::IterpBufferIndx = 0;
      }
    }
    else
    {
      if (IterpBufferIndx >= (MONITOR_DATA_LENGTH-1))
      {
        StartMarker = false;
        MonitorDataParseFunc(-1);
      }
    }
  }
}
//------------------------------------------------------------------------------------

void TInterpreter::TASK_CmdProcessor( void *pvParameters )
{
  while(1)
  {
    if (!TAppProcessor::StartMonitioring)
    {
      ControllerInteraction();
		}

    vTaskDelay(3/portTICK_RATE_MS);
  }
}

uint8_t TInterpreter::CommandRequest()
{

	uint8_t IterpBufferIndxEnd = TInterpreter::IterpBufferIndx;
	static uint8_t IterpBufferIndxRd = 0;
	static uint8_t FrameBufferIndx = 0;

	static uint8_t FrameReadsCnt = 0;
	static uint8_t FrameDataCnt = 0xFF;

	static bool IsLMXFrameBegin = false;
	static bool IsCPMFrameBegin = false;

	static uint8_t TryingAttemptsToFrameRead = 0;

	if(!IterpBuffNotEmpty)
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
			return crqUNoneFrame;
		}

	TryingAttemptsToFrameRead = 0;

	if(IterpBufferIndxRd == IterpBufferIndxEnd)
		{
			IterpBufferIndxRd = 0;
			IterpBufferIndx = 0;
			IterpBuffNotEmpty = false;
			return crqUNoneFrame;
		}


	for(uint8_t j = IterpBufferIndxRd;j < IterpBufferIndxEnd;j++)
		{
			if(!IsLMXFrameBegin && !IsCPMFrameBegin)
				{
					if(IterpBuffer[IterpBufferIndxRd] == 0x02)
						{
							IsLMXFrameBegin = true;
						}
					if(IterpBuffer[IterpBufferIndxRd] == 0x55)
						{
							IsCPMFrameBegin = true;
						}
				}
			else
				{
					if(IsLMXFrameBegin)
						{
							uint8_t UartDataByte =TInterpreter::IterpBuffer[IterpBufferIndxRd];

							if(FrameReadsCnt == 2) FrameDataCnt = UartDataByte + 4;

							xBuffer[FrameBufferIndx++] = UartDataByte;
							FrameReadsCnt++;
							FrameDataCnt--;

							if(FrameDataCnt == 0)
								{
									IsLMXFrameBegin = false;
									if(UartDataByte == 0x03)
										{
											IterpBufferIndxRd++;
											FrameReadsCnt = 0;
											FrameDataCnt = 0xFF;
											gDataLenght = FrameBufferIndx;
											FrameBufferIndx = 0;
											return crqUARTFrame;
										}
								}
						}
					else
						if(IsCPMFrameBegin)
							{
								uint8_t UartDataByte =TInterpreter::IterpBuffer[IterpBufferIndxRd];

								if(FrameReadsCnt == 1) FrameDataCnt = UartDataByte + 3;

								xBuffer[FrameBufferIndx++] = UartDataByte;
								FrameReadsCnt++;
								FrameDataCnt--;

								if(FrameDataCnt == 0)
									{
										IsCPMFrameBegin = false;
										if(UartDataByte == 0xAA)
											{
												IterpBufferIndxRd++;
												FrameReadsCnt = 0;
												FrameDataCnt = 0xFF;
												gDataLenght = FrameBufferIndx;
												FrameBufferIndx = 0;
												return crqUCPMFrame;
											}
									}
							}
				}
			IterpBufferIndxRd++;
		}

	return crqUNoneFrame;
}


void TInterpreter::ControllerInteraction()
{
    uint8_t CmtType = TInterpreter::CommandRequest();

   	if(CmtType == crqUARTFrame)
 		{
			uint8_t _type;
 			uint8_t Packet = ParseUARTCommand(&_type);
 			ParsingFunc(Packet,_type,gData);
 		}

	 	else if(CmtType == crqUCPMFrame)
		{
 			uint8_t _datacnt;
 			uint8_t Packet = ParseUSysCommand(&_datacnt);
 			ParsingFunc(Packet,_datacnt,gData);
		}
}


void TInterpreter::ParsingFunc(BYTE pckt,BYTE _datalen,BYTE *_gdata)
{
	switch(pckt)
	{

    case CMD_NULL_CMD:
      #ifdef LCD_TRACE
				TLcdTrace::AddLine("NULL COMMAND");
			#endif
      TInterpreter::SendFrameToHost(RPL_NULL_CMD);
			break;

    case CMD_GET_RTC:
			#ifdef LCD_TRACE
				TLcdTrace::AddLine("GET RTC COMMAND");
			#endif

      break;

		case CMD_SET_RTC:
			#ifdef LCD_TRACE
				TLcdTrace::AddLine("SET RTC COMMAND");
			#endif

      break;

  case CMD_SET_POWERSTATUS:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("SET POWERSTATUS COMMAND");
      #endif

      break;

   case CMD_START_MONITORING:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("START MONITORING COMMAND");
      #endif

      break;

   case CMD_STOP_MONITORING:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("STOP MONITORING COMMAND");
      #endif

      break;

   case CMD_MONITORINGDATA:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("MONITORING DATA");
      #endif

      break;

   case CMD_SET_LEDMODE:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("SET LED MODE");
      #endif

      break;

   case CMD_SET_SLEEP_MODE:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("SET SLEEP MODE");
      #endif

      break;

   case CMD_BOOTLOAD:

      #ifdef LCD_TRACE
         TLcdTrace::AddLine("BOOTLOAD COMMAND");
      #endif

      break;

   case RPL_NULL_CMD:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("RPL NULL COMMAND");
      #endif
      Fx_Online = true;
      break;

   case RPL_GET_RTC:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("RPL GET RTC");
      #endif

      {
        uint32_t _Clk = ((gData[3] << 24) + (gData[2] << 16) + (gData[1] << 8) + gData[0]);

        int y,m,d,hr,mi,se;

        TRtc::RTC_TimeStampToTDMY(_Clk,&y,&m,&d,&hr,&mi,&se);

        TRtc::year = y;

        TRtc::month = m;

        TRtc::day = d;

        TRtc::hours = hr;

        TRtc::minutes = mi;

        TRtc::seconds = se;

        /*
  		  TRtc::RTC_TimeAdjust(_Clk);

        tempvar = TRtc::RTC_TimeRead();
        */

      }

      break;

   case RPL_START_MONITORING:
      {

      }
      break;

   case RPL_STOP_MONITORING:
      {
      }

      break;

   case RPL_GETFLASHCRC:
      {
        Fx_CRC = ((gData[3] << 24) + (gData[2] << 16) + (gData[1] << 8) + gData[0]);
      }

      break;

   case RPL_RST:
      {
      }

      break;

   default:
      #ifdef LCD_TRACE
				TLcdTrace::AddLine("UNDEFINED COMMAND");
			#endif
      TInterpreter::SendFrameToHost(RPL_UNDEF_CMD);
			break;
  }
}

void TInterpreter::MonitorDataParseFunc(int index)
{
  TEcgProcessor::EcgData.Status.ChannelBytes[3] = 0;
	TEcgProcessor::EcgData.Status.ChannelBytes[2] = 0;

  TEcgProcessor::EcgData.Status.ChannelBytes[1] = 0;
	TEcgProcessor::EcgData.Status.ChannelBytes[0] = TInterpreter::IterpBuffer[index + 5];

	if (TAppProcessor::cable_state == ct10Leads)
	{
		     TEcgProcessor::EcgData.Channel[2].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 6];
		     TEcgProcessor::EcgData.Channel[2].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 7];
		     TEcgProcessor::EcgData.Channel[2].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 8];

		     TEcgProcessor::EcgData.Channel[0].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 9];
		     TEcgProcessor::EcgData.Channel[0].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 10];
		     TEcgProcessor::EcgData.Channel[0].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 11];

		     TEcgProcessor::EcgData.Channel[1].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 12];
		     TEcgProcessor::EcgData.Channel[1].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 13];
	       TEcgProcessor::EcgData.Channel[1].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 14];

		     TEcgProcessor::EcgData.Channel[6].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 15];
		     TEcgProcessor::EcgData.Channel[6].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 16];
		     TEcgProcessor::EcgData.Channel[6].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 17];

		     TEcgProcessor::EcgData.Channel[7].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 18];
		     TEcgProcessor::EcgData.Channel[7].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 19];
		     TEcgProcessor::EcgData.Channel[7].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 20];

		     TEcgProcessor::EcgData.Channel[5].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 21];
		     TEcgProcessor::EcgData.Channel[5].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 22];
		     TEcgProcessor::EcgData.Channel[5].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 23];

		     TEcgProcessor::EcgData.Channel[4].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 24];
		     TEcgProcessor::EcgData.Channel[4].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 25];
		     TEcgProcessor::EcgData.Channel[4].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 26];

		     TEcgProcessor::EcgData.Channel[3].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 27];
		     TEcgProcessor::EcgData.Channel[3].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 28];
		     TEcgProcessor::EcgData.Channel[3].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 29];
	     }
       else
       {
		     TEcgProcessor::EcgData.Channel[0].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 6];
		     TEcgProcessor::EcgData.Channel[0].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 7];
		     TEcgProcessor::EcgData.Channel[0].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 8];

		     TEcgProcessor::EcgData.Channel[2].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 9];
		     TEcgProcessor::EcgData.Channel[2].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 10];
		     TEcgProcessor::EcgData.Channel[2].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 11];

		     TEcgProcessor::EcgData.Channel[1].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 12];
		     TEcgProcessor::EcgData.Channel[1].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 13];
		     TEcgProcessor::EcgData.Channel[1].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 14];

		     TEcgProcessor::EcgData.Channel[3].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 15];
		     TEcgProcessor::EcgData.Channel[3].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 16];
		     TEcgProcessor::EcgData.Channel[3].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 17];

		     TEcgProcessor::EcgData.Channel[4].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 18];
		     TEcgProcessor::EcgData.Channel[4].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 19];
		     TEcgProcessor::EcgData.Channel[4].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 20];

		     TEcgProcessor::EcgData.Channel[5].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 21];
		     TEcgProcessor::EcgData.Channel[5].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 22];
		     TEcgProcessor::EcgData.Channel[5].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 23];

		     TEcgProcessor::EcgData.Channel[6].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 24];
		     TEcgProcessor::EcgData.Channel[6].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 25];
		     TEcgProcessor::EcgData.Channel[6].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 26];

		     TEcgProcessor::EcgData.Channel[7].ChannelBytes[3] =TInterpreter::IterpBuffer[index + 27];
		     TEcgProcessor::EcgData.Channel[7].ChannelBytes[2] =TInterpreter::IterpBuffer[index + 28];
		     TEcgProcessor::EcgData.Channel[7].ChannelBytes[1] =TInterpreter::IterpBuffer[index + 29];
	    }

	TRespProcessor::RespData.Status.ChannelBytes[3] = 0;
	TRespProcessor::RespData.Status.ChannelBytes[2] = 0;
	TRespProcessor::RespData.Status.ChannelBytes[1] = 0;
	TRespProcessor::RespData.Status.ChannelBytes[0] = 0;

	TRespProcessor::RespData.Channel_1.ChannelBytes[3] =TInterpreter::IterpBuffer[index + 30];
	TRespProcessor::RespData.Channel_1.ChannelBytes[2] =TInterpreter::IterpBuffer[index + 31];
	TRespProcessor::RespData.Channel_1.ChannelBytes[1] =TInterpreter::IterpBuffer[index + 32];

	int16_t tmp =TInterpreter::IterpBuffer[index+3] << 8;
	tmp += TInterpreter::IterpBuffer[index+4];

	uint8_t ADCi = (TInterpreter::IterpBuffer[index+5] & 0xF0) >> 4;
	if(ADCi == 6)
    TAppProcessor::SysVoltageValue = tmp;
	else if(ADCi == 5)
    TAppProcessor::AnalogVoltageValue = tmp;
	else if(ADCi == 4)
    TAppProcessor::LiVoltageValue = tmp;
	else if(ADCi == 3)
    TAppProcessor::BatVoltageValue = tmp;
	else if(ADCi == 2)
		TAppProcessor::AX_CoordWr[ADCi] = ((tmp - 2048) * 100 / ADXL_WR_DIVIDE);
	else if(ADCi == 1)
		TAppProcessor::AX_CoordWr[ADCi] = -((tmp - 2048) * 100 / ADXL_WR_DIVIDE);
	else if(ADCi == 0)
		TAppProcessor::AX_CoordWr[ADCi] = -((tmp - 2048) * 100 / ADXL_WR_DIVIDE);
	else {};

  for(uint8_t i = 0; i < 8; i++)
  {
    TEcgProcessor::EcgData.Channel[i].ChannelData = TEcgProcessor::EcgData.Channel[i].ChannelData >> 8;		/* сдвигаем на 8 и делаем 24 битные данные */
  }
	TRespProcessor::RespData.Channel_1.ChannelData = TRespProcessor::RespData.Channel_1.ChannelData >> 8;		/* сдвигаем на 8 и делаем 24 битные данные */
	TRespProcessor::RespData.Channel_2.ChannelData = TRespProcessor::RespData.Channel_2.ChannelData >> 8;

	TEcgProcessor::ChannelsProcessing();									/* обработка данных каналов экг */
	TRespProcessor::ChannelsProcessing();									/* обработка данных каналов рео */

	TEcgProcessor::EcgData.Channel[8].ChannelData = TRespProcessor::RespData.Channel_2.ChannelData;

/*
	if((TSDProcessor::SDPresent == true) && (TSDProcessor::SDStartWrPool == true))
		TSDProcessor::FillBuffer_ECG( &TEcgProcessor::EcgData );			// заполн€ем буффер Ё √ данными дл€ записи на карту 
*/

	if( TFrame::StartMonitioring == true )									/* если пришЄл флаг запуска измерений по BT */
		TFrame::AddSampleToMonitoringDataBuffer( &TEcgProcessor::EcgData );	/* заполн€ет буффер дл€ посылки по BT */

	if(TGuiObjects::EcgOnScreen)
		TGuiObjects::AddECGArrayToScreen( &TEcgProcessor::EcgData );
}


uint8_t TInterpreter::ParseUARTCommand(uint8_t *Type)
{
	uint8_t retByte = xBuffer[0];

	*Type = xBuffer[1];

  gData = &xBuffer[2];

	return retByte;
}
//------------------------------------------------------------------------------------

uint8_t TInterpreter::ParseUSysCommand(uint8_t *DataCount)
{
	uint8_t retByte = xBuffer[0];

	*DataCount = xBuffer[1];
//	for(uint8_t i = 0;i < *DataCount;i++) Data[i] = xBuffer[i + 2];
	gData = &xBuffer[2];

	return retByte;
}
//------------------------------------------------------------------------------------

bool TInterpreter::SendFrameToHost(BYTE FrameID, BYTE BytesCnt,const BYTE *Data)
{
	if(!Uart_Connected) return false;

	CharInt crcBCC;

	crcBCC.Int = 0;

	SendByte(0x55);

	SendByte(FrameID);

	SendByte(BytesCnt);

	for(BYTE i = 0;i < BytesCnt;i++)
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

bool TInterpreter::SendFrameToHost(BYTE FrameID)
{
	if(!Uart_Connected) return false;

	CharInt crcBCC;

	crcBCC.Int = 0;

  #ifdef LCD_TRACE
     TLcdTrace::AddLine("Send Frame To Host:");
  #endif

  SendByte(0x55);

	SendByte(FrameID);

	SendByte(0);

	crcBCC.Int += FrameID;

	SendByte(crcBCC.Char[0]);

	SendByte(0xAA);

  return true;
}
//------------------------------------------------------------------------------------