/*
    Title:
    Author:   VN Asliuk(Cardian)
    Date:     20.07.2007 12:01
    Purpose:
    Software:
    Hardware:
    Note:
*/
//------------------------------------------------------------------------------------
#ifndef TFRAME_H
#define TFRAME_H
//------------------------------------------------------------------------------------
#include "TLMX9838.h"
#include "Utilities.h"
#include "ae_ext_cm.h"
//#include "TADS1298.h"
#include "TEcgProcessor.h"
//------------------------------------------------------------------------------------
#define XBUFFER_SIZE 					256
#define USART_BUFFER_SIZE 				256

#ifdef CH_8
#define MONITORINGSAMPLEDATABUFF_SIZE 	248		/* Bytes */
#else
#define MONITORINGSAMPLEDATABUFF_SIZE 	240		/* Bytes */    //!!! 255 !!!
#endif
//------------------------------------------------------------------------------------
enum _enCommandREQ {	crqNoneFrame,
						crqLMXFrame,
						crqCPMFrame
					};

/* принимаемые команды */
enum _enHostCommands {	CMD_NULL_COMMAND = 0,
                        CMD_UNIFIED_FRAME,
                        CMD_RESET,
                        CMD_GO_BLD

//						CMD_START_MONITORING,		/* начать отсылать данные экг по блютуз */
//					 	CMD_STOP_MONITORING

//						CMD_RESET,//2
//						CMD_TST_GO_BLD,
//						CMD_GOSLEEP
					};

/* передаваемые команды */
enum _enCPMReplies {	RPL_NULL,
						RPL_UNIFIED_CONFIRM,//1
						RPL_UNIFIED_FRAME,//2
						RPL_UNDEFINED_CMD,
						RPL_BLD,
						RPL_LINKED_MARK,
						RPL_GOSLEEP,
						RPL_RESET
					};

	static int Pila[10];


//------------------------------------------------------------------------------------
class TFrame : public TLMX9838
{
  public:
	static xTaskHandle xhCmdProcessor;
	static void TASK_CmdProcessor( void *pvParameters );

	static xTaskHandle xhECGDataSend;
	static void TASK_SendDataToHost( void *pvParameters );
	static void TASK_CheckISRFlag( void *pvParameters );
  static xTaskHandle xhBPDataSend;
	static void TASK_NIBPSendDataToHost( void *pvParameters );

	static bool SendDataFrameToHost( uint8_t smplIndx );
	static bool SendNIBPDataFrameToHost(uint8_t smplIndx );
	static bool SendFrameToHost( uint8_t FrameID, uint8_t BytesCnt, const uint8_t *Data );
	static bool SendFrameToHost( uint8_t FrameID, uint8_t BytesCnt, uint8_t Data );
	static bool SendFrameToHost( uint8_t FrameID );

	static bool LMXUpdateDeviceName;
	static bool LMXUpdateSecurity;
	static void ISR_USART_Rcv( void );

  static bool ResumeFromISR;

	static void AddSampleToMonitoringDataBuffer( EcgData_t *Data );
	static void SetState();

	static bool TurnOffAfterLinkDisconnected;

	static bool StartMonitioring;

  static bool StartUpBluetooth;



  protected:
	static uint8_t gDataLenght;
	static uint8_t *gData;
	static uint8_t CommandRequest();
	static uint8_t ParseSysCommand( uint8_t *DataCount );
	static uint8_t ParseLMXCommand( uint8_t *Type );

	static uint8_t xBuffer[ XBUFFER_SIZE ];

	static void ClearMonitoringDataBuffer();


  private:
	volatile static uint8_t UartBuffer[ USART_BUFFER_SIZE ];
	volatile static int  UartBufferIndx;
	volatile static bool UartBuffNotEmpty;

	static void ClearxBuffer();

	static int16_t SampleDataBuffer[ MONITORINGSAMPLEDATABUFF_SIZE ];
	static uint16_t NIBPSampleDataBuffer[ MONITORINGSAMPLEDATABUFF_SIZE ];
	static int32_t NIBP_Proc_SampleDataBuffer[ MONITORINGSAMPLEDATABUFF_SIZE ];
	static uint8_t inSmpDataIndx;
	static uint8_t outSmpDataIndx;
	static uint8_t inNIBPSmpDataIndx;
	static uint8_t outNIBPSmpDataIndx;
	static bool  LSTAT_Stored;
};

#endif

