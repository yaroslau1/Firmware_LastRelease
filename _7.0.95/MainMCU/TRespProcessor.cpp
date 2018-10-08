/************************************************************************************/
/* @title																			*/
/* @author		Aleksis														*/
/* @version		0.1																	*/
/* @date		12.11.2012															*/
/************************************************************************************/
/* @brief																			*/
/************************************************************************************/
#include "TRespProcessor.h"
#include "TFrame.h"
#include "TDevice.h"
#include "TAppProcessor.h"
/*----------------------------------------------------------------------------------*/
RespData_t TRespProcessor::RespData;
RespData_t TRespProcessor::WrRespData;
Respiration_TypeDef TRespProcessor::Respiration;

bool TRespProcessor::RR_Led_Enable = false;

/*----------------------------------------------------------------------------------*/
/* @brief	ЦОС ЭКГ данных															*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void TRespProcessor::ChannelsProcessing( void )
{
	RespData.Channel_1.ChannelData = ( RespData.Channel_1.ChannelData * 100 ) / ADS1292_NORMALIZE_DIVIDER;	// нормализуем сигнал по ГОСТ 
//	RespData.Channel_1.ChannelData = RespData.Channel_1.ChannelData * 10000;	// нормализуем сигнал по ГОСТ 
	RespData.Channel_2.ChannelData = RespData.Channel_1.ChannelData;
//	RespData.Channel_2.ChannelData = ( RespData.Channel_2.ChannelData * 100 ) / ADS1292_NORMALIZE_DIVIDER;	// нормализуем сигнал по ГОСТ 
	if( TFrame::StartMonitioring == true )
	{
		RespData.Channel_1.ChannelData = FHigh_0_32_Fixed_Resp( RespData.Channel_1.ChannelData, false );	// активировать фильтр при тестах 
//		RespData.Channel_2.ChannelData = FHigh_0_32_Fixed( RespData.Channel_2.ChannelData, 9, false, true );	// активировать фильтр при тестах 
		RespData.Channel_2.ChannelData = FF_LP_12( RespData.Channel_1.ChannelData );
//RespData.Channel_2.ChannelData = RFF_LP_12( RespData.Channel_1.ChannelData );
	}
	else
	{
		RespData.Channel_1.ChannelData = FHigh_0_32_Fixed_Resp( RespData.Channel_1.ChannelData, false );
//		RespData.Channel_2.ChannelData = FHigh_0_32_Fixed( RespData.Channel_2.ChannelData, 9, false, false );
		RespData.Channel_2.ChannelData = RFF_LP_12( RespData.Channel_1.ChannelData );
	}

//	RespirationRateCalculation();

//	RespData.Channel_1.ChannelData = FF_Butterworth_HF8( RespData.Channel_1.ChannelData );				// усиливаем ВЧ область Fc = 50Гц, чтобы поднять АЧХ 
//	RespData.Channel_2.ChannelData = FF_Butterworth_HF9( RespData.Channel_2.ChannelData );				// усиливаем ВЧ область Fc = 50Гц, чтобы поднять АЧХ 

//	RespData.Channel_1.ChannelData *= -1;
//	RespData.Channel_2.ChannelData *= -1;
		
}


void TRespProcessor::WrChannelsProcessing( void )
{
/*
	WrRespData.Channel_1.ChannelData = ( WrRespData.Channel_1.ChannelData * 100 ) / ADS1292_NORMALIZE_DIVIDER;	// нормализуем сигнал по ГОСТ 
	WrRespData.Channel_2.ChannelData = RespData.Channel_1.ChannelData;
	if( TFrame::StartMonitioring == true )
	{
		WrRespData.Channel_1.ChannelData = FHigh_0_32_Fixed_Resp( WrRespData.Channel_1.ChannelData, false, true );	// активировать фильтр при тестах 
		WrRespData.Channel_2.ChannelData = FF_LP_12( WrRespData.Channel_1.ChannelData );
	}
	else
	{
		WrRespData.Channel_1.ChannelData = FHigh_0_32_Fixed_Resp( WrRespData.Channel_1.ChannelData, false, false );
		WrRespData.Channel_2.ChannelData = RFF_LP_12( WrRespData.Channel_1.ChannelData );
	}
*/
	TRespProcessor::WrRespData.Channel_1.ChannelData = ( TRespProcessor::WrRespData.Channel_1.ChannelData * 100 ) / ADS1292_NORMALIZE_DIVIDER;	// нормализуем сигнал по ГОСТ 
	TRespProcessor::WrRespData.Channel_1.ChannelData = FHigh_0_32_Fixed_Resp( TRespProcessor::WrRespData.Channel_1.ChannelData, false );
	TRespProcessor::WrRespData.Channel_1.ChannelData = FF_LP_12( TRespProcessor::WrRespData.Channel_1.ChannelData );
}



/*----------------------------------------------------------------------------------*/
/* @brief	ЧСС ЭКГ Детектор														*/
/* @param	wData - входные данные													*/
/* @return	отфильтрованные данные													*/
/*----------------------------------------------------------------------------------*/
int TRespProcessor::respirationrate_detector( int16_t wData )
{
	static int32_t rX_i_1;
	static int32_t rY_i;
	static int16_t rwData = 0;

	rY_i = (1500 * (rY_i + wData - rX_i_1));

	if(rY_i < 0)
	{
		rY_i = labs(rY_i);
		rY_i >>= 11;
		rY_i = -rY_i;
	}
	else rY_i >>= 11;

	rX_i_1 = wData;

	/* ограничитель */
	if(rY_i >  32766) rwData = 32766;
	else if(rY_i < -32766) rwData = -32766;
	else rwData = rY_i;
	/* ENDBLOCK */

	return (rwData*2);
}



/*----------------------------------------------------------------------------------*/
/* @brief	Функция измерения ЧСС													*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void TRespProcessor::RespirationRateCalculation( void )
{
	static int16_t data;
	static int16_t rOnStatus = 0;
	static int16_t rOffStatus = 0;
	static int16_t rMaxVal = 0;
	static uint16_t rOnOffPrd = 0;
	static bool StartMeasurment;

//	if( Hartbeat.Status == false )
//		return;

	data = respirationrate_detector( (int16_t)RespData.Channel_1.ChannelData );

	if( data > rMaxVal ){
		rMaxVal = data;
	}

	int16_t Threshold;

	static uint8_t OffStat = 0;

	Threshold = (int16_t)(((int16_t)rMaxVal * 1) / 2);

	if( data > Threshold ){ 												/* засекли высокий уровень (пик) */
		OffStat = 0;
//!!!    if(RR_Led_Enable) TDevice::LedOn();
//		LedOn();
//		TDevice::DEVICE->DebugTools->Led.On();
		if( StartMeasurment == false ){										/* если таймер не запущен */
			Respiration.Value = (int)( TIM_GetCounter( TIM5 ));
			TRespProcessor::Respiration.Redraw = true;
			StartMeasurment = true;
			TIM_SetCounter( TIM5, 0 );										/* сброс счётчика */
		}

		rOnStatus++;
	}
	else {																	/* засекли низкий уровень */
		if( OffStat > 10 ){
			OffStat = 0;
			StartMeasurment = false;
      if(RR_Led_Enable) TDevice::LedOff();
//			LedOff();
//			TDevice::DEVICE->DebugTools->Led.Off();
		}
		OffStat++;
		rOffStatus++;
	}

	rOnOffPrd++;
	if( rOnOffPrd > 800 ){													/* сброс порога, пика давно нет */
		if((rOnStatus - rOffStatus) > 780)
			rMaxVal = 0;
		if((rOffStatus - rOnStatus) > 780)
			rMaxVal = 0;

		rOnOffPrd = 0;
		rOnStatus = 0;
		rOffStatus = 0;
	}
}



/*----------------------------------------------------------------------------------*/
void TRespProcessor::TimerInit( void )
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM5, ENABLE );

#ifdef CLK_48
	TIM_TimeBaseStructure.TIM_Prescaler = 2049;
#else
	TIM_TimeBaseStructure.TIM_Prescaler = 1024;
#endif /* CLK_48 */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit( TIM5, &TIM_TimeBaseStructure );

	TIM_Cmd( TIM5, ENABLE );		/* запускаем таймер для измерения периода между R зубцами */

	DBGMCU_Config( DBGMCU_TIM5_STOP, ENABLE );
}



/*----------------------------------------------------------------------------------*/
void TRespProcessor::TimerDeinit( void )
{
	TIM_DeInit( TIM5 );
}