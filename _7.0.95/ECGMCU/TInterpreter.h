/************************************************************************************/
/* @title		  Interpreter.h     										*/
/* @author		Cyber Genius													*/
/* @version		0.1.1																  */
/* @date		  13.11.2015														*/
/************************************************************************************/

//------------------------------------------------------------------------------------

#ifndef TINTERP_H
#define TINTERP_H

#include "TInterpreterHardware.h"
//------------------------------------------------------------------------------------

//#define STX_BT 0x02
//#define ETX_BT 0x03
//#define CFM_BT 0x03
//------------------------------------------------------------------------------------

#define XBUFFER_SIZE              256
#define USART_BUFFER_SIZE 				256

#define FrameIDSize               5

enum _enInterpCommandREQ {	crqUNoneFrame,
						crqUARTFrame,
						crqUCPMFrame
					};

enum _enInterpCommands {CMD_NULL_CMD = 0x00,
                        CMD_GET_RTC,     
                        CMD_SET_RTC,       
                        CMD_START_ECGWRITE,
                        CMD_STOP_ECGWRITE,
                        CMD_SET_POWERSTATUS,
                        CMD_START_MONITORING,
                        CMD_STOP_MONITORING,
                        CMD_MONITORINGDATA,
                        CMD_SET_LEDMODE,
                        CMD_SET_SLEEP_MODE,
                        CMD_BOOTLOAD,
                        CMD_GETFLASHCRC,
                        CMD_RST,
                        CMD_LIBAT_OFF,
                        CMD_LIBAT_AUTO,
                        CMD_RESP_ON,
                        CMD_RESP_OFF,
                        
                        CMD_UNDEF_CMD = 0x7F
                        };

enum _enInterpReplies  {RPL_NULL_CMD = 0x80,
                        RPL_GET_RTC,     
                        RPL_SET_RTC,       
                        RPL_SET_POWERSTATUS,
                        RPL_START_ECGWRITE,
                        RPL_STOP_ECGWRITE,
                        RPL_START_MONITORING,
                        RPL_STOP_MONITORING,
                        RPL_MONITORINGDATA,
                        RPL_SET_LEDMODE,
                        RPL_SET_SLEEP_MODE,
                        RPL_BOOTLOAD,
                        RPL_GETFLASHCRC,
                        RPL_RST,
                        RPL_LIBAT_OFF,
                        RPL_LIBAT_AUTO,
                        RPL_RESP_ON,
                        RPL_RESP_OFF,

                        RPL_UNDEF_CMD = 0xFF
                        };

class TInterpreter
{
  public:
  	static void UartPHYInit();
    
    static void UartInit();
    
    static void TurnOnUart();

    static void TurnOffUart();
    
  	static void SendByte(BYTE data);
		static bool ReceiveByte(BYTE *bt);
    
    static void ISR_USART_Rcv(void);
    
    static bool SendFrameToHost(BYTE FrameID, BYTE BytesCnt,const BYTE *Data);
		static bool SendFrameToHost(BYTE FrameID);
    
    static bool SendDMA_UART(BYTE FrameID);
    static bool SendDMA_UART(BYTE BytesCnt,const BYTE *Data);

    static void ControllerInteraction();
    
    static void ParsingFunc(BYTE pckt,BYTE _datalen,BYTE *_gdata);
    
  	static inline void SetOnlineState() {Fx_Online = true;}

  	static bool Fx_Online;
    
    static bool Uart_Connected;

    static BYTE *gData;
    
    static uint8_t xBuffer[ XBUFFER_SIZE ];
    
    static int TempData;

    volatile static uint8_t UartBuffer[ USART_BUFFER_SIZE ];
	  volatile static uint8_t UartBufferIndx;

    volatile static bool UartBuffNotEmpty;

    volatile static bool IsMonitoring;
    volatile static bool IsEcgWrite;
    
    static void SetIsMonitoring(bool new_state);
    static void SetIsEcgWrite(bool new_state);

  protected:
    
    static uint8_t gDataLenght;
    
    static uint8_t CommandRequest();
    
    static uint8_t ParseUSysCommand( uint8_t *DataCount );
	  static uint8_t ParseUARTCommand( uint8_t *Type );
  	
  private:
    
};


//------------------------------------------------------------------------------------
#endif  //TINTERP_H
