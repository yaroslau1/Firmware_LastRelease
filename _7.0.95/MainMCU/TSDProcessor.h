/* 	������ ����� ecg.trv
	����:
	0 - mode of travel (0x01 - ���� ecg.ecg ��� ������, 0x00 - ���� �� ��� ������)
	1..4 - token - 4 bytes
	5 - ����� ���������� ����� .ecg
*/
#ifndef TSDPROCESSOR_H
#define TSDPROCESSOR_H
/*----------------------------------------------------------------------------------*/
#include "diskio.h"
#include "ff.h"
#include "TEcgProcessor.h"
#include "main.h"
/*----------------------------------------------------------------------------------*/
#ifdef SD_SDIO_MODE
	#include "sd_sdio.h"
#elif defined SD_SPI_MODE
	#include "sd_spi.h"
#else
	#error "No mode for SD card!"
#endif
/*----------------------------------------------------------------------------------*/
enum ecgTrvFileErrors{ trverrGood, trverrNofile, trverrNotRead };
/*----------------------------------------------------------------------------------*/
class TSDProcessor
{
	public:
		static void FillBuffer_ECG(EcgData_t *Data);
		static void TASK_SDDataWrite( void *pvParameters );
		static void TASK_SDPresentCheck( void *pvParameters );
		static uint8_t StartWriting( void );
		static void StopWriting( void );
		static void write_patient_card( void );
		
		/* ������� ���� ������� ������ ��� */
		static int8_t EcgBuffer_1[ 2048 ];
		static int8_t EcgBuffer_2[ 2048 ];

		/* ��� ����� �������� ����� �� ������ */
		static bool EcgBuffer_1_Full;
		static bool EcgBuffer_2_Full;

		static FATFS FATFS_Obj;
		static FIL MainFile;
		static FIL TrvFile;
		static uint8_t TrMode;

		static xTaskHandle xSDPresentCheck;
		static xTaskHandle xSDDataWrite;

		static bool ReplaceECGFile;
		static bool NoAskReplaceECGFileQuestion;
		static bool Def;
		
		/* ����� ������� SD ����� */
		static bool SDPresent;		/* ���� ������� SD ����� */
		static bool SDWriteEnable;	/* ���� ���������� ������ �� ����� */
    static bool SDStartWrPool;

	private:

	protected:
};
/*----------------------------------------------------------------------------------*/
#endif