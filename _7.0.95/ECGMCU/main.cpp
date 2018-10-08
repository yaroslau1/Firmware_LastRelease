/************************************************************************************/
/* @title		MAIN application file												*/
/* @author		Cyber Genius														*/
/* @version		0.5.1																*/
/* @date		10.07.2012															*/
/************************************************************************************/
/* @brief		Application for holter KP-01WTF										*/
/* @cpu			STM32F103VE															*/
/* @IDE			IAR 6.30 С++ Embedded												*/
/* @OS			FreeRTOS 7.2.0														*/
/************************************************************************************/
#include "main.h"
#include "IAP.h"

#include "THardware.h"

#include "TECG.h"
#include "TSpiSram.h"

#include "TInterpreter.h"
#include "TPowerManagement.h"
#include "TLed.h"
#include "TRtc.h"

//extern "C" uint32_t __checksum;                                 // импортируем контрольную сумму
#pragma section=".intvec"                                       // говорим, что будем использовать
#pragma section=".checksum"                                     // информацию о секциях


bool StopMode = false;
bool EnterStopMode = false;

/*__IO */uint16_t RegularConvData_Tab[7];
uint16_t RegularConvData_TabDMA[7];

uint8_t BuffA[BUFF_AB_SIZE];
uint8_t BuffB[BUFF_AB_SIZE];
bool    BuffA_B_Select = 0;
bool    BuffWriteOut = 0;
int     BuffAB_Indx = 0;

//--------------------------------------------------------------------------------------

uint32_t temprtcsec = 0;

bool chekCRC(uint32_t* begin, uint32_t* end, uint32_t read_crc)
{
  uint32_t* ptr = begin;
  RCC->AHBENR  |= RCC_AHBENR_CRCEN;                             // включаем такирование модуля CRC
  CRC->CR |= CRC_CR_RESET;                                      // Сбрасываем модуль.
  do
  {
    CRC->DR = *ptr;                                             // добавляем всю прошивку в CRC
  } while(++ptr != end);
  uint32_t calculated_crc = CRC->DR;
  RCC->AHBENR  &= ~RCC_AHBENR_CRCEN;                            // отключаем тактирование
  if (calculated_crc != read_crc) return false;                 // плохо :(
  else return true;
}


static void MCO_Config(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOA, GPIOB and GPIOE Clocks enable */
  RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);

  /* GPIOA Configuration: Channel 1, 2, 3, 4 and Channel 1N as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);

  uint16_t  TimerPeriod = 1;
  uint16_t  Channel1Pulse = 1;

  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);

  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);

  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);

  /* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

/*----------------------------------------------------------------------------------*/
/* @brief	Main																	*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
int main( void )
{

  MCO_Config();
  volatile uint32_t read_crc = __checksum;
#ifdef BOOTLOADER
  uint32_t* begin = (uint32_t*)__section_begin(".intvec");      // получаем начало прошивки
  uint32_t* end = (uint32_t*)__section_begin(".checksum");      // получаем начало CRC (конец прошивки)
  if(!chekCRC(begin, end, read_crc))
    while(1);

  IAP_Start();
  while( 1 );
#else

  static volatile uint32_t *Src  = (uint32_t *) 0x08006000;
  static volatile uint32_t *Dest = (uint32_t *) 0x20000000;

  for(int i = 0; i < 48; i++)
  {
    Dest[i] = Src[i];
  }

  //SYSCFG clock enable
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  //Set CFGR1 register with flash memory remap at address 0
  SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);

//  MCO_Config();
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  PWR_BackupAccessCmd(ENABLE);
  RTC_TamperCmd(RTC_Tamper_1, DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
	                                          functionality must be disabled */

  uint16_t  rec_lb = RTC_ReadBackupRegister(RTC_BKP_DR3);

  RTC_WriteBackupRegister(RTC_BKP_DR3,0xFFFF);

  TECG::Init();

  TSPISRAM::PHYInit();
  
//TECG::StartConversation();

  TPowerManagement::PowerManagement_Init();

  TLed::LED_PHYInit();

  TInterpreter::UartPHYInit();

  TInterpreter::TurnOnUart();

  TRtc::Rtc_Init();

  TRtc::RTC_TimeAdjust(186063387);       //24.11.2015 Example Adjust

  // устан. в системе питания
  TLed::Charge_Status = 0;

  //TInterpreter::IsMonitoring = true;

  while( 1 )
  {
#ifdef BUFFERED_TRANSFER

    if(BuffWriteOut)
    {
      while(!GPIO_ReadInputDataBit(SPISRAM_IRQ_PORT, SPISRAM_IRQ_PIN));
      static uint32_t spisram_addr = 0;
      if(BuffA_B_Select == 1) //Writeout buffA
        TSPISRAM::WriteSeq(spisram_addr, BUFF_AB_SIZE, BuffA);
      else
        TSPISRAM::WriteSeq(spisram_addr, BUFF_AB_SIZE, BuffB);

      spisram_addr += BUFF_AB_SIZE;

      if(spisram_addr >= SPISRAM_SIZE)
      {
        spisram_addr = 0;
        TSPISRAM::IRQControl(0);
        TSPISRAM::IRQControl(1);
      }

      BuffWriteOut = 0;
    }

#endif

    TInterpreter::ControllerInteraction();

    TPowerManagement::PowerControl();
    
//    if(TLed::LedMode != 2) TLed::LED_IndicateMode();
//    TLed::LED_IndicateMode();

//    PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);
    
  }
#endif /* BOOTLOADER */
}



/*----------------------------------------------------------------------------------*/
/* @brief	Обработка ошибок использования StdPeriphDriver							*/
/* @param	Not used																*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
#ifdef USE_FULL_ASSERT
void assert_failed( uint8_t* file, uint32_t line )
{
//	while( 1 ){
		__no_operation();
//		ITM_EVENT8_WITH_PC( 2, 0xAF );
//	}
}
#endif /* USE_FULL_ASSERT */




