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
#include "common.h"

#include "THardware.h"
#include "TAppProcessor.h"
#include "TSDProcessor.h"
#include "TLcd.h"
#include "TAudio.h"
#include "IAP.h"
#include "TDevice.h"
//#include "dosfs.h"
#include "TInterpreter.h"

extern "C" uint32_t __checksum;                                 // импортируем контрольную сумму
#pragma section=".intvec"                                       // говорим, что будем использовать
#pragma section=".checksum"                                     // информацию о секциях

bool StopMode = false;
bool EnterStopMode = false;
bool StopModePeriod = true;
#include "TGuiObjectsV2.h"


void SetClock(void)
{
//******************************************************************************
//            PLL (clocked by HSE) used as System clock source                **
//******************************************************************************
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
  __delay_cycles(	5000000 );
  
  // Enable HSE 
  RCC->CR |= ((uint32_t)RCC_CR_HSEBYP);
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  // Wait till HSE is ready and if Time out is reached exit 
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;
  } while((HSEStatus == RESET) && (StartUpCounter < HSE_STARTUP_TIMEOUT));

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }

  if (HSEStatus == (uint32_t)0x01)
  {
    // Select regulator voltage output Scale 1 mode 
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    __DSB();
//    PWR->CR |= PWR_CR_VOS;
    PWR->CR &= ~PWR_CR_VOS;

    // HCLK = SYSCLK / 1
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

    // PCLK2 = HCLK / 1
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
    
    // PCLK1 = HCLK / 1
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;
   
    // Configure the main PLL 
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
    (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

    // Enable the main PLL 
    RCC->CR |= RCC_CR_PLLON;

    // Wait till the main PLL is ready 
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }
   

    // Configure Flash prefetch, Instruction cache, Data cache and wait state 
//    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_0WS;
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |FLASH_ACR_DCEN |FLASH_ACR_LATENCY_1WS;


    // Select the main PLL as system clock source 
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    // Wait till the main PLL is used as system clock source 
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
    {
    }
  }

}


static void MCO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);

  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

/*
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
*/

//  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
//  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_MCO);

  RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_1);
//  RCC_MCO2Config(RCC_MCO2Source_SYSCLK, RCC_MCO1Div_1);
}



bool chekCRC(uint32_t* begin, uint32_t* end, uint32_t read_crc)
{
  uint32_t* ptr = begin;
  RCC->AHB1ENR  |= RCC_AHB1ENR_CRCEN;                             // включаем такирование модуля CRC
  __DSB();
  CRC->CR |= CRC_CR_RESET;                                      // Сбрасываем модуль. 
  do 
  {
    CRC->DR = *ptr;                                             // добавляем всю прошивку в CRC
  } while(++ptr != end);
  
  uint32_t calculated_crc = CRC->DR; 
  RCC->AHB1ENR  &= ~RCC_AHB1ENR_CRCEN;                            // отключаем тактирование
  __DSB();
  if (calculated_crc != read_crc) return false;                 // плохо :(
  else return true;
}

/*----------------------------------------------------------------------------------*/
/* @brief	Функция простоя у FreeRTOS												*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void vApplicationIdleHook( void )
{

//	if(TAppProcessor::RecordInProgress && TLcd::LcdOff) 
//	if(EnterStopMode) 
	if((TGuiObjects::GUIMode == guioRecMode) 
     && (GPIO_ReadInputDataBit(DISP_BL_PORT, DISP_BL_PIN) == Bit_RESET) 
     && (StopModePeriod == false)) 
  {
//    GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
		StopMode = true;
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    GPIO_SetBits( GPIOA, GPIO_Pin_8 );
		PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
    GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
//		if (StopMode) SetClock();
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		StopMode = false;
		vTaskResume(TGuiObjects::xGUIObj);
	} 
  else 
  {
//    GPIO_SetBits( GPIOA, GPIO_Pin_8 );
		__WFI();	// Enter idle mode 
//    GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
	}
}

/*----------------------------------------------------------------------------------*/
/* @brief	Main																	*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
int main( void )
{ 
//PWR_EnterSTANDBYMode();
//PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
  
  SetClock();

  MCO_Config();

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);
  
  DBGMCU_Config(uint32_t DBGMCU_STOP, DISABLE);
  
#ifdef BOOTLOADER
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, BOOTLSTART_ADDR );

  volatile uint32_t read_crc = __checksum;
  uint32_t* begin = (uint32_t*)__section_begin(".intvec");      // получаем начало прошивки
  uint32_t* end = (uint32_t*)__section_begin(".checksum");      // получаем начало CRC (конец прошивки)
  if(!chekCRC(begin, end, read_crc)) 
    while(1);

	IAP_Start();
	while( 1 );

#else
        
  NVIC_SetVectorTable( NVIC_VectTab_FLASH, APPSTART_ADDR );
  RCC->CSR |= RCC_CSR_RMVF;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  
//  Включение MCO для теста тактовой частоты
//  MCO_Config();

/* Init backup registers */
//	RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE );
//	PWR_BackupAccessCmd( ENABLE );
//	BKP_TamperPinCmd( DISABLE );
//	BKP_WriteBackupRegister(BKP_DR2,BOOTLOADER_IN_MARKER);

	TDevice::SystemLowLevelInit();
  
  PWR_BackupAccessCmd(ENABLE);
 	RTC_TamperCmd(RTC_Tamper_1, DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
	                                 functionality must be disabled */
  
  uint16_t  rec_lb = RTC_ReadBackupRegister(RTC_BKP_DR3); 
  
  if(rec_lb == 0x5555) TAppProcessor::ExitFromRecordMode = true;
  else TAppProcessor::ExitFromRecordMode = false;

  if(rec_lb == 0xAAAA) TAppProcessor::ErrorInRecordMode = true;
  else TAppProcessor::ErrorInRecordMode = false;
  
  RTC_WriteBackupRegister(RTC_BKP_DR3,0xFFFF);

  TDevice::DEVICE->Lcd.Enable();
  TDevice::DEVICE->BattMeter.Enable();
  TDevice::DEVICE->Keyboard.Enable();
  TDevice::DEVICE->Bluetooth.Enable();
  TDevice::DEVICE->Ecg.Enable();

//TDevice::DEVICE->SD.Enable();
  
  TDevice::DEVICE->Hartbeat->Enable();
  TDevice::DEVICE->DebugTools->Enable();
  
  TInterpreter::UartInit();
  TInterpreter::TurnOnUart();

//TDevice::DEVICE->Audio.Enable();
              /* задача инициализации устройства */
 // xTaskCreate( TAppProcessor::TASK_Init, "Init", configMINIMAL_STACK_SIZE+128, NULL,
//	tskIDLE_PRIORITY+0, NULL );
  
  xTaskCreate( TAppProcessor::TASK_StartUp, "Startup", configMINIMAL_STACK_SIZE+128, NULL,
		tskIDLE_PRIORITY+0, NULL );

	/* задача загрузки приложения */
  //xTaskCreate( TAppProcessor::TASK_Loading, "Startup", configMINIMAL_STACK_SIZE+128, NULL,
//		tskIDLE_PRIORITY+0, NULL );

  xTaskCreate( TFrame::TASK_CheckISRFlag, "CheckISR", configMINIMAL_STACK_SIZE+128, NULL,
		tskIDLE_PRIORITY+2, NULL );
  
  xTaskCreate( TInterpreter::TASK_CmdProcessor, "InterpProcessor", 
				configMINIMAL_STACK_SIZE+1, NULL, 
				tskIDLE_PRIORITY+2, NULL );  


//	/* задача сканирования клавиатуры */
//	if( TDevice::DEVICE->Keyboard.Status == true ){
//		xTaskCreate( TAppProcessor::TASK_KeyProcessor, "KeyProcessor", configMINIMAL_STACK_SIZE, NULL,
//			tskIDLE_PRIORITY+0, &TAppProcessor::xKeyProcessor );
//	}

//	/* задача мигания светодиодом */
//	xTaskCreate( TAppProcessor::TASK_LEDblink, "LEDblink", configMINIMAL_STACK_SIZE, NULL,
//		tskIDLE_PRIORITY+0, &TAppProcessor::xLEDblink );


//	TAudio::Speex_WriteInit();

//	xTaskCreate( TAudio::TASK_WriteOnSD, "sound", configMINIMAL_STACK_SIZE+512, NULL,
//			tskIDLE_PRIORITY+1, &TAudio::xWriteOnSD );

//	xTaskCreate(  TASK_TEST_SDCARD, "test", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

//Buzzer();
        
	vTaskStartScheduler();
	
	while( 1 ){
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




