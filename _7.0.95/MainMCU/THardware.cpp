/************************************************************************************/
/* Title:		Низкоуровневая инициализация переферии								*/
/* Author:		Cyber Genius														*/
/* Version:																			*/
/* Date:																			*/
/************************************************************************************/
/*																					*/
/************************************************************************************/
#include "main.h"
#include "THardware.h"
#include "TLcd.h"
#include "TADS1298.h"
#include "TDevice.h"
#include "TLcdTrace.h"
#include "TSound.h"

/*----------------------------------------------------------------------------------*/
volatile uint16_t THardware::ADC3_ScanResult[4];
/*----------------------------------------------------------------------------------*/
DebugTools_TypeDef THardware::DebugTools;

bool  THardware::BlockKeys = false;

/*----------------------------------------------------------------------------------*/
void THardware::TestPinControl( bool state ){
	if( state == true )	GPIO_SetBits( TEST_PORT, TEST_PIN );
	else GPIO_ResetBits( TEST_PORT, TEST_PIN );
}

void THardware::LedInvert( void )
{
	if(GPIO_ReadInputDataBit( TEST_PORT, TEST_PIN ) == Bit_SET)
    TestPinControl(0);
  else
    TestPinControl(1);
}

void THardware::LedOn( void )
{
	TestPinControl( 1 );
}

/*----------------------------------------------------------------------------------*/
void THardware::LedOff( void )
{
	TestPinControl( 0 );
}

/*----------------------------------------------------------------------------------*/
void THardware::TestPinInit( void )
{
	RCC_AHB1PeriphClockCmd( TEST_GPIO_CLK, ENABLE );

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = TEST_PIN;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( TEST_PORT, &GPIO_InitStructure );
}




/*----------------------------------------------------------------------------------*/
void THardware::TestPinDeinit( void )
{
}



/*----------------------------------------------------------------------------------*/
void THardware::KeysInit( void )
{

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd( UB1_BUTTON_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = UB1_BUTTON_PIN;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init( UB1_BUTTON_PORT, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = UB2_BUTTON_PIN;
	GPIO_Init( UB2_BUTTON_PORT, &GPIO_InitStructure );
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig( UB1_EXTI_PORT, UB1_EXTI_PIN );
	SYSCFG_EXTILineConfig( UB2_EXTI_PORT, UB2_EXTI_PIN );

	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;


	/* Configure Button EXTI line */
	EXTI_ClearITPendingBit( UB1_EXTI_LINE ); 			// Clear the Key Button EXTI line pending bit
	EXTI_InitStructure.EXTI_Line = UB1_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init( &EXTI_InitStructure );

	EXTI_ClearITPendingBit( UB2_EXTI_LINE ); 			// Clear the Key Button EXTI line pending bit
	EXTI_InitStructure.EXTI_Line = UB2_EXTI_LINE;
	EXTI_Init( &EXTI_InitStructure );

	/* Enable and set Button EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = UB1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	NVIC_InitStructure.NVIC_IRQChannel = UB2_IRQChannel;
	NVIC_Init( &NVIC_InitStructure );
}



/*----------------------------------------------------------------------------------*/
void THardware::KeysDeinit( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = UB1_BUTTON_PIN;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_NOPULL;
	GPIO_Init( UB1_BUTTON_PORT, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = UB2_BUTTON_PIN;
	GPIO_Init( UB2_BUTTON_PORT, &GPIO_InitStructure );
}


int THardware::CheckKey( int _Key )
{
  if(BlockKeys) return 0;
  
  switch( _Key )
  {
    case ChangeButton: 
      if( GPIO_ReadInputDataBit( UB1_BUTTON_PORT, UB1_BUTTON_PIN ) == 0 ) 
      {
        return 1; 
      }
      break;
    case SelectButton: 
      if( GPIO_ReadInputDataBit( UB2_BUTTON_PORT, UB2_BUTTON_PIN ) == 0 )
      { 
        return 1; 
      }
      break;
    default: 
      return 0;
  }
  return 0;
}


//------------------------------------------------------------------------------
/*
int THardware::CheckKey( int _Key )
{
	if(BlockKeys) return 0;

	int cnt = 0;
	bool press = false;
	static int LastPress = -1;

	if( LastPress == _Key )
	{
		switch( _Key )
		{
			case ChangeButton: 
				if ( GPIO_ReadInputDataBit( UB1_BUTTON_PORT, UB1_BUTTON_PIN ) == 0 ) 
				{
					//TSound::PlaySound(_modiShortBeep);
          
          return 0; 
				}
				else
				{
					LastPress = -1;; 
				} break;
			case SelectButton: 
				if ( GPIO_ReadInputDataBit( UB2_BUTTON_PORT, UB2_BUTTON_PIN ) == 0 )
				{
					//TSound::PlaySound(_modiShortBeep);
          
          return 0; 
				}
				else
				{
					LastPress = -1; 
				} break;
		}
	}
	do
	{
		switch( _Key )
		{
			case ChangeButton: 
				if( GPIO_ReadInputDataBit( UB1_BUTTON_PORT, UB1_BUTTON_PIN ) == 0 ) 
				{
					press = true;
				}
				else 
				{
					press = false;
				} 
        break;
			case SelectButton: 
				if( GPIO_ReadInputDataBit( UB2_BUTTON_PORT, UB2_BUTTON_PIN ) == 0 )
				{
					press = true;
				}
				else 
				{
					press = false;
				} 
        break;
		}
		if( press ) 
		{
			cnt++;
			if( cnt > 100 ) 
			{
				LastPress = _Key;
				return 2;
			}
		}
		vTaskDelay(20/portTICK_RATE_MS);
	} while (press);

	if( cnt < 3 ) return 0;
	else if( cnt < 100 ) return 1;
	else return 2;
}
*/

/*----------------------------------------------------------------------------------*/
void THardware::VoltmeterInit( void )
{
/*
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd( NiBat_GPIO_CLK | Axel_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE );
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

	RCC_ADCCLKConfig( RCC_PCLK2_Div8 );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC3, ENABLE );
#ifdef DEVICE_2
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC2, ENABLE );
#endif

//	NVIC_InitStructure.NVIC_IRQChannel = ADC3_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init( &NVIC_InitStructure );

//  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel4_5_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure); //Используется прерывание DMA канала


	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

	GPIO_InitStructure.GPIO_Pin = NiBat_GPIO_PIN;
	GPIO_Init( NiBat_GPIO_PORT, &GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = Axel_X_GPIO_PIN | Axel_Y_GPIO_PIN | Axel_Z_GPIO_PIN;
	GPIO_Init( Axel_GPIO_PORT, &GPIO_InitStructure );

	DMA_InitTypeDef  DMA_InitStructure;
	// DMA1 channel1 configuration ----------------------------------------------
	DMA_DeInit(DMA2_Channel5);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC3_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC3_ScanResult[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 4;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  
	DMA_Init(DMA2_Channel5, &DMA_InitStructure);
  
	// Enable DMA1 channel1 
	DMA_Cmd(DMA2_Channel5, ENABLE);
  
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 4;
	ADC_Init( ADC3, &ADC_InitStructure );

	ADC_RegularChannelConfig(ADC3, NiBat_ADC_Channel,  1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC3, Axel_X_ADC_Channel, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC3, Axel_Y_ADC_Channel, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC3, Axel_Z_ADC_Channel, 4, ADC_SampleTime_55Cycles5);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init( ADC2, &ADC_InitStructure );

	ADC_RegularChannelConfig(ADC2, NiBat_ADC_Channel,  1, ADC_SampleTime_55Cycles5);
	ADC_Cmd( ADC2, ENABLE );
//	ADC_ITConfig( ADC3, ADC_IT_EOC, ENABLE );
  
  ADC_DMACmd(ADC3, ENABLE);

	ADC_Cmd( ADC3, ENABLE );

	ADC_ResetCalibration( ADC3 );
	while( ADC_GetResetCalibrationStatus( ADC3 ));

	ADC_StartCalibration( ADC3 );
	while( ADC_GetCalibrationStatus( ADC3 ));

	ADC_SoftwareStartConvCmd( ADC3, ENABLE );
*/
}



/*----------------------------------------------------------------------------------*/
void THardware::VoltmeterDeinit( void )
{
/*
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC3, DISABLE );

	NVIC_InitStructure.NVIC_IRQChannel = ADC3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 14;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init( &NVIC_InitStructure );

	GPIO_InitStructure.GPIO_Pin = NiBat_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING;
	GPIO_Init( NiBat_GPIO_PORT, &GPIO_InitStructure );

	ADC_DeInit( ADC3 );
*/
}



////---------------------------------------------------------------------------

// void THardware::Switch_RCC_to_24Mhz(void)
// {
	// FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

	// /* Flash 2 wait state */
	// FLASH_SetLatency( FLASH_Latency_1 );

	// /* HCLK = SYSCLK */
	// RCC_HCLKConfig( RCC_SYSCLK_Div1 );

	// /* PCLK2 = HCLK */
	// RCC_PCLK2Config( RCC_HCLK_Div1 );

	// /* PCLK1 = HCLK/2 */
	// RCC_PCLK1Config( RCC_HCLK_Div1 );

	// /* ADCCLK = PCLK2/8 */
	// RCC_ADCCLKConfig( RCC_PCLK2_Div2 );

	// ChangePllMul( RCC_PLLSource_HSE_Div1, 3 );
// }
//---------------------------------------------------------------------------

// void THardware::Switch_RCC_to_72Mhz(void)
// {
//		if(CurrentSysClock72Mhz) return;
//	 CurrentSysClock72Mhz = true;

	// /* Enable Prefetch Buffer */
	// FLASH_PrefetchBufferCmd( FLASH_PrefetchBuffer_Enable );

	// /* Flash 2 wait state */
	// FLASH_SetLatency( FLASH_Latency_2 );

	// /* HCLK = SYSCLK */
	// RCC_HCLKConfig( RCC_SYSCLK_Div1 );

	// /* PCLK2 = HCLK */
	// RCC_PCLK2Config( RCC_HCLK_Div1 );

	// /* PCLK1 = HCLK/2 */
	// RCC_PCLK1Config( RCC_HCLK_Div2 );

	// /* ADCCLK = PCLK2/8 */
	// RCC_ADCCLKConfig( RCC_PCLK2_Div8 );

	// ChangePllMul( RCC_PLLSource_HSE_Div1, 9 );
// }
//---------------------------------------------------------------------------

// void THardware::ChangePllMul( uint32_t div,uint32_t PllMul )
// {

	// /* Если источник системной тактовой частоты HSI то ничего не делаем
	// Это означает, что система Clock security system сработала */
	// if(( RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0x0 ){
	// return;
	// }

	// /* Выбираем HSI как источник системной частоты */
	// RCC_SYSCLKConfig( RCC_SYSCLKSource_HSI);

	// /* Ожидаем, пока HSI выберется как источник системной частоты */
	// while(( RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != (uint32_t)0x00 ){
	// }

	// /* Выключаем PLL */
	// RCC_PLLCmd( DISABLE );

	// /* Ожидаем, пока PLL снимет бит готовности */
	// while( RCC_GetFlagStatus( RCC_FLAG_PLLRDY) == SET ){
	// }

	// /* Конфигурируем множитель PLL: PLLCLK = HSE * PllMul */
	// RCC_PLLConfig( div, (( PllMul - 2) << 18 ));

	// /* Включаем PLL */
	// RCC_PLLCmd( ENABLE );

	// /* Ожидаем, пока PLL выставит бит готовности */
	// while( RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET ){
	// }

	// /* Выбираем PLL как источник системной частоты */
	// RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );

	// /* Ожидаем, пока PLL выберется как источник системной частоты */
	// while(( RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08 ){
	// }

	// return;
// }
//------------------------------------------------------------------------------------