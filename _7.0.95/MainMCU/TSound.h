/*
    Title:    
    Author:   VN Asliuk(Cardian)
    Date:     02.09.2010 13:06:28
    Purpose:  
    Software: 
    Hardware: 
    Note:    
*/
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#ifndef TSOUND_H
#define TSOUND_H
//------------------------------------------------------------------------------------
//#include "TWinstarLcd.h"
#include "Utilities.h"
#include "TDevice.h"
#include "TLcdTrace.h"
//------------------------------------------------------------------------------------
#define SND_TICK_SIZE_MS 50            //50
#define SOUND_FREQUENCY  4100
//#define SOUND_OFF
//------------------------------------------------------------------------------------
const uint32_t _modiStartUp           = 0xC6000000;
const uint32_t _modiShortBeep         = 0xC0000000;
const uint32_t _modiStartBPMeasure    = 0xF0F0FFF0;
const uint32_t _modiStopMeasureReady  = 0xF0FC0000;
const uint32_t _modiStopMeasureError  = 0xFFFC0000;
const uint32_t _modiReadBegin         = 0xC3000000;
const uint32_t _modiReadEnd           = 0xC7000000;
const uint32_t _modiSetUp             = 0xCCC00000;
//------------------------------------------------------------------------------------

class TSound : public TDevice
{
	public:
		static void SoundInit();
		
		static void PlaySound(const uint32_t midi);
		
		static void SoundEnable(bool state);
		
	protected:
	private:	
		static void Sound_PHY_Init();
		static void SoundPPEnable();
		static void SoundPPDisable();
		static uint32_t CurrentSound;
		static bool Enabled;

    static xTaskHandle xhSound;
		static void TASK_SoundProcessor(void *pvParameters);
};
//------------------------------------------------------------------------------------
#endif
