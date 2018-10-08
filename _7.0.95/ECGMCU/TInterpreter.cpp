/************************************************************************************/
/* @title		  Interpreter.h     										*/
/* @author		Cyber Genius													*/
/* @version		0.1.1																  */
/* @date		  13.11.2015														*/
/************************************************************************************/

//------------------------------------------------------------------------------------

#include "main.h"

#include "TInterpreter.h"
#include "TLed.h"
#include "TRtc.h"
#include "TPowerManagement.h"
#include "TECG.h"


#ifdef LCD_TRACE
   #include "TLcdTrace.h"
#endif

#include "IAP.h"

//------------------------------------------------------------------------------------

bool TInterpreter::Fx_Online = false;	

bool TInterpreter::Uart_Connected = false;

volatile uint8_t TInterpreter::UartBuffer[ USART_BUFFER_SIZE ];
volatile uint8_t TInterpreter::UartBufferIndx = 0;
volatile bool UartBuffNotEmpty = false;

volatile bool TInterpreter::UartBuffNotEmpty = false;

uint8_t TInterpreter::xBuffer[ XBUFFER_SIZE ];
BYTE *TInterpreter::gData = NULL;
uint8_t TInterpreter::gDataLenght = 0;

int TInterpreter::TempData = 0;

uint8_t UartBufferIndxEnd = 0;

volatile bool TInterpreter::IsMonitoring = false;
volatile bool TInterpreter::IsEcgWrite = false;

extern int     BuffAB_Indx;


//------------------------------------------------------------------------------------

void TInterpreter::UartPHYInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  // RCC config
  UART_GPIO_RCC_ENABLE;
  UART_RCC_ENABLE;

  GPIO_InitStructure.GPIO_Pin = UART_TX_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
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

	/* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = UART_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );  
  
  USART_InitTypeDef USART_InitStructure;
  
  USART_InitStructure.USART_BaudRate = 256000;   //
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(INTER_PORT, &USART_InitStructure);
	
  USART_ITConfig(INTER_PORT, USART_IT_RXNE, ENABLE);
  
}
//------------------------------------------------------------------------------------

void TInterpreter::UartInit()
{
  UartPHYInit();
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

void TInterpreter::ISR_USART_Rcv(void)
{
	UartBuffer[UartBufferIndx] = (uint8_t)USART_ReceiveData(INTER_PORT);
	UartBufferIndx++;
	if(UartBufferIndx >= (USART_BUFFER_SIZE - 1)) UartBufferIndx = (USART_BUFFER_SIZE - 1);
	UartBuffNotEmpty = true;
}
//------------------------------------------------------------------------------------

uint8_t TInterpreter::CommandRequest()
{
	UartBufferIndxEnd = UartBufferIndx;
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
			return crqUNoneFrame;
		}

	TryingAttemptsToFrameRead = 0;

	if(UartBufferIndxRd == UartBufferIndxEnd)
		{
			UartBufferIndxRd = 0;
			UartBufferIndx = 0;
			UartBuffNotEmpty = false;
			return crqUNoneFrame;
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
											return crqUARTFrame;
										}
								}
						}
					else
						if(IsCPMFrameBegin)
							{
								uint8_t UartDataByte = UartBuffer[UartBufferIndxRd];

								if(FrameReadsCnt == 1)
                {
                   FrameDataCnt = UartDataByte + 3;

                }
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
												return crqUCPMFrame;
											}
									}
							}
				}
			UartBufferIndxRd++;
		}

	return crqUNoneFrame;

}

void TInterpreter::ControllerInteraction()
{
  uint8_t CmtType = CommandRequest();
    
  if(CmtType == crqUNoneFrame)
 		{

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
      {
      #ifdef LCD_TRACE
				TLcdTrace::AddLine("NULL COMMAND");
			#endif
        TInterpreter::SendFrameToHost(RPL_NULL_CMD);
      }

			break;
      
		case CMD_GET_RTC:
			#ifdef LCD_TRACE
				TLcdTrace::AddLine("GET RTC COMMAND");
			#endif

/*       
        int y,m,d,hr,mi,se;
			  
        TRtc::RTC_TimeStampToTDMY(_Clk,&y,&m,&d,&hr,&mi,&se);
      */
      {
        uint32_t _Clk = TRtc::RTC_TimeRead();
        
        uint32_t Frame_Crc = 0;

        /*
        BYTE EnRtc[4];

        EnRtc[3] = ((_Clk & 0xFF000000) >> 24);
        EnRtc[2] = ((_Clk & 0x00FF0000) >> 16);
        EnRtc[1] = ((_Clk & 0x0000FF00) >> 8);
        EnRtc[0] = (_Clk & 0x000000FF);
       
        TInterpreter::SendFrameToHost(RPL_GET_RTC, 4, EnRtc);
        */
        
        BYTE send_data[9];
        
        send_data[0] = 0x55;
        
        send_data[1] = RPL_GET_RTC;
        
        send_data[2] = 4;
        
        send_data[3] = ((_Clk & 0xFF000000) >> 24);
        send_data[4] = ((_Clk & 0x00FF0000) >> 16);
        send_data[5] = ((_Clk & 0x0000FF00) >> 8);
        send_data[6] = (_Clk & 0x000000FF);
            
        for (BYTE i=1; i<7; i++)
        {
          Frame_Crc += send_data[i];
        }
        
        send_data[7] = Frame_Crc;
      
        send_data[8] = 0xAA;
        
        SendDMA_UART(9, send_data);
      }

      break;

		case CMD_SET_RTC:
			#ifdef LCD_TRACE
				TLcdTrace::AddLine("SET RTC COMMAND");
			#endif

      {
        uint32_t _Clk = ((gData[3] << 24) + (gData[2] << 16) + (gData[1] << 8) + gData[0]);

			  int y,m,d,hr,mi,se;
			  TRtc::RTC_TimeStampToTDMY(_Clk,&y,&m,&d,&hr,&mi,&se);

  		  TRtc::RTC_TimeAdjust(_Clk);
      }

      break;

  case CMD_SET_POWERSTATUS:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("SET POWERSTATUS COMMAND");
      #endif

      break;

  case CMD_START_ECGWRITE:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("START WRITE ECG COMMAND");
      #endif
      BuffAB_Indx = 0;
      TECG::SampleCounter = 0;
      SetIsEcgWrite(true);
      SendFrameToHost(RPL_START_ECGWRITE);
      break;

  case CMD_STOP_ECGWRITE:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("STOP WRITE ECG COMMAND");
      #endif
      SetIsEcgWrite(false);
      SendFrameToHost(RPL_STOP_ECGWRITE);
      break;

  case CMD_START_MONITORING:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("START MONITORING COMMAND");
      #endif
      SetIsMonitoring(true);
      //SendFrameToHost(RPL_START_MONITORING);
      break;

  case CMD_STOP_MONITORING:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("STOP MONITORING COMMAND");
      #endif
      SetIsMonitoring(false);
      SendFrameToHost(RPL_STOP_MONITORING);
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

      TLed::LedMode = gData[0];

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
      /* Allow access to BKP Domain */
      PWR_BackupAccessCmd(ENABLE);
      RTC_WriteBackupRegister(RTC_BKP_DR2,BOOTLOADER_IN_MARKER);
      SendFrameToHost(RPL_BOOTLOAD);
      NVIC_SystemReset();
  
      break;
      
   case CMD_GETFLASHCRC:       
        
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("GETFLASHCRC COMMAND");
      #endif
      CharLong crc;
      crc.Long = __checksum;
      SendFrameToHost(RPL_GETFLASHCRC, 4, &crc.Char[0]);
  
      break;

   case CMD_RST:       
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("RESET COMMAND");
      #endif
      SendFrameToHost(RPL_RST);
      NVIC_SystemReset();
      break;
      
   case CMD_LIBAT_OFF:       
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("LI BAT OFF COMMAND");
      #endif
      SendFrameToHost(RPL_LIBAT_OFF);
      TPowerManagement::LiMode = LI_MODE_OFF;
      TPowerManagement::FastCharge_Enable();
      TPowerManagement::Li_Disable();
      break;
      
   case CMD_RESP_ON:       
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("RESP ON COMMAND");
      #endif
      SendFrameToHost(RPL_RESP_ON);
      TECG::SetResp( true );
      break;
      
   case CMD_RESP_OFF:       
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("RESP OFF COMMAND");
      #endif
      SendFrameToHost(RPL_RESP_OFF);
      TECG::SetResp( false );
      break;
      
  case RPL_NULL_CMD:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("RPL NULL COMMAND");
      #endif

      break;

  case RPL_GET_RTC:
      #ifdef LCD_TRACE
         TLcdTrace::AddLine("RPL GET RTC");
      #endif
    
      break;   
   
   default:
      #ifdef LCD_TRACE
				TLcdTrace::AddLine("UNDEFINED COMMAND");
			#endif
      //TInterpreter::SendFrameToHost(RPL_UNDEF_CMD);
        TInterpreter::SendDMA_UART(RPL_UNDEF_CMD);
			break;
  }
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

bool TInterpreter::SendDMA_UART(BYTE FrameID)
{
  if (!Uart_Connected) return false;
  
  uint8_t send_data[FrameIDSize];
  
  send_data[0] = 0x55;
  
  send_data[1] = FrameID;
  
  send_data[2] = 0x00;
  
  send_data[3] = FrameID;
    
  send_data[4] = 0xAA;

  UART_DMA_ENABLE;
  
  /* Private typedef -----------------------------------------------------------*/
  DMA_InitTypeDef  DMA_InitStructure;
      
  // DMA channel Tx of USART Configuration 
 
  DMA_DeInit(UART_TX_DMA_CHANNEL);
  
  // DMA Configuration -------------------------------------------------------
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_MemoryBaseAddr =  (uint32_t)send_data;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)UART_TDR_ADDRESS;
  DMA_InitStructure.DMA_BufferSize = (uint16_t)FrameIDSize;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  
  DMA_Init(UART_TX_DMA_CHANNEL, &DMA_InitStructure);
  
  // Enable the USART DMA requests 
  //USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
  
  /* Enable the USART1 Rx and Tx DMA2 requests */
  USART_DMACmd(USART1, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);
  
  // Clear the TC bit in the SR register by writing 0 to it 
  //USART_ClearFlag(USART1, USART_FLAG_TC);
  
  // Enable the DMA USART Tx Channel 
  DMA_Cmd(UART_TX_DMA_CHANNEL, ENABLE);
  
  TurnOnUart();
    
  while (!DMA_GetFlagStatus(UART_TX_DMA_FLAG_TC));
  
  
  DMA_ClearFlag(UART_TX_DMA_FLAG_TC);
    
  while (!USART_GetFlagStatus(USART1, USART_FLAG_TC))
  {
  }
     
  DMA_Cmd(UART_TX_DMA_CHANNEL, DISABLE);
  
  // Clear DMA global flags 
  //DMA_ClearFlag(UART_TX_DMA_FLAG_GL);
    
  // Disable the USART Tx DMA requests 
  //USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
  return true;
}

bool TInterpreter::SendDMA_UART(BYTE BytesCnt,const BYTE *Data)
{
  if (!Uart_Connected) return false;
  
  UART_DMA_ENABLE;
  
  /* Private typedef -----------------------------------------------------------*/
  DMA_InitTypeDef  DMA_InitStructure;
      
  // DMA channel Tx of USART Configuration 
 
  DMA_DeInit(UART_TX_DMA_CHANNEL);
  
  // DMA Configuration -------------------------------------------------------
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_MemoryBaseAddr =  (uint32_t)Data;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)UART_TDR_ADDRESS;
  DMA_InitStructure.DMA_BufferSize = (uint16_t)BytesCnt;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  
  DMA_Init(UART_TX_DMA_CHANNEL, &DMA_InitStructure);
  
  // Enable the USART DMA requests 
  //USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
  
  /* Enable the USART1 Rx and Tx DMA2 requests */
  USART_DMACmd(USART1, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);
  
  // Clear the TC bit in the SR register by writing 0 to it 
  //USART_ClearFlag(USART1, USART_FLAG_TC);
  
  // Enable the DMA USART Tx Channel 
  DMA_Cmd(UART_TX_DMA_CHANNEL, ENABLE);
  
  TurnOnUart();
    
  while (!DMA_GetFlagStatus(UART_TX_DMA_FLAG_TC));
  
  
  DMA_ClearFlag(UART_TX_DMA_FLAG_TC);
    
  while (!USART_GetFlagStatus(USART1, USART_FLAG_TC))
  {
  }
     
  DMA_Cmd(UART_TX_DMA_CHANNEL, DISABLE);
  
  // Clear DMA global flags 
  //DMA_ClearFlag(UART_TX_DMA_FLAG_GL);
    
  // Disable the USART Tx DMA requests 
  //USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
  
  return true;
}

void TInterpreter::SetIsEcgWrite(bool new_state)
{
  if (new_state != IsEcgWrite)
  {
    IsEcgWrite = new_state;
    if(!IsEcgWrite) TECG::SampleCounter = 0;
    if (IsMonitoring || IsEcgWrite) TECG::SetECGOn(true);
    else TECG::SetECGOn(false);
  }
}

void TInterpreter::SetIsMonitoring(bool new_state)
{
  if (new_state != IsMonitoring)
  {
    IsMonitoring = new_state;
    if (IsMonitoring || IsEcgWrite) TECG::SetECGOn(true);
    else TECG::SetECGOn(false);
  }
}
