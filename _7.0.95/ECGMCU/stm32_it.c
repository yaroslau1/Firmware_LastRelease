/************************************************************************************/
/* @title		Interrupt functions													*/
/* @author		Cyber Genius														*/
/* @version		0.1																	*/
/* @date		13.07.2012															*/
/************************************************************************************/
/* @brief																			*/
/************************************************************************************/
#include "main.h"

#include "stm32_it.h"


#include "THardware.h"
#include "TLed.h"
#include "TECG.h"
#include "TSpiSram.h"
#include "TInterpreter.h"
#include "TPowerManagement.h"

extern uint16_t RegularConvData_Tab[7];

#define BUFFx32_LEN 32

uint8_t buff[BUFFx32_LEN];
uint8_t send_monitor_data[BUFFx32_LEN+5] = {0x55, CMD_MONITORINGDATA,
                                            BUFFx32_LEN, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                            0, 0xAA};

uint32_t SendCrc = 0;

int     buff_indx = 0;
uint8_t ADCi = 0;


extern uint8_t BuffA[BUFF_AB_SIZE];
extern uint8_t BuffB[BUFF_AB_SIZE];
extern bool    BuffA_B_Select;
extern int     BuffAB_Indx;
extern bool    BuffWriteOut;


int error = 0;
int ADC_Timer_4ms_Tick = 0;



/*----------------------------------------------------------------------------------*/
/* @brief  This function handles NMI exception.										*/
/* @param  None																		*/
/* @retval None																		*/
/*----------------------------------------------------------------------------------*/
void NMI_Handler(void)
{
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
	while (1)
	{
    //TAnalogBoard::LedOn();
		//TADS1298::LedOn();
		__no_operation();		/* trollface =) */
	}
}

/*----------------------------------------------------------------------------------*/
/* @brief  This function handles SVCall exception.									*/
/* @param  None																		*/
/* @retval used in FreeRTOS!														*/
/*----------------------------------------------------------------------------------*/
void SVC_Handler(void)
{
}

/*----------------------------------------------------------------------------------*/
/* @brief  This function handles PendSVC exception.									*/
/* @param  None																		*/
/* @retval used in FreeRTOS!														*/
/*----------------------------------------------------------------------------------*/
void PendSV_Handler(void)
{
}

/*----------------------------------------------------------------------------------*/
/* @brief  This function handles SysTick Handler.									*/
/* @param  None																		*/
/* @retval used in FreeRTOS!														*/
/*----------------------------------------------------------------------------------*/
void SysTick_Handler(void)
{

}

/*----------------------------------------------------------------------------------*/
/* @brief	Обработка внешнего прерывания по спаду импульса с DRDY					*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void EXTI2_3_IRQHandler( void )
{
  static uint8_t VoltageSource = 0;

	if( EXTI_GetITStatus( EXTI_Line2 ) != RESET )								/* DRDY line trigger */
	{
#ifndef BOOTLOADER

    ADC_Timer_4ms_Tick++;

    if((TInterpreter::IsMonitoring) || (TInterpreter::IsEcgWrite))
    {
      if (ADCi == 3)
      {
        if (VoltageSource == 0)
        {
      		TECG::ADSData[0]  = (uint8_t)((RegularConvData_Tab[3] & 0xFF00) >> 8);
  	    	TECG::ADSData[1]  = (uint8_t)(RegularConvData_Tab[3] & 0x00FF);
          VoltageSource++;
          ADCi = 3;
        }
        else if (VoltageSource == 1)
        {
          uint16_t LiVoltage = RegularConvData_Tab[4] - RegularConvData_Tab[5];
      		TECG::ADSData[0]  = (uint8_t)((LiVoltage & 0xFF00) >> 8);
  	    	TECG::ADSData[1]  = (uint8_t)(LiVoltage & 0x00FF);
          VoltageSource++;
          ADCi = 4;
        }
        else if (VoltageSource == 2)
        {
      		TECG::ADSData[0]  = (uint8_t)((RegularConvData_Tab[6] & 0xFF00) >> 8);
  	    	TECG::ADSData[1]  = (uint8_t)(RegularConvData_Tab[6] & 0x00FF);
          VoltageSource++;
          ADCi = 5;
        }
        else
        {
      		TECG::ADSData[0]  = (uint8_t)((RegularConvData_Tab[4] & 0xFF00) >> 8);
  	    	TECG::ADSData[1]  = (uint8_t)(RegularConvData_Tab[4] & 0x00FF);
          VoltageSource = 0;
          ADCi = 6;
        }
      }
      else
      {
        TECG::ADSData[0]  = (uint8_t)((RegularConvData_Tab[ADCi] & 0xFF00) >> 8);
  	    TECG::ADSData[1]  = (uint8_t)(RegularConvData_Tab[ADCi] & 0x00FF);
      }

	  	TECG::ADSData[2]  = TECG::ADSData[2] & 0x0F;
      TECG::ADSData[2] += (ADCi) << 4;

  		TECG::ADSData[30] = 0;
	  	TECG::ADSData[31] = 0;

      if(TInterpreter::IsEcgWrite)
      {
        TECG::SampleCounter++;
        uint8_t *buff_ptr;
        if(BuffA_B_Select == 0) //Buff_A selected
          buff_ptr = &BuffA[BuffAB_Indx];
        else
          buff_ptr = &BuffB[BuffAB_Indx];
        BuffAB_Indx += 32;
        if(BuffAB_Indx >= BUFF_AB_SIZE)
        {
          BuffAB_Indx = 0;
          BuffA_B_Select = !BuffA_B_Select;
          if(BuffWriteOut == 1)
          {
            error = 1; //overflow
            TLed::LedMode = 0xFF;
           	__delay_cycles( 1000000 );
            NVIC_SystemReset();
          }
          BuffWriteOut = 1;
        }
        for (int i = 0; i < 32; i++)
        {
      		buff_ptr[i] = TECG::ADSData[i];
        }
      }

      if(TInterpreter::IsMonitoring)
      {
        //SendFrameToHost //передача данных в MainMCU для мониторирования. можно использовать DMA
        SendCrc = CMD_MONITORINGDATA + BUFFx32_LEN;
        for (BYTE i=0; i<BUFFx32_LEN; i++)
        {
          send_monitor_data[i + 3] = TECG::ADSData[i];
          SendCrc += TECG::ADSData[i];
        }

        send_monitor_data[35] = SendCrc;
        TInterpreter::SendDMA_UART(BUFFx32_LEN + 5, send_monitor_data);
      }

/*      
      if (TLed::LedMode == 2)  // HR - ЧСС
      {
         TECG::HeartbeatCalculation();
      }
*/          

      TECG::GetConvertResult();										/* считывание данных экг с ADS1298 */
      

    }

		ADCi++;
		if(ADCi >= 4)
    {
			ADCi = 0;
//			ADC_StartOfConversion(ADC1);
		}


#endif /* BOOTLOADER */

		EXTI_ClearITPendingBit( EXTI_Line2 );
	}

}

#ifndef BOOTLOADER

void USART1_IRQHandler( void )
{
  TInterpreter::ISR_USART_Rcv();

  USART_ClearITPendingBit(INTER_PORT, USART_IT_ORE);
}

/**
  * @brief  This function handles RTC global interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
  EXTI_ClearITPendingBit(EXTI_Line17);
}

void DMA1_Channel1_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_IT_TC1))
  {
//    DMA_ClearITPendingBit(DMA1_IT_TC1);
    TPowerManagement::DMA_IRQHandler();

    /* Clear DMA1 Channel1 Half Transfer, Transfer Complete and Global interrupt pending bits */
    DMA_ClearITPendingBit(DMA1_IT_GL1);
  }
}

void DMA1_Channel4_5_IRQHandler(void)
{
  if(DMA_GetITStatus(DMA1_IT_TC4))
  {
    /* Clear DMA1 Channel1 Half Transfer, Transfer Complete and Global interrupt pending bits */
    DMA_ClearITPendingBit(DMA1_IT_GL4);

    TECG::DMA_IRQHandler();
  }
}

void TIM15_IRQHandler(void)
{
  if (TIM_GetITStatus(PWR_TIM, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(PWR_TIM, TIM_IT_Update);
    TLed::TIM_IRQ_Handler();
  }
  else
  {
    TLed::LedRed_On();
    TLed::LedGreen_Off();
  }

}

#endif /* BOOTLOADER */
