/********************************************************************************/
/* @title		Power Management 			*/
/* @author	Aleksis							  */
/* @version	0.1							      */
/* @date		26.11.2015						*/
/********************************************************************************/
/* @brief									*/
/********************************************************************************/

#include "TPowerManagement.h"
#include "TInterpreter.h"

#include "TAxel.h"
#include "TLed.h"
#include "TECG.h"

extern uint16_t RegularConvData_TabDMA[7];
extern uint16_t RegularConvData_Tab[7];

uint16_t TPowerManagement::PowerNBat = 0;

uint16_t TPowerManagement::PowerLBat_N = 0;

uint16_t TPowerManagement::PowerLBat_P = 0;

bool TPowerManagement::WritingMode = false;
bool TPowerManagement::F4SleepMode = false;

int  TPowerManagement::LiErrors = 0;
int  TPowerManagement::LiMode = LI_MODE_AUTO;

void TPowerManagement::PowerManagement_Init(void)
{
  ADC_Config();
  DMA_Config();
  TIM_Config();
  FastCharge_Disable();
  Li_Enable();
}

void TPowerManagement::ADC_Config(void)
{
	ADC_InitTypeDef     ADC_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;

	/* ADC1 DeInit */
	ADC_DeInit(PWR_ADC);

	/* GPIOA Periph clock enable */
	PWR_GPIO_RCC_ENABLE;

	/* ADC1 Periph clock enable */
	PWR_ADC_RCC_ENABLE;

  TAxel::Axel_Init();

  GPIO_SetBits( GPIOA, LBAT_EN_PIN );
	GPIO_InitStructure.GPIO_Pin = LBAT_EN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


  GPIO_ResetBits( GPIOA, FAST_CHRG_EN_PIN );
	GPIO_InitStructure.GPIO_Pin = FAST_CHRG_EN_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	/* Configure AX_EN */
	/*
  GPIO_SetBits( GPIOA, GPIO_Pin_15 );
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
  */

	/* Configure ADC channels as analog input */
	/*
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	*/

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  GPIO_InitStructure.GPIO_Pin = NBAT_PIN;
	GPIO_Init(PWR_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LBAT_P_PIN;
	GPIO_Init(PWR_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = LBAT_N_PIN;
	GPIO_Init(PWR_PORT, &GPIO_InitStructure);

	/* Initialize ADC structure */
	ADC_StructInit(&ADC_InitStructure);

	/* Configure the ADC1 in continuous mode withe a resolution equal to 12 bits  */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
//	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;

  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
  ADC_InitStructure.ADC_ExternalTrigConv =  ADC_ExternalTrigConv_T15_TRGO;

	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(PWR_ADC, &ADC_InitStructure);

	/* Convert the PWR_ADC Channels with 55.5 Cycles as sampling time */
	ADC_ChannelConfig(PWR_ADC, ADC_Channel_0 , ADC_SampleTime_55_5Cycles);
	ADC_ChannelConfig(PWR_ADC, ADC_Channel_1 , ADC_SampleTime_55_5Cycles);
	ADC_ChannelConfig(PWR_ADC, ADC_Channel_2 , ADC_SampleTime_55_5Cycles);

  ADC_ChannelConfig(PWR_ADC, ADC_Channel_3 , ADC_SampleTime_55_5Cycles);
	ADC_ChannelConfig(PWR_ADC, ADC_Channel_4 , ADC_SampleTime_55_5Cycles);
	ADC_ChannelConfig(PWR_ADC, ADC_Channel_5 , ADC_SampleTime_55_5Cycles);
	ADC_ChannelConfig(PWR_ADC, ADC_Channel_Vrefint , ADC_SampleTime_55_5Cycles);
	ADC_VrefintCmd(ENABLE);

	/* ADC Calibration */
	ADC_GetCalibrationFactor(PWR_ADC);

	/* ADC DMA request in circular mode */
	ADC_DMARequestModeConfig(PWR_ADC, ADC_DMAMode_Circular);

	/* Enable ADC_DMA */
	ADC_DMACmd(PWR_ADC, ENABLE);

	/* Enable the ADC peripheral */
	ADC_Cmd(PWR_ADC, ENABLE);

	/* Wait the ADRDY flag */
	while(!ADC_GetFlagStatus(PWR_ADC, ADC_FLAG_ADRDY));
  
  ADC_StartOfConversion(PWR_ADC);
}

void TPowerManagement::DMA_Config(void)
{
  DMA_InitTypeDef   DMA_InitStructure;

  /* PWR_DMA clock enable */
  PWR_DMA_RCC_ENABLE;

  /* DMA1 Channel1 Config */
  DMA_DeInit(PWR_DMA_CHAN);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&(PWR_ADC->DR));
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RegularConvData_TabDMA;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 7;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(PWR_DMA_CHAN, &DMA_InitStructure);
  /* DMA1 Channel1 enable */
  DMA_Cmd(PWR_DMA_CHAN, ENABLE);
  DMA_ITConfig(PWR_DMA_CHAN, DMA_IT_TC, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3; //104
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
}

void TPowerManagement::TIM_Config( void )
{
	NVIC_InitTypeDef NVIC_InitStructure;
  
	NVIC_InitStructure.NVIC_IRQChannel = PWR_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
  
  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  /* PWR_TIM clock enable */
  PWR_TIM_RCC_ENABLE;

  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Prescaler = 4096 - 1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 16 - 1;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit( PWR_TIM, &TIM_TimeBaseStructure );

  TIM_Cmd( PWR_TIM, ENABLE );   
  TIM_SelectOutputTrigger(PWR_TIM, TIM_TRGOSource_Update);
  TIM_ITConfig(PWR_TIM, TIM_IT_Update, ENABLE);

}


void TPowerManagement::StartConversion(void)
{
  ADC_StartOfConversion(PWR_ADC);
}

void TPowerManagement::DMA_IRQHandler(void)
{
  static uint16_t midd[3][32];
  static int    midIndx = 0;
  static int    acc[3] = {0,0,0};

  RegularConvData_Tab[6] = (int16_t)((int32_t) 3300 * *(__IO uint16_t *)VREFINT_CAL / RegularConvData_TabDMA[6]);
  for (int i = 0; i < 3; i++)
  {
    RegularConvData_Tab[i] = RegularConvData_TabDMA[i];
  }
  for (int i = 0; i < 3; i++)
  {
    uint16_t curr = (int16_t)((int32_t) RegularConvData_TabDMA[i+3] * RegularConvData_Tab[6] * 2 / 4096);
    acc[i] -= midd[i][midIndx];
    acc[i] += curr;
    midd[i][midIndx] = curr;
    RegularConvData_Tab[i+3] = acc[i] / 32;
  }
  midIndx++;
  if(midIndx >= 32) midIndx = 0;
}

bool Li_Enabled = false;

void TPowerManagement::Li_Enable()
{
  if(LiMode == LI_MODE_AUTO)
  {
    GPIO_ResetBits( GPIOA, LBAT_EN_PIN );
    Li_Enabled = true;
  }
}

void TPowerManagement::Li_Disable()
{
    GPIO_SetBits( GPIOA, LBAT_EN_PIN );
    Li_Enabled = false;
}

void TPowerManagement::FastCharge_Enable()
{
    GPIO_ResetBits( GPIOA, FAST_CHRG_EN_PIN );
}

void TPowerManagement::FastCharge_Disable()
{
  if(LiMode == LI_MODE_AUTO)
  {
    GPIO_SetBits( GPIOA, FAST_CHRG_EN_PIN );
  }
}

#define NICD_CHARGE_LVL0_THR      2250
#define NICD_CHARGE_LVL1_THR      2410
#define NICD_CHARGE_LVL2_THR      2450
#define NICD_CHARGE_LVL3_THR      2490
#define NICD_CHARGE_LVL4_THR      2550
#define NICD_CHARGE_LVL_HYST        10

#define NICD_STEPUP_VOLTAGE_THR   2000
#define POWERUP_SKIP              1000 //20000L //подобрать!
#define LI_DISCHARGE_VLTG         3000
#define LI_DETECT_VLTG             500 //2000
#define LI_OVER_VLTG              5300
//#define LI_SAFE_VLTG              3600

//#define MIN_WRITE_TIME            (24*60+30)*60*250 //24ч.30мин.

#pragma message("WARNING: Include in project F4SleepMode and WritingMode using")

void TPowerManagement::PowerControl()
{
  static int PowerUpSkip = POWERUP_SKIP;
  static uint16_t LI_SAFE_VLTG;

  PowerNBat = RegularConvData_Tab[3];
  PowerLBat_P = RegularConvData_Tab[4];
  PowerLBat_N = RegularConvData_Tab[5];
  LI_SAFE_VLTG = PowerLBat_P - 50;
  
  switch (TLed::Charge_Status)
  {
    case 0:
      if (PowerNBat > NICD_CHARGE_LVL0_THR + NICD_CHARGE_LVL_HYST) TLed::Charge_Status++;
      break;
    case 1:
      if (PowerNBat > NICD_CHARGE_LVL1_THR + NICD_CHARGE_LVL_HYST) TLed::Charge_Status++;
      else if (PowerNBat < NICD_CHARGE_LVL0_THR - NICD_CHARGE_LVL_HYST) TLed::Charge_Status--;
      break;
    case 2:
      if (PowerNBat > NICD_CHARGE_LVL2_THR + NICD_CHARGE_LVL_HYST) TLed::Charge_Status++;
      else if (PowerNBat < NICD_CHARGE_LVL1_THR - NICD_CHARGE_LVL_HYST) TLed::Charge_Status--;
      break;
    case 3:
      if (PowerNBat > NICD_CHARGE_LVL3_THR + NICD_CHARGE_LVL_HYST) TLed::Charge_Status++;
      else if (PowerNBat < NICD_CHARGE_LVL2_THR - NICD_CHARGE_LVL_HYST) TLed::Charge_Status--;
      break;
    case 4:
      if (PowerNBat > NICD_CHARGE_LVL4_THR + NICD_CHARGE_LVL_HYST) TLed::Charge_Status++;
      else if (PowerNBat < NICD_CHARGE_LVL3_THR - NICD_CHARGE_LVL_HYST) TLed::Charge_Status--;
      break;
    case 5:
      if (PowerNBat < NICD_CHARGE_LVL4_THR - NICD_CHARGE_LVL_HYST) TLed::Charge_Status--;
      break;
    default:
      TLed::Charge_Status = 0;
  }
/*
  if (PowerNBat <= 2400) TLed::Charge_Status = 1;
  else if (PowerNBat <= 2466) TLed::Charge_Status = 2;
  else if (PowerNBat <= 2533) TLed::Charge_Status = 3;
  else if (PowerNBat <= 2600) TLed::Charge_Status = 4;
  else TLed::Charge_Status = 5;
*/

  int Li_Voltage = PowerLBat_P - PowerLBat_N;
//  static int Li_VoltageValid = 3300;

//  static bool Li_EnabledPrev = 0;

//  if(Li_Enabled && !Li_EnabledPrev) PowerUpSkip = POWERUP_SKIP;
//  Li_EnabledPrev = Li_Enabled;

  if(PowerUpSkip > 0) //Ждем пока значения АЦП станут валидными
  {
    PowerUpSkip--;
    return;
  }

//  if(Li_Enabled) Li_VoltageValid = Li_Voltage;

  if(PowerNBat > NICD_STEPUP_VOLTAGE_THR) //NiCd power Enabled
  {
    Li_Enable();
  }
  else
  {
    if(TInterpreter::IsEcgWrite)
    {
      if(Li_Voltage < LI_DISCHARGE_VLTG)
      {
        //Li сел выключаемся
        Li_Disable();
      }
      else
      {
        Li_Enable();
      }
    }
    else
    {
        Li_Disable();
    }
  }
}

