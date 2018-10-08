/*
    Title:    
    Author:   VN Asliuk(Cardian)
    Date:     02.09.2010 13:06:23
    Purpose:  
    Software: 
    Hardware: 
    Note:    
*/
//------------------------------------------------------------------------------------
#include "TSound.h"
#include "TGuiObjectsV2.h"
#include "TAppProcessor.h"
//------------------------------------------------------------------------------------
uint32_t  TSound::CurrentSound = _modiStartUp;
xTaskHandle TSound::xhSound;
bool TSound::Enabled = true;	
//------------------------------------------------------------------------------------

void TSound::SoundInit()
{
	Sound_PHY_Init(); 
	xTaskCreate(TASK_SoundProcessor, (signed char *) "SND", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xhSound);
	vTaskDelay(100/portTICK_RATE_MS);			
}
//------------------------------------------------------------------------------------

void TSound::Sound_PHY_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Enable Clocks
	RCC_AHB1PeriphClockCmd(BUZZER_P_GPIO_CLK | BUZZER_N_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(BUZZER_TIM_CLK, ENABLE);

	//Positive GPIO configuration
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = BUZZER_P_GPIO_PIN;
	GPIO_Init( BUZZER_P_GPIO_PORT, &GPIO_InitStructure );
  
  GPIO_PinAFConfig(BUZZER_P_GPIO_PORT, BUZZER_P_GPIO_PINSOURCE, BUZZER_TIM_AF);
  
  //Negative GPIO configuration
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = BUZZER_N_GPIO_PIN;
  GPIO_Init( BUZZER_N_GPIO_PORT, &GPIO_InitStructure );

  GPIO_PinAFConfig(BUZZER_N_GPIO_PORT, BUZZER_N_GPIO_PINSOURCE, BUZZER_TIM_AF);
  
	// Time base configuration 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = (SystemCoreClock / SOUND_FREQUENCY) - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(BUZZER_TIM, &TIM_TimeBaseStructure);
	
	// PWM configuration: Channel2 
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = (SystemCoreClock / SOUND_FREQUENCY) / 2 - 1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	
  TIM_OC2Init(BUZZER_TIM, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(BUZZER_TIM, TIM_OCPreload_Enable);
  
  TIM_ARRPreloadConfig(BUZZER_TIM, ENABLE);
 
	// TIM enable
	TIM_CtrlPWMOutputs(BUZZER_TIM, ENABLE);	
  //TIM_Cmd(BUZZER_TIM, ENABLE);
}
//------------------------------------------------------------------------------------

void TSound::SoundPPEnable()
{
	/*
  GPIO_InitTypeDef GPIO_InitStructure;

	//Positive GPIO configuration
	GPIO_InitStructure.GPIO_Pin = BUZZER_P_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init( BUZZER_P_GPIO_PORT, &GPIO_InitStructure );

  //GPIO_PinAFConfig(BUZZER_P_GPIO_PORT, BUZZER_P_GPIO_PINSOURCE, GPIO_AF_TIM4);
  
	//Negative GPIO configuration
	GPIO_InitStructure.GPIO_Pin = BUZZER_N_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	//GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init( BUZZER_N_GPIO_PORT, &GPIO_InitStructure );
  
  //GPIO_PinAFConfig(BUZZER_N_GPIO_PORT, BUZZER_N_GPIO_PINSOURCE, GPIO_AF_TIM4);
  
  */
    
	TIM_Cmd(BUZZER_TIM, ENABLE);
}
//------------------------------------------------------------------------------------

void TSound::SoundPPDisable()
{
	TIM_Cmd(BUZZER_TIM, DISABLE);

  /*
	GPIO_InitTypeDef GPIO_InitStructure;
  
  //Positive GPIO configuration
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = BUZZER_P_GPIO_PIN;
	GPIO_Init( BUZZER_P_GPIO_PORT, &GPIO_InitStructure );

	//Negative GPIO configuration
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = BUZZER_N_GPIO_PIN;
	GPIO_Init( BUZZER_N_GPIO_PORT, &GPIO_InitStructure );
  */
}
//------------------------------------------------------------------------------------

void TSound::TASK_SoundProcessor(void *pvParameters)
{
	while(1)
	{
//	TAppProcessor::TaskFlags |= TASKF_SND;
		vTaskSuspend(NULL);
//	TAppProcessor::TaskFlags &= ~TASKF_SND;
		
		int iend = 4;
		int shift_sz = 24;
		int shift_sz_bk = 0;
		for(int h = 0;h < 4;h++)
		{
			BYTE snbyte = CurrentSound >> shift_sz_bk;
			shift_sz_bk += 8;
			if(snbyte == 0)
			{
				iend--;
			}
			else break;	
		}
		
		for(int i = 0;i < iend;i++)
		{
			BYTE snbyte = CurrentSound >> shift_sz;
			shift_sz -= 8;
			for(int j = 0;j < 8;j++)
			{
				BYTE sn_bit = snbyte & 0x80;
				if(sn_bit == 0x80)
					SoundPPEnable();
				else	
					SoundPPDisable();
				snbyte <<= 1;	
				vTaskDelay(SND_TICK_SIZE_MS/portTICK_RATE_MS);			
				if((i == (iend-1)) && (snbyte == 0)) break;
			}
		}
		SoundPPDisable();
	}
}
//------------------------------------------------------------------------------------

void TSound::PlaySound(const uint32_t midi)
{
	if(!Enabled) return;
	#ifndef SOUND_OFF
		CurrentSound = midi;
		vTaskResume(xhSound);
	#endif	
}
//------------------------------------------------------------------------------------

void TSound::SoundEnable(bool state)
{
	Enabled = state;
	#ifndef SOUND_OFF
//		TGuiObjects::ToolbarSet(TBFM_NOSOUND,!state);	
	#endif		
}
//------------------------------------------------------------------------------------
