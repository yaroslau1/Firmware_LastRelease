//------------------------------------------------------------------------------------
#ifndef TPRW_H
#define TPRW_H
//------------------------------------------------------------------------------------

#include "main.h"

#include "TPowerManagementHardware.h"

//------------------------------------------------------------------------------------
#define LI_NOERRORS               0
#define LI_ERROR_NOLI             1
#define LI_ERROR_STC4054_BREAK    2

#define LI_MODE_AUTO              0
#define LI_MODE_OFF               1

//------------------------------------------------------------------------------------
class TPowerManagement
{
	public:
    static void PowerManagement_Init();
    static void StartConversion();
    static void DMA_IRQHandler();

    static uint16_t PowerNBat;
    static uint16_t PowerLBat_P;
    static uint16_t PowerLBat_N;

    static void PowerControl();

    static void Li_Enable();
    static void Li_Disable();

    static void FastCharge_Enable();
    static void FastCharge_Disable();

    static bool WritingMode;
    static bool F4SleepMode;

    static int  LiErrors;
    static int  LiMode;

	protected:
  private:
    static void ADC_Config();
    static void DMA_Config();
    static void TIM_Config();

};
//------------------------------------------------------------------------------------
#endif
