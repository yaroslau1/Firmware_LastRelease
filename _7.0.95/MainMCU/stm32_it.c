/************************************************************************************/
/* @title		Interrupt functions													*/
/* @author		Cyber Genius														*/
/* @version		0.1																	*/
/* @date		13.07.2012															*/
/************************************************************************************/
/* @brief																			*/
/************************************************************************************/
#include "main.h"
//#include "clocks.h"

#include "stm32_it.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "THardware.h"
#include "TADS1298.h"
#include "TADS1292.h"
#include "TFrame.h"
#include "TSDProcessor.h"
#include "TAudio.h"
#include "IAP.h"
#include "TAppProcessor.h"
#include "TRespProcessor.h"
#include "TDevice.h"
#include "TPoolingNand.h"
#include "TGuiObjectsV2.h"
#include "TInterpreter.h"

extern volatile WORD Timer;

uint16_t TIM4_AutoreloadVal = 2999;

extern bool StopMode;
extern bool EnterStopMode;
extern bool StopModePeriod;


//#define LED_IN_IRQ
//#define LED_PACE


/*----------------------------------------------------------------------------------*/
void SetClockTo40MHz(void)
{
//******************************************************************************
//            PLL (clocked by HSE) used as System clock source                **
//******************************************************************************
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
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
    RCC->PLLCFGR = 16 | (154 << 6) | (((4 >> 1) -1) << 16) |
                   (RCC_PLLCFGR_PLLSRC_HSI) | (4 << 24);

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

/*----------------------------------------------------------------------------------*/
void SysReset()
{

  __delay_cycles(600000);
#ifndef BOOTLOADER
  TInterpreter::SendFrameToHost(CMD_RST);
  __delay_cycles(600000);
#endif
  NVIC_SystemReset();

	while(1) 
	{}	
}
/*----------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------*/
/* @brief  This function handles NMI exception.										*/
/* @param  None																		*/
/* @retval None																		*/
/*----------------------------------------------------------------------------------*/
void NMI_Handler(void)
{
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
  SysReset();
}



/*----------------------------------------------------------------------------------*/
/* @brief  This function handles Hard Fault exception								*/
/* @param  None																		*/
/* @retval None																		*/
/*----------------------------------------------------------------------------------*/
void HardFault_Handler(void)
{
//	uint32_t link = __get_LR();
//	uint32_t sp   = __get_SP();
//
// 	SCB_Type scb;
//	uint32_t nv = scb.BFAR;
//
//	BKP_WriteBackupRegister(BKP_DR15,(uint16_t)(link & 0x0000FFFF));
//	BKP_WriteBackupRegister(BKP_DR16,(uint16_t)((link & 0xFFFF0000)>>16));
//
//	BKP_WriteBackupRegister(BKP_DR17,(uint16_t)(sp & 0x0000FFFF));
//	BKP_WriteBackupRegister(BKP_DR18,(uint16_t)((sp & 0xFFFF0000)>>16));
//
//	BKP_WriteBackupRegister(BKP_DR19,(uint16_t)(nv & 0x0000FFFF));
//	BKP_WriteBackupRegister(BKP_DR20,(uint16_t)((nv & 0xFFFF0000)>>16));
//
//	BKP_WriteBackupRegister(BKP_DR21,0x0004);

	/* Go to infinite loop when Hard Fault exception occurs */

//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
  SysReset();
//  NVIC_SystemReset();

	while (1)
	{
		//TDevice::LedOn();
    
    __no_operation();		/* trollface =) */
	}
}



/*----------------------------------------------------------------------------------*/
/* @brief  This function handles Memory Manage exception.							*/
/* @param  None																		*/
/* @retval None																		*/
/*----------------------------------------------------------------------------------*/
void MemManage_Handler(void)
{
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
  SysReset();
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1)
	{
	}
}



/*----------------------------------------------------------------------------------*/
/* @brief  This function handles Bus Fault exception.								*/
/* @param  None																		*/
/* @retval None																		*/
/*----------------------------------------------------------------------------------*/
void BusFault_Handler(void)
{
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
  SysReset();
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1)
	{
	}
}



/*----------------------------------------------------------------------------------*/
/* @brief  This function handles Usage Fault exception.								*/
/* @param  None																		*/
/* @retval None																		*/
/*----------------------------------------------------------------------------------*/
void UsageFault_Handler(void)
{
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
  SysReset();
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1)
	{
	}
}


/*----------------------------------------------------------------------------------*/
/* @brief  This function handles SVCall exception.									*/
/* @param  None																		*/
/* @retval used in FreeRTOS!														*/
/*----------------------------------------------------------------------------------*/
void SVC_Handler(void)
{
#ifndef BOOTLOADER
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
	vPortSVCHandler();
#endif // BOOTLOADER
}


/*----------------------------------------------------------------------------------*/
/* @brief  This function handles Debug Monitor exception.							*/
/* @param  None																		*/
/* @retval None																		*/
/*----------------------------------------------------------------------------------*/
void DebugMon_Handler(void)
{
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
}



/*----------------------------------------------------------------------------------*/
/* @brief  This function handles PendSVC exception.									*/
/* @param  None																		*/
/* @retval used in FreeRTOS!														*/
/*----------------------------------------------------------------------------------*/
void PendSV_Handler(void)
{
  #ifndef BOOTLOADER
//    GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
	  xPortPendSVHandler();
  #endif // BOOTLOADER
}



/*----------------------------------------------------------------------------------*/
/* @brief  This function handles SysTick Handler.									*/
/* @param  None																		*/
/* @retval used in FreeRTOS!														*/
/*----------------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
  #ifndef BOOTLOADER
//    GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
	  xPortSysTickHandler();
  #endif // BOOTLOADER
}



/*----------------------------------------------------------------------------------*/
/* @brief	Обработка внешнего прерывания по спаду импульса с DRDY					*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------*/
/* @brief	Bluetooth USART transmition												*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void USART1_IRQHandler( void )
{
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
  #ifndef BOOTLOADER
	  
//    if (USART_GetFlagStatus(LMX_USART, USART_FLAG_ORE) != RESET)               //USART_FLAG_ORE
//	  {	
//      //USART_ClearITPendingBit (LMX_USART, USART_IT_ORE_RX);                    //USART_IT_ORE = USART_IT_ORE_ER 
//      
//      //USART_ClearITPendingBit (LMX_USART, USART_IT_ORE_ER);
//      
//      USART_ClearFlag(LMX_USART, USART_FLAG_ORE);
//      
//      (void)USART_ReceiveData(LMX_USART);
//		  //TFrame::ISR_USART_Rcv();
//	  }
       
	  if( USART_GetITStatus( LMX_USART, USART_IT_RXNE) != RESET )
	  {
		  TFrame::ISR_USART_Rcv();
	  }
    
  #else
	  
    if(USART_GetITStatus( LMX_USART, USART_IT_RXNE ) != RESET)
	  {
		  UsartRcv = (int)( (BYTE)USART_ReceiveData(LMX_USART) );
		  UsartRcv |= 0xFF00;
	  }
    
  #endif // BOOTLOADER
}



/*----------------------------------------------------------------------------------*/
/* @brief	Обработка внешнего прерывания по линии LSTAT							*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void EXTI9_5_IRQHandler( void )
{
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
  #ifndef BOOTLOADER
	  if( EXTI_GetITStatus( EXTI_Line9 ) != RESET ){
		  EXTI_ClearITPendingBit( EXTI_Line9 ); 			/* Clear the Key Button EXTI line pending bit */
		  //xTaskResumeFromISR( TFrame::xhCmdProcessor );
      TFrame::ResumeFromISR = true;
    }
  #endif // BOOTLOADER
}


/*----------------------------------------------------------------------------------*/
/* @brief	Обработка ICI_IRQ					*/
/* @param	None								*/
/* @return	None								*/
/*----------------------------------------------------------------------------------*/
void EXTI2_IRQHandler( void )
{
#ifndef BOOTLOADER
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );

	if( EXTI_GetITStatus( EXTI_Line2 ) != RESET )								/* DRDY line trigger */
	{	
//    GPIO_SetBits( GPIOA, GPIO_Pin_8 );
    StopModePeriod = true;
    if (StopMode) SetClockTo40MHz();
    
		xTaskResumeFromISR( TEcgProcessor::xhECGWrProcessing );

		EXTI_ClearITPendingBit( EXTI_Line2 );
    
    //увелич. счетчик ICI_IRQ_Cnt
    TNandWrite::ICI_IRQ_Cnt ++;
    
    //проверка счетчика ICI_IRQ_Cnt
    if (TNandWrite::ICI_IRQ_Cnt > 50)
      SysReset();
    
	}
#endif /* BOOTLOADER */
}

#ifndef BOOTLOADER
/*----------------------------------------------------------------------------------*/
/* @brief	SDIO interrupt															*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
#ifdef SD_SDIO_MODE
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
void SDIO_IRQHandler( void )
{
	SD_ProcessIRQSrc();	  /* Process All SDIO Interrupt Sources */
}
#endif /* SD_SDIO_MODE */


/*----------------------------------------------------------------------------------*/
/* @brief	DMA1 Channel1 Interrupt	Handler	func										*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void AU_PMD_IRQHandler( int strtIndx )
{
  int j = strtIndx;
  for(int i = 0;i < 512;i++)
  {
     TAudio::Audio_raw_data[i] = TAudio::ADC1_raw_data[j++];
  }
  if(TAppProcessor::audio_rec_start) TPoolingNand::AddDataToPool(2,(uint8_t*)&TAudio::Audio_raw_data[0],1024,1);
}

/*----------------------------------------------------------------------------------*/
/* @brief	DMA1 Channel1 Interrupt	Handler											*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void DMA2_Stream0_IRQHandler( void )
{
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
	if( DMA_GetITStatus( DMA2_Stream0, DMA_IT_HTIF0 ))
	{
		AU_PMD_IRQHandler(0);
		DMA_ClearITPendingBit( DMA2_Stream0, DMA_IT_HTIF0 );
	}
	if( DMA_GetITStatus( DMA2_Stream0, DMA_IT_TCIF0 ))
	{
		AU_PMD_IRQHandler(512);
		DMA_ClearITPendingBit( DMA2_Stream0, DMA_IT_TCIF0 );
	}
}



/*----------------------------------------------------------------------------------*/
/* @brief	ADC 1 & 2 Interrupt	Handler												*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void ADC1_2_IRQHandler( void )
{
//	if( ADC_GetITStatus( ADC1, DMA_IT_TCIF0 ))
//	{
////    GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
//  	ADC_ClearITPendingBit( ADC1, ADC_IT_EOC );
//  }
//	if( ADC_GetITStatus( ADC2, DMA_IT_TCIF0 ))
//	{
////    GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
//  	ADC_ClearITPendingBit( ADC2, ADC_IT_EOC );
//  }
}



/*----------------------------------------------------------------------------------*/
/* @brief	ADC3 Interrupt Handler													*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void ADC3_IRQHandler( void )
{
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
	ADC_ClearITPendingBit( ADC3, ADC_IT_EOC );
}

/*----------------------------------------------------------------------------------*/
/* @brief	Прерывание для SD карты													*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
#ifdef SD_SPI_MODE
void TIM3_IRQHandler( void )
{
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
//	static bool _So;

	if( TIM_GetITStatus( TIM3, TIM_IT_Update ) != RESET )
	{
		TIM_ClearITPendingBit( TIM3, TIM_IT_Update );
				
		Timer++;			/* Performance counter for this module */
		disk_timerproc();	/* Drive timer procedure of low level disk I/O module */
//    THardware::LedInvert();
	}
}
#endif /* SD_SPI_MODE */

/*----------------------------------------------------------------------------------*/
void EXTI0_IRQHandler( void )
{
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
	if( EXTI_GetITStatus( EXTI_Line0 ) != RESET ){
		EXTI_ClearITPendingBit( EXTI_Line0 );
	}
}

/*----------------------------------------------------------------------------------*/
void EXTI1_IRQHandler( void )
{
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
	if( EXTI_GetITStatus( EXTI_Line1 ) != RESET ){
		EXTI_ClearITPendingBit( EXTI_Line1 );
	}
}


void USART3_IRQHandler( void )
{
  uint16_t status = INTER_PORT->SR;
  uint16_t data = INTER_PORT->DR;
  if(status & USART_IT_RXNE)
  {
//    USART_ClearITPendingBit(INTER_PORT, USART_IT_RXNE);
    TInterpreter::ISR_USART_Rcv(data);
  }
}

#endif /* BOOTLOADER */
