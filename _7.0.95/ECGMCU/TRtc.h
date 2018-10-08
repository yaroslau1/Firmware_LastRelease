//------------------------------------------------------------------------------------
#ifndef TRTC_H
#define TRTC_H
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
class TRtc
{
	public:
    static void Rtc_Init();
    static void RTC_TimeAdjust(uint32_t TimeSec);
    static uint32_t RTC_TimeRead();

		static void RTC_TimeStampToTDMY(uint32_t ttime,int *year,int *month, int *day,int *hours,int *minutes,int *seconds);
		static void RTC_GetTDMY(int *year,int *month, int *day,int *hours,int *minutes,int *seconds);

		static int  RTC_TimeStampToMM();
    
		//static void _RTC_WaitForLastTask();
		//static void _RTC_WaitForSynchro();

	protected:
  private:
		static void RTC_Configuration();
		static void RTC_EXTI_Configuration();
		static void RTC_NVIC_Configuration();
};
//------------------------------------------------------------------------------------
#endif
