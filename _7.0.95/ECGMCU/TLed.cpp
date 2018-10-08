/********************************************************************************/
/* @title		Led control						*/
/* @author	Aleksis							*/
/* @version	0.1							*/
/* @date		20.11.2015						*/
/********************************************************************************/
/* @brief									*/
/********************************************************************************/

#include "main.h"

#include "TLed.h"

#include "TECG.h"

BYTE TLed::LedMode = 1;

BYTE TLed::Charge_Status = 3;

int  TLed::SysTickDelay = 0;

bool TLed::LedToggle = 0;

BYTE TLed::LedOnCounts = 0;

void TLed::LED_PHYInit(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOA, GPIOB and GPIOE Clocks enable */
  //RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);
  LED_GPIO_RCC_ENABLE;
  
  
  /* GPIOA Configuration: Channel 1, 2, 3, 4 and Channel 1N as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = LED1_PIN | LED2_PIN;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
  GPIO_Init(LED1_PORT, &GPIO_InitStructure);
  
 // GPIO_PinAFConfig(LED1_PORT, LED1_PINSOURCE, LED_AF);
 // GPIO_PinAFConfig(LED1_PORT, LED2_PINSOURCE, LED_AF);
  
  GPIO_ResetBits(LED1_PORT, LED1_PIN);
  GPIO_ResetBits(LED2_PORT, LED2_PIN);

  
  /*uint16_t  TimerPeriod = 409;
    uint16_t  Channel1Pulse = 100;

  // TIM1 clock enable 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);
  
  // Time Base configuration
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  // Channel 1, 2,3 and 4 Configuration in PWM mode
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);

  // TIM1 counter enable
  TIM_Cmd(TIM3, ENABLE);

  // TIM1 Main Output Enable
  TIM_CtrlPWMOutputs(TIM3, ENABLE);
  */
  
  //TECG::TimerInit();
}

void TLed::LED_IndicateMode()
{
  if (LedMode == 0)        // 4 - все ок
  {
    if ((SysTickDelay > (LED_PAUSE_PERIOD / 2 ) / SYSTICK_PERIOD - 1) && (LedOnCounts == 0))
    {
      LedOnCounts ++;
      
      SysTickDelay = 0;
      
      //LedToggle = false;              
    }
    
    if ((SysTickDelay > LED_OFF_PERIOD / SYSTICK_PERIOD - 1) && (!LedToggle) && (LedOnCounts<5) && (LedOnCounts != 0)) 
    {
      LED_IndicateCharge(); 
      LedToggle = true;
      LedOnCounts++;
      
      SysTickDelay = 0;
    }

    if ((SysTickDelay > LED_ON_PERIOD / SYSTICK_PERIOD - 1) && (LedToggle) && (LedOnCounts<=5)  && (LedOnCounts != 0))
    {
      Leds_Off();
      LedToggle = false;
      SysTickDelay = 0;
    }
       
    if (SysTickDelay > LED_PAUSE_PERIOD / SYSTICK_PERIOD - 1)
    {
      LedOnCounts = 1;
      
      SysTickDelay = 0;
      
      LedToggle = false;              
    }
  }
  
  else if (LedMode == 1)  // 2 - ошибка  
  {
    if ((SysTickDelay > (LED_PAUSE_PERIOD / 2) / SYSTICK_PERIOD - 1) && (LedOnCounts == 0))
    {
      LedOnCounts ++;
      
      SysTickDelay = 0;
      
      //LedToggle = false;              
    }
    
    if ((SysTickDelay > LED_OFF_PERIOD / SYSTICK_PERIOD - 1) && (!LedToggle) && (LedOnCounts<3) && (LedOnCounts != 0)) 
    {
      LED_IndicateCharge(); 
      LedToggle = true;
      LedOnCounts++;
     
      SysTickDelay = 0;
    }

    if ((SysTickDelay > LED_ON_PERIOD / SYSTICK_PERIOD - 1) && (LedToggle) && (LedOnCounts<=3) && (LedOnCounts != 0))
    {
      Leds_Off();
      LedToggle = false;
      SysTickDelay = 0;
    }
       
    if (SysTickDelay > LED_PAUSE_PERIOD / SYSTICK_PERIOD - 1)
    {
      LedOnCounts = 1;
      
      SysTickDelay = 0;
      
      LedToggle = false;              
    }  
  }
  
  
  else if (LedMode == 2)
  {
    if ((SysTickDelay > LED_OFF_PERIOD / SYSTICK_PERIOD - 1) && (!LedToggle) && (TECG::R)) 
    {
      LED_IndicateCharge(); 
      LedToggle = true;
      SysTickDelay = 0;
    }

    if ((SysTickDelay > LED_ON_PERIOD / SYSTICK_PERIOD - 1) && (LedToggle))
    {
      Leds_Off();
      LedToggle = false;
      SysTickDelay = 0;
      TECG::R = false;
    }
  }
  
  
  else if (LedMode == 3)
  {
    LED_IndicateCharge(); 
  }
  
  else
  {
    LedGreen_Off();
    LedRed_On();
  }
  
}

void TLed::LED_IndicateCharge()
{
/*
  if ((Charge_Status > 3) && (Charge_Status <= 5))
  {
    LedGreen_On();
    LedRed_Off();
  }
  
  else if (Charge_Status == 3) 
  {
    LedGreen_On();
    LedRed_On();
  }
  
  else if (Charge_Status == 2) 
  {
    LedGreen_On();
    LedRed_On();
  }
  
  else if (Charge_Status == 1)
  {
    LedGreen_Off();
    LedRed_On();
  }
  
  else if (Charge_Status == 0)
  {
    LedGreen_Off();
    LedRed_On();  
  }
*/
  LedGreen_On();
//  LedRed_Off();
}

void TLed::TIM_IRQ_Handler()
{
  SysTickDelay++;
  LED_IndicateMode();
}
