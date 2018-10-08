/************************************************************************************/
/* @title																			*/
/* @author		Cyber Genius														*/
/* @version		0.1																	*/
/* @date		12.11.2012															*/
/************************************************************************************/
/* @brief																			*/
/************************************************************************************/
#include "TEcgProcessor.h"
#include "TRespProcessor.h"
#include "TSDProcessor.h"
#include "TFrame.h"
#include "TDevice.h"
#include "TAppProcessor.h"
#include "TSpiSram.h"

/*----------------------------------------------------------------------------------*/
EcgData_t TEcgProcessor::EcgData;
EcgData_t TEcgProcessor::WrEcgData;
Hartbeat_TypeDef TEcgProcessor::Hartbeat;
xTaskHandle TEcgProcessor::xhECGWrProcessing;

uint8_t TEcgProcessor::BuffEcgData[4096];


bool TEcgProcessor::HR_Led_Enable = true;    //false
volatile bool TEcgProcessor::PaceInFrontEnd = false;
uint32_t TEcgProcessor::ram_addr = 0;
int  TEcgProcessor::StartWritingZeroAdduction = WR_ZERO_ADDUCTION_SAMPLES;


void TEcgProcessor::TASK_WrProcessing( void *pvParameters )
{

	while(1)
  {
//    static uint32_t TestPacketCount = 0;
		vTaskSuspend( NULL );
    TSPISRAM::IRQControl( 0 );
    ram_addr = 0;
//		for (int i = 0; i < SPISRAM_SIZE / 32; i++)

    TSPISRAM::ReadSeq( TEcgProcessor::ram_addr, 4096, &BuffEcgData[0] );
    TSPISRAM::IRQControl( 1 );

		for (int i = 0; i < 4096/32; i++)
    {
/*
      static bool TestFail = false;
			if(!TEcgProcessor::GetConvertResultTest())										// считывание данных экг с ADS1298 
      {
        TestFail = true;
      }
      TestPacketCount++;
*/      
			TEcgProcessor::GetConvertResult();										/* считывание данных экг с ADS1298 */
			TEcgProcessor::WrChannelsProcessing();									/* обработка данных каналов экг */
			TRespProcessor::WrChannelsProcessing();									/* обработка данных каналов экг */
      TEcgProcessor::WrEcgData.Channel[9].ChannelData = TRespProcessor::WrRespData.Channel_1.ChannelData;
//      TEcgProcessor::WrEcgData.Channel_9.ChannelData = 0xFF;
			TSDProcessor::FillBuffer_ECG( &TEcgProcessor::WrEcgData );			/* заполняем буффер ЭКГ данными для записи на карту */
    }
//    GPIO_ResetBits( GPIOA, GPIO_Pin_8 );

//    TSPISRAM::IRQControl( 1 );
  }
}

/*----------------------------------------------------------------------------------*/
/* @brief	ЦОС ЭКГ данных															*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void TEcgProcessor::ChannelsProcessing(void)
{
  uint8_t ch_cnt = 3;
  if (TAppProcessor::cable_state == ct10Leads)
	{
    ch_cnt = 8;
	}

  uint8_t ZeroAddu = 1;
	if( TFrame::StartMonitioring == true )
	{
    ZeroAddu = 1;
	}

  for(uint8_t i = 0; i < ch_cnt; i++)
  {
    EcgData.Channel[i].ChannelData = (EcgData.Channel[i].ChannelData * 100) / ADS1298_NORMALIZE_DIVIDER;   // нормализуем сигнал по ГОСТ
    EcgData.Channel[i].ChannelData = FHigh_0_32_Fixed(EcgData.Channel[i].ChannelData, i, false, ZeroAddu); // фильтр изолинии
    EcgData.Channel[i].ChannelData = FF_Butterworth_HF(EcgData.Channel[i].ChannelData, i);                 // усиливаем ВЧ область Fc = 50Гц, чтобы поднять АЧХ
    //EcgData.Channel[i].ChannelData = Butterworth_Ecg_HF(EcgData.Channel[i].ChannelData, i);
    EcgData.Channel[i].ChannelData *= -1;
  }

//	HeartbeatCalculation();
}


void TEcgProcessor::WrChannelsProcessing( void )
{
  uint8_t ch_cnt = 3;
  if (TAppProcessor::cable_state == ct10Leads)
	{
    ch_cnt = 8;
	}

  uint8_t ZeroAddu = 1;
  if(StartWritingZeroAdduction > 0) 
  {
    StartWritingZeroAdduction--;
    ZeroAddu = 2;
  }

  //WrEcgData.Channel[0].ChannelData = (WrEcgData.Channel[0].ChannelData * 100) / ADS1298_NORMALIZE_DIVIDER;       // нормализуем сигнал по ГОСТ
  
  //WrEcgData.Channel[0].ChannelData = FHigh_0_32_Fixed(WrEcgData.Channel[0].ChannelData, 8, false, ZeroAddu);     // фильтр изолинии
  
  //WrEcgData.Channel[0].ChannelData = FF_Butterworth_HF(WrEcgData.Channel[0].ChannelData, 8);
  
  //WrEcgData.Channel[0].ChannelData *= -1;
  
  for(uint8_t i = 0; i < ch_cnt; i++)
  {
    WrEcgData.Channel[i].ChannelData = (WrEcgData.Channel[i].ChannelData * 100) / ADS1298_NORMALIZE_DIVIDER;       // нормализуем сигнал по ГОСТ
    WrEcgData.Channel[i].ChannelData = Butterworth_Ecg_HF(WrEcgData.Channel[i].ChannelData, 8 + i);
    WrEcgData.Channel[i].ChannelData = FF_Butterworth_HF(WrEcgData.Channel[i].ChannelData, 8 + i);                 // усиливаем ВЧ область Fc = 50Гц, чтобы поднять АЧХ
    WrEcgData.Channel[i].ChannelData *= -1;
  }
}


/*
//------------------------------------------------------------------------------
// Функция для усредненя значений, усредняет 2 значения
// нужна для того чтобы привести значения к 250 Гц
//------------------------------------------------------------------------------
void TEcgProcessor::DataAveraging( void )
{
	static int NData,								// номер полученного значения
		tempCH1, tempCH2, tempCH3;					// временные значения каналов

#ifdef EV_BOARD
	if( NData == 0 ){								// первая интерация
		tempCH1 = EcgData.Channel_4.ChannelData;
		tempCH2 = EcgData.Channel_5.ChannelData;
		tempCH3 = EcgData.Channel_6.ChannelData;

		NData++;
		return;
	}
	if( NData == 1 ){		// вторая интерация
		EcgData.Channel_4.ChannelData = ( tempCH1 + EcgData.Channel_4.ChannelData ) / 2;
		EcgData.Channel_5.ChannelData = ( tempCH2 + EcgData.Channel_5.ChannelData ) / 2;
		EcgData.Channel_6.ChannelData = ( tempCH3 + EcgData.Channel_6.ChannelData ) / 2;

		NData = 0;
	}
#else
	if( NData == 0 ){		// первая интерация
		tempCH1 = EcgData.Channel_1.ChannelData;
		tempCH2 = EcgData.Channel_2.ChannelData;
		tempCH3 = EcgData.Channel_3.ChannelData;

		NData++;
		return;
	}
	if( NData == 1 ){		// вторая интерация
		EcgData.Channel_1.ChannelData = ( tempCH1 + EcgData.Channel_1.ChannelData ) / 2;
		EcgData.Channel_2.ChannelData = ( tempCH2 + EcgData.Channel_2.ChannelData ) / 2;
		EcgData.Channel_3.ChannelData = ( tempCH3 + EcgData.Channel_3.ChannelData ) / 2;

		NData = 0;
	}
#endif

	if( TFrame::StartMonitioring == true )									// если пришёл флаг запуска измерений по BT
		TFrame::AddSampleToMonitoringDataBuffer( &TEcgProcessor::EcgData );	// заполняет буффер для посылки по BT
}
*/


/*----------------------------------------------------------------------------------*/
/* @brief	ЧСС ЭКГ Детектор														*/
/* @param	wData - входные данные													*/
/* @return	отфильтрованные данные													*/
/*----------------------------------------------------------------------------------*/
int TEcgProcessor::hartbeat_detector( int16_t wData )
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


/*
//------------------------------------------------------------------------------
// @brief	Функция измерения ЧСС
// @param	None
// @return	None
//------------------------------------------------------------------------------
void TEcgProcessor::HeartbeatCalculation( void )
{
	static int16_t data;
	static int16_t rOnStatus = 0;
	static int16_t rOffStatus = 0;
	static int16_t rMaxVal = 0;
	static uint16_t rOnOffPrd = 0;
	static bool StartMeasurment;

//	if( Hartbeat.Status == false )
//		return;

	data = hartbeat_detector( (int16_t)EcgData.Channel_1.ChannelData );

	if( data > rMaxVal ){
		rMaxVal = data;
	}

	int16_t Threshold;

	static uint8_t OffStat = 0;

	Threshold = (int16_t)(((int16_t)rMaxVal * 1) / 2);

	if( data > Threshold ){ 												// засекли высокий уровень (пик)
		OffStat = 0;
    if(HR_Led_Enable) TDevice::LedOn();
//		LedOn();
//		TDevice::DEVICE->DebugTools->Led.On();
		if( StartMeasurment == false ){										// если таймер не запущен
			Hartbeat.Value = (int)( TIM_GetCounter( TIM2 ));
			TEcgProcessor::Hartbeat.Redraw = true;
			StartMeasurment = true;
			TIM_SetCounter( TIM2, 0 );										// сброс счётчика
		}

		rOnStatus++;
	}
	else {																	// засекли низкий уровень
		if( OffStat > 10 ){
			OffStat = 0;
			StartMeasurment = false;
      if(HR_Led_Enable) TDevice::LedOff();
//			LedOff();
//			TDevice::DEVICE->DebugTools->Led.Off();
		}
		OffStat++;
		rOffStatus++;
	}

	rOnOffPrd++;
	if( rOnOffPrd > 800 ){													// сброс порога, пика давно нет
		if((rOnStatus - rOffStatus) > 780)
			rMaxVal = 0;
		if((rOffStatus - rOnStatus) > 780)
			rMaxVal = 0;

		rOnOffPrd = 0;
		rOnStatus = 0;
		rOffStatus = 0;
	}
}
*/


/*----------------------------------------------------------------------------------*/
void TEcgProcessor::TimerInit( void )
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );

#ifdef CLK_48
	TIM_TimeBaseStructure.TIM_Prescaler = 2049;
#else
	TIM_TimeBaseStructure.TIM_Prescaler = 1024;
#endif /* CLK_48 */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit( TIM2, &TIM_TimeBaseStructure );

	TIM_Cmd( TIM2, ENABLE );		/* запускаем таймер для измерения периода между R зубцами */

	DBGMCU_Config( DBGMCU_TIM2_STOP, ENABLE );
}



/*----------------------------------------------------------------------------------*/
void TEcgProcessor::TimerDeinit( void )
{
	TIM_DeInit( TIM2 );
}