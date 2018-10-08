#ifndef TAPPPROCESSOR_H
#define TAPPPROCESSOR_H
/*----------------------------------------------------------------------------------*/
#include "main.h"
#include "TFrame.h"
#include "semphr.h"

#define CARD_VERSION		2
#define DEVICE_VERSION	0
/*----------------------------------------------------------------------------------*/
#define    DWT_CYCCNT    *(volatile uint32_t *)0xE0001004
#define    DWT_CONTROL   *(volatile uint32_t *)0xE0001000
#define    SCB_DEMCR     *(volatile uint32_t *)0xE000EDFC
/*----------------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct
{
	char DeviceName[25];
	uint16_t Serial;
	uint32_t DateOfProduction;
	char AnPCBID[5];
	char DiPCBID[5];
} stufSerialNumber;		/* 41 byte */
#pragma pack()

typedef struct
{
	uint32_t Time;
	uint32_t Date;
} TTimeStampEV;

typedef union
{
	struct
	{
		uint32_t Value;
	} Single;
	struct
	{
		uint16_t  _PatientSeqNum;
		uint16_t  _DeviceSerialID;
	} Bits;
} stDeviceToken;

#pragma pack(1)			/* Выравниевание структуры */
typedef struct
{
	uint8_t PatientCardVer;			/* кардиостимулятор 1-нет, 2-есть */
	uint8_t DeviceVersion;			/* 0 - старый КР01, 1 - новый КР01 */
	uint8_t Name[50];
	uint8_t DateOfBirth[12];
	uint8_t Sex;
	uint8_t BP[10];
	uint8_t Doctor[50];
	uint8_t Diagnosis[50];
	uint8_t Comments[50];
	uint32_t RecordLenght;//4
	TTimeStampEV DateTimeOfStart;//8
	stDeviceToken Token;//4		/* определяет конец файла */
	uint8_t RecordOptionRequest;//1
	uint8_t RecordOptionAck;//1
	uint8_t ECGAdvancedOption;//1
	uint8_t NIBPAdvancedOption;//1
} stPatientCardV1;		/* 245 bytes */
#pragma pack()

typedef struct
{
	union
	{
		struct
		{
			WORD fmSetup:1,
				fmSerialNumber:1,
				fmPatientCard:1,
				fmDateTime:1,
				
				fmBTConfiguration:1,
				fmNotes:1,
				fmCalibrations:1,
				fmTestingSignals:1,
				
				fmClearLog:1,
				fmSetPatientAction:1,
				fmSetRecordLenght:1,
				fmSetTravelFile:1,
				
				fmVoice:1,
				fmAxel:1,
				fmResp:1,
				fmStartNow:1;
		} _bits;
		struct
		{
			WORD  value;
		} _word;
	} u;
} stUFrameHostSetsMap;

typedef struct
{
	union
	{
		struct
		{
			WORD fmrSetup:1,
				fmrSerialNumber:1,
				fmrPatientCard:1,
				fmrDateTime:1,
				
				fmrBTConfiguration:1,
				fmrNotes:1,
				fmrCalibrations:1,
				fmrTestingSignals:1,
				
				fmrInfo:1,
				fmrLog:1,
				fmrBatteries:1,
				fmrWrStarted:1,
				nu:4;
		} _bits;
		struct
		{
			WORD  value;
		} _word;
	} u;
} stUFramDeviceMap;

typedef struct
{
	union
	{
		struct
		{
			BYTE EP_Enable:1,
				UseHPF:1,
				UseBaselineKeeper:1,
				UseRWaveIndicator:1,
				UseMonitoring:1,
				UseWriteToCard:1,
				UseBtLink:1,
				UseDisableAutoTurnOff:1;
		} _bits;
		struct
		{
			BYTE  value;
		} _word;
	} u;
} stSetup;
/*----------------------------------------------------------------------------------*/

enum eCableType {
	ctNoCable,
	ct5Leads,
	ct7Leads,
	ct10Leads
};

/*----------------------------------------------------------------------------------*/
#define HOST_SETS_MAP_HIBYTE				0
#define HOST_SETS_MAP_LOBYTE  				1
#define DIVICE_REQUEST_MAP_HIBYTE			2
#define DIVICE_REQUEST_MAP_LOBYTE			3
#define UFRAME_BLOCKS_BEGIN   				4
/*----------------------------------------------------------------------------------*/
#define ADXL_WR_DIVIDE 600
/*----------------------------------------------------------------------------------*/
class TAppProcessor : public TFrame
{
	public:
		static void App_ParceEvents( uint8_t pckt, uint8_t _datalen, uint8_t *_gdata );
		static void UnifiedFrameRcvHandler( void );
		static void TASK_Loading( void *pvParameters );
                static void TASK_Init( void *pvParameters );
                static void TASK_StartUp( void *pvParameters );
  	  	
  	static void TASK_ScreenHandler( void *pvParameters );
  	static void TASK_MainProcessor( void *pvParameters );
  	static void TASK_KeyProcessor( void *pvParameters );
		static void TASK_LEDblink( void *pvParameters );

		static uint8_t ReadBlockIntoBuffer( uint8_t *Buffer, uint8_t Indx );
		static uint8_t UploadBlockIntoBuffer( uint8_t *Buffer, uint8_t Indx, uint8_t DataCount );
    
    static void RCC_Configuration_Main(void);

    
    
    static int  CableCheck();     
    static void ChangeCableIcon(int NewState);
    static uint8_t ecg_format;
		
		static stPatientCardV1 PatientCard;
		static stufSerialNumber ufSerialNumber;
		static stUFramDeviceMap UFrameReqDeviceMap;
		static stSetup Setup;						/* Настройка из старого холтера */

		static TTimeStampEV DeviceDateTime;
		
		static uint8_t BatVoltageLevel;			/* Уровень заряда батареи */
		static uint16_t BatVoltageValue;			/* Напряжение батареи */
		static uint16_t LiVoltageValue;			 /* Напряжение резервной батареи */
		static uint16_t AnalogVoltageValue;			/* Напряжение аналогового питания */
		static uint16_t SysVoltageValue;			/* Напряжение Vsys */

		static xTaskHandle xKeyProcessor;
		static xTaskHandle xScreenHandler;
		static xTaskHandle xLEDblink;
    
    static bool StartRecording;
    static bool AudioRec;
    
    static int AX_X_Coord;
    static int AX_Y_Coord;
    static int AX_Z_Coord;
    
    static volatile uint8_t AX_CoordWr[3];
    
    static int cable_state;
    
    static volatile bool audio_rec_start;
    
    
    
    static bool StartECGRecording;
    static bool RecordInProgress;
    
    static bool ExitFromRecordMode;
    static bool ErrorInRecordMode;
    static bool ECGRecordingStarted;
    
    static bool StartNow;

    
};
#endif