#ifndef TECGPROCESSOR_H
#define TECGPROCESSOR_H
/*----------------------------------------------------------------------------------*/
#include "TADS1298.h"
#include "main.h"

/* данные от одного преобразования ADS1298 */
typedef union
{
	int ChannelData;
	uint8_t ChannelBytes[ 4 ];
} U_Ch;

typedef struct
{
	U_Ch Status;		/* первые 4 байта */

	U_Ch Channel[10];

//	U_Ch Channel_1;		/* 4 байта на канал */
//	U_Ch Channel_2;
//	U_Ch Channel_3;
//	U_Ch Channel_4;

//	U_Ch Channel_5;
//	U_Ch Channel_6;
//	U_Ch Channel_7;
//	U_Ch Channel_8;		/* последние 4 байта */
//	U_Ch Channel_9;		/* последние 4 байта */
//	U_Ch Channel_10;		/* последние 4 байта */
} EcgData_t;

typedef struct
{
	void( *Enable )( void );
	void( *Disable )( void );
	int Value;
	bool Redraw;
	bool HartIconReset;
	bool Status;
}Hartbeat_TypeDef;


/*----------------------------------------------------------------------------------*/
//#define PACE_DETECTOR_THRESHOLD (150)
//#define PACE_DETECTOR_BASELINE  (2048)
#define PACE_DETECTOR_THRESHOLD (130)
#define PACE_DETECTOR_BASELINE  (1784)
/*----------------------------------------------------------------------------------*/
#define WR_ZERO_ADDUCTION_SAMPLES 300
/*----------------------------------------------------------------------------------*/
class TEcgProcessor : public TADS1298
{
	public:
		static void ChannelsProcessing( void );
		static void WrChannelsProcessing( void );
//		static void DataAveraging( void );
//		static void HeartbeatCalculation( void );
		static void TimerInit( void );
		static void TimerDeinit( void );
    
    static void TASK_WrProcessing( void *pvParameters );
   	static xTaskHandle xhECGWrProcessing;


		static int hartbeat_detector( int16_t wData );

		static uint8_t BuffEcgData[4096];
		static EcgData_t EcgData;
		static EcgData_t WrEcgData;
		static Hartbeat_TypeDef Hartbeat;
    static int  StartWritingZeroAdduction;
    
    static bool HR_Led_Enable;
    
    static volatile bool PaceInFrontEnd;

		static uint32_t ram_addr;
};
#endif