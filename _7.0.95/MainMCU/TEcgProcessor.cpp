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
			if(!TEcgProcessor::GetConvertResultTest())										// ���������� ������ ��� � ADS1298 
      {
        TestFail = true;
      }
      TestPacketCount++;
*/      
			TEcgProcessor::GetConvertResult();										/* ���������� ������ ��� � ADS1298 */
			TEcgProcessor::WrChannelsProcessing();									/* ��������� ������ ������� ��� */
			TRespProcessor::WrChannelsProcessing();									/* ��������� ������ ������� ��� */
      TEcgProcessor::WrEcgData.Channel[9].ChannelData = TRespProcessor::WrRespData.Channel_1.ChannelData;
//      TEcgProcessor::WrEcgData.Channel_9.ChannelData = 0xFF;
			TSDProcessor::FillBuffer_ECG( &TEcgProcessor::WrEcgData );			/* ��������� ������ ��� ������� ��� ������ �� ����� */
    }
//    GPIO_ResetBits( GPIOA, GPIO_Pin_8 );

//    TSPISRAM::IRQControl( 1 );
  }
}

/*----------------------------------------------------------------------------------*/
/* @brief	��� ��� ������															*/
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
    EcgData.Channel[i].ChannelData = (EcgData.Channel[i].ChannelData * 100) / ADS1298_NORMALIZE_DIVIDER;   // ����������� ������ �� ����
    EcgData.Channel[i].ChannelData = FHigh_0_32_Fixed(EcgData.Channel[i].ChannelData, i, false, ZeroAddu); // ������ ��������
    EcgData.Channel[i].ChannelData = FF_Butterworth_HF(EcgData.Channel[i].ChannelData, i);                 // ��������� �� ������� Fc = 50��, ����� ������� ���
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

  //WrEcgData.Channel[0].ChannelData = (WrEcgData.Channel[0].ChannelData * 100) / ADS1298_NORMALIZE_DIVIDER;       // ����������� ������ �� ����
  
  //WrEcgData.Channel[0].ChannelData = FHigh_0_32_Fixed(WrEcgData.Channel[0].ChannelData, 8, false, ZeroAddu);     // ������ ��������
  
  //WrEcgData.Channel[0].ChannelData = FF_Butterworth_HF(WrEcgData.Channel[0].ChannelData, 8);
  
  //WrEcgData.Channel[0].ChannelData *= -1;
  
  for(uint8_t i = 0; i < ch_cnt; i++)
  {
    WrEcgData.Channel[i].ChannelData = (WrEcgData.Channel[i].ChannelData * 100) / ADS1298_NORMALIZE_DIVIDER;       // ����������� ������ �� ����
    WrEcgData.Channel[i].ChannelData = Butterworth_Ecg_HF(WrEcgData.Channel[i].ChannelData, 8 + i);
    WrEcgData.Channel[i].ChannelData = FF_Butterworth_HF(WrEcgData.Channel[i].ChannelData, 8 + i);                 // ��������� �� ������� Fc = 50��, ����� ������� ���
    WrEcgData.Channel[i].ChannelData *= -1;
  }
}


/*
//------------------------------------------------------------------------------
// ������� ��� ��������� ��������, ��������� 2 ��������
// ����� ��� ���� ����� �������� �������� � 250 ��
//------------------------------------------------------------------------------
void TEcgProcessor::DataAveraging( void )
{
	static int NData,								// ����� ����������� ��������
		tempCH1, tempCH2, tempCH3;					// ��������� �������� �������

#ifdef EV_BOARD
	if( NData == 0 ){								// ������ ���������
		tempCH1 = EcgData.Channel_4.ChannelData;
		tempCH2 = EcgData.Channel_5.ChannelData;
		tempCH3 = EcgData.Channel_6.ChannelData;

		NData++;
		return;
	}
	if( NData == 1 ){		// ������ ���������
		EcgData.Channel_4.ChannelData = ( tempCH1 + EcgData.Channel_4.ChannelData ) / 2;
		EcgData.Channel_5.ChannelData = ( tempCH2 + EcgData.Channel_5.ChannelData ) / 2;
		EcgData.Channel_6.ChannelData = ( tempCH3 + EcgData.Channel_6.ChannelData ) / 2;

		NData = 0;
	}
#else
	if( NData == 0 ){		// ������ ���������
		tempCH1 = EcgData.Channel_1.ChannelData;
		tempCH2 = EcgData.Channel_2.ChannelData;
		tempCH3 = EcgData.Channel_3.ChannelData;

		NData++;
		return;
	}
	if( NData == 1 ){		// ������ ���������
		EcgData.Channel_1.ChannelData = ( tempCH1 + EcgData.Channel_1.ChannelData ) / 2;
		EcgData.Channel_2.ChannelData = ( tempCH2 + EcgData.Channel_2.ChannelData ) / 2;
		EcgData.Channel_3.ChannelData = ( tempCH3 + EcgData.Channel_3.ChannelData ) / 2;

		NData = 0;
	}
#endif

	if( TFrame::StartMonitioring == true )									// ���� ������ ���� ������� ��������� �� BT
		TFrame::AddSampleToMonitoringDataBuffer( &TEcgProcessor::EcgData );	// ��������� ������ ��� ������� �� BT
}
*/


/*----------------------------------------------------------------------------------*/
/* @brief	��� ��� ��������														*/
/* @param	wData - ������� ������													*/
/* @return	��������������� ������													*/
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

	/* ������������ */
	if(rY_i >  32766) rwData = 32766;
	else if(rY_i < -32766) rwData = -32766;
	else rwData = rY_i;
	/* ENDBLOCK */

	return (rwData*2);
}


/*
//------------------------------------------------------------------------------
// @brief	������� ��������� ���
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

	if( data > Threshold ){ 												// ������� ������� ������� (���)
		OffStat = 0;
    if(HR_Led_Enable) TDevice::LedOn();
//		LedOn();
//		TDevice::DEVICE->DebugTools->Led.On();
		if( StartMeasurment == false ){										// ���� ������ �� �������
			Hartbeat.Value = (int)( TIM_GetCounter( TIM2 ));
			TEcgProcessor::Hartbeat.Redraw = true;
			StartMeasurment = true;
			TIM_SetCounter( TIM2, 0 );										// ����� ��������
		}

		rOnStatus++;
	}
	else {																	// ������� ������ �������
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
	if( rOnOffPrd > 800 ){													// ����� ������, ���� ����� ���
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

	TIM_Cmd( TIM2, ENABLE );		/* ��������� ������ ��� ��������� ������� ����� R ������� */

	DBGMCU_Config( DBGMCU_TIM2_STOP, ENABLE );
}



/*----------------------------------------------------------------------------------*/
void TEcgProcessor::TimerDeinit( void )
{
	TIM_DeInit( TIM2 );
}