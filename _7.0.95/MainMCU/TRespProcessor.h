#ifndef TRESPPROCESSOR_H
#define TRESPPROCESSOR_H
/*----------------------------------------------------------------------------------*/
#include "TADS1292.h"
#include "main.h"

/* данные от одного преобразования ADS1292 */
typedef union
{
	int ChannelData;
	uint8_t ChannelBytes[ 4 ];
} Resp_Ch;

typedef struct
{
	Resp_Ch Status;		/* первые 4 байта */

	Resp_Ch Channel_1;		/* 4 байта на канал */
	Resp_Ch Channel_2;		/* последние 4 байта */
} RespData_t;

typedef struct
{
	void( *Enable )( void );
	void( *Disable )( void );
	int Value;
	bool Redraw;
	bool HartIconReset;
	bool Status;
}Respiration_TypeDef;


class TRespProcessor : public TADS1292
{
	public:
		static void ChannelsProcessing( void );
		static void WrChannelsProcessing( void );
		static void RespirationRateCalculation( void );
		static void TimerInit( void );
		static void TimerDeinit( void );

		static int respirationrate_detector( int16_t wData );

		static RespData_t RespData;
		static RespData_t WrRespData;
		static Respiration_TypeDef Respiration;
    
		static bool RR_Led_Enable;
    
};
#endif