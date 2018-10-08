
#include "TLedHardware.h"

#define SYSTICK_PERIOD			            16
#define LED_PAUSE_PERIOD			          4000
#define LED_ON_PERIOD       			      100
#define LED_OFF_PERIOD			            100


class TLed //: public TDSP
{
	public:
    static void LED_PHYInit();

    static inline void LedGreen_On() {GPIO_SetBits(LED1_PORT, LED1_PIN);}
    static inline void LedRed_On() {GPIO_SetBits(LED2_PORT, LED2_PIN);}

    static inline void LedGreen_Off() {GPIO_ResetBits(LED1_PORT, LED1_PIN);}
    static inline void LedRed_Off() {GPIO_ResetBits(LED2_PORT, LED2_PIN);}
    
    static inline void Leds_Off() {GPIO_ResetBits(LED1_PORT, LED1_PIN); GPIO_ResetBits(LED2_PORT, LED2_PIN);}
    
    static int  SysTickDelay;
    
    static bool LedToggle;
    
    static BYTE LedOnCounts;

    static void LED_IndicateMode();

    static void LED_IndicateCharge();

    static BYTE LedMode;

    static BYTE Charge_Status;

    static void TIM_IRQ_Handler();
};
