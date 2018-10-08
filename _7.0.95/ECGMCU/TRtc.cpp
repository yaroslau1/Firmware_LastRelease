//------------------------------------------------------------------------------------
#include "main.h"

#include "TRtc.h"
//------------------------------------------------------------------------------------

const BYTE YearsMonthArray[] = {
	31,28,31,30,31,30,31,31,30,31,30,31, //2010
	31,28,31,30,31,30,31,31,30,31,30,31, //2011
	31,29,31,30,31,30,31,31,30,31,30,31, //2012
	31,28,31,30,31,30,31,31,30,31,30,31, //2013
	31,28,31,30,31,30,31,31,30,31,30,31, //2014
	31,28,31,30,31,30,31,31,30,31,30,31, //2015
	31,29,31,30,31,30,31,31,30,31,30,31, //2016
	31,28,31,30,31,30,31,31,30,31,30,31, //2017
	31,28,31,30,31,30,31,31,30,31,30,31, //2018
	31,28,31,30,31,30,31,31,30,31,30,31, //2019
	31,29,31,30,31,30,31,31,30,31,30,31, //2020
	31,28,31,30,31,30,31,31,30,31,30,31, //2021
	31,28,31,30,31,30,31,31,30,31,30,31, //2022
	31,28,31,30,31,30,31,31,30,31,30,31, //2023
	31,29,31,30,31,30,31,31,30,31,30,31, //2024
	31,28,31,30,31,30,31,31,30,31,30,31, //2025
	31,28,31,30,31,30,31,31,30,31,30,31, //2026
	31,28,31,30,31,30,31,31,30,31,30,31, //2027
	31,29,31,30,31,30,31,31,30,31,30,31, //2028
	31,28,31,30,31,30,31,31,30,31,30,31, //2029
	31,28,31,30,31,30,31,31,30,31,30,31, //2030
	31,28,31,30,31,30,31,31,30,31,30,31, //2031
	31,29,31,30,31,30,31,31,30,31,30,31, //2032
	31,28,31,30,31,30,31,31,30,31,30,31, //2033
	31,28,31,30,31,30,31,31,30,31,30,31, //2034
	31,28,31,30,31,30,31,31,30,31,30,31, //2035
	31,29,31,30,31,30,31,31,30,31,30,31}; //2036

const WORD YearsDaysArray[] = {
	365, //2010
	365, //2011
	366, //2012
	365, //2013
	365, //2014
	365, //2015
	366, //2016
	365, //2017
	365, //2018
	365, //2019
	366, //2020
	365, //2021
	365, //2022
	365, //2023
	366, //2024
	365, //2025
	365, //2026
	365, //2027
	366, //2028
	365, //2029
	365, //2030
	365, //2031
	366, //2032
	365, //2033
	365, //2034
	365, //2035
	366}; //2036
//------------------------------------------------------------------------------------

/*
void _RTC_WaitForLastTask(void)
{
  // Loop until RTOFF flag is set
  //while ((RTC->CRL & RTC_FLAG_RTOFF) == (uint16_t)RESET)
  //{
  // 	__no_operation();//taskYIELD();
  //}

	while((RTC->ISR & RTC_ISR_RSF) == RESET)
	{
    __no_operation();//taskYIELD();
	}
}
*/
//------------------------------------------------------------------------------------

/*
void _RTC_WaitForSynchro(void)
{
  // Clear RSF flag
  RTC->CRL &= (uint16_t)~RTC_FLAG_RSF;

	// Loop until RSF flag is set
  while ((RTC->CRL & RTC_FLAG_RSF) == (uint16_t)RESET)
  {
  	__no_operation();//taskYIELD();
  }
}
*/
//------------------------------------------------------------------------------------

void TRtc::Rtc_Init()
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Disable the Tamper Pin */
  //BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
  //                               functionality must be disabled */

  //RCC_BackupResetCmd(ENABLE);
  //RCC_BackupResetCmd(DISABLE);

	/*
  if (RTC_ReadBackupRegister(RTC_BKP_DR1) != 0xA5A5A5A5)
  {
    // Backup data register value is not correct or not yet programmed (when
    //   the first time the program is executed) 

		#ifdef LCD_TRACE
			TLcdTrace::AddLine("RTC not yet cfg...");
		#endif

    RTC_Configuration();

    // Adjust time by values entred by the user on the hyperterminal
    RTC_TimeAdjust(0);

    RTC_WriteBackupRegister(RTC_BKP_DR1, 0xA5A5A5A5);
  }
	else
  {
  */
  
  //RTC_TimeAdjust(0);  

  RTC_Configuration();
		
  /* Wait for RTC registers synchronization */
  /* Wait for RTC APB registers synchronisation */
  
  RTC_WaitForSynchro();

  /* Clear the RTC Alarm Flag */
  RTC_ClearFlag(RTC_FLAG_ALRAF);

  /* Clear the EXTI Line 17 Pending bit (Connected internally to RTC Alarm) */
  EXTI_ClearITPendingBit(EXTI_Line17);
  //}

//  /* Enable PWR and BKP clocks */
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

}
//------------------------------------------------------------------------------------

void TRtc::RTC_Configuration(void)
{
  RTC_InitTypeDef  RTC_InitStructure;

  //RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_AHB1Periph_BKPSRAM, ENABLE);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	RCC_LSICmd( DISABLE );
	// RCC_APB1Periph_BKP

	/* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

//  /* Reset Backup Domain */
//  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  //_RTC_WaitForSynchro();

  //PWR_BackupRegulatorCmd( ENABLE );

  /* Configure the RTC data register and RTC prescaler */
  /* ck_spre(1Hz) = RTCCLK(LSI) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/
  RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
  RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
  RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
  RTC_Init(&RTC_InitStructure);

//  /* Set the time to 00h 00mn 00s AM */
//  RTC_TimeStruct.RTC_H12     = RTC_H12_AM;
//  RTC_TimeStruct.RTC_Hours   = 0;
//  RTC_TimeStruct.RTC_Minutes = 0;
//  RTC_TimeStruct.RTC_Seconds = 0;
//  RTC_SetTime(RTC_Format_BCD, &RTC_TimeStruct);

	RTC_EXTI_Configuration();
  RTC_NVIC_Configuration();

  RTC_AlarmTypeDef RTC_AlarmStructure;

  /* Set the alarm A Masks */
  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_All;
  RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
  RTC_AlarmStructure.RTC_AlarmDateWeekDay = RTC_Weekday_Monday;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = 0x0;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x0;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x0;
  RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

  /* Set alarm A sub seconds and enable SubSec Alarm : generate 8 interrupts per Second */
  RTC_AlarmSubSecondConfig(RTC_Alarm_A, 0xFF, RTC_AlarmSubSecondMask_SS14_8);

  /* Enable alarm A interrupt */
  //RTC_ITConfig(RTC_IT_ALRA, ENABLE);

  //RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
  
  //RTC_ClearFlag(RTC_FLAG_ALRAF);
}
//------------------------------------------------------------------------------------

void TRtc::RTC_EXTI_Configuration()
{
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Configure EXTI Line17(RTC Alarm) to generate an interrupt on rising edge */
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}
//------------------------------------------------------------------------------------

void TRtc::RTC_NVIC_Configuration()
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
//------------------------------------------------------------------------------------

void TRtc::RTC_TimeAdjust(uint32_t TimeSec)
{
  RTC_TimeTypeDef  RTC_TimeStructure;
  RTC_DateTypeDef  RTC_DateStructure;

  if(TimeSec == 0)
  {
    /* Set the time to 00h 00mn 00s AM */
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = 0;
    RTC_TimeStructure.RTC_Minutes = 0;
    RTC_TimeStructure.RTC_Seconds = 0;
    RTC_DateStructure.RTC_Year = 10;
    RTC_DateStructure.RTC_Month = 1;
    RTC_DateStructure.RTC_Date = 1;
    RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Friday;
  }
  else
  {
    int y,m,d,hr,mi,se;
		TRtc::RTC_TimeStampToTDMY(TimeSec,&y,&m,&d,&hr,&mi,&se);
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = hr;
    RTC_TimeStructure.RTC_Minutes = mi;
    RTC_TimeStructure.RTC_Seconds = se;
    RTC_DateStructure.RTC_Year = (uint8_t)(y-2000L);
    RTC_DateStructure.RTC_Month = m;
    RTC_DateStructure.RTC_Date = d;
    RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Friday;
  }

  RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
  RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
}
//------------------------------------------------------------------------------------

uint32_t TRtc::RTC_TimeRead()
{
  RTC_TimeTypeDef  RTC_TimeStructure;
  RTC_DateTypeDef RTC_DateStructure;

  uint32_t ret = 0;

  RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);

  uint32_t y_sec = 0;
  int i;
	for(i = 0;i < 27;i++)
	{
		if(i == (RTC_DateStructure.RTC_Year-10)) break;
		y_sec += (YearsDaysArray[i]*86400L);
	}
  i *= 12;
  uint32_t m_sec = 0;
  int j;
	for(j = 1;j <= 12;j++)
	{
		if(j == (RTC_DateStructure.RTC_Month)) break;
    m_sec += (YearsMonthArray[i++]*86400L);
	}

  uint32_t d_sec = ((uint32_t)RTC_DateStructure.RTC_Date - 1)*86400L;

  uint32_t h_sec = (uint32_t)RTC_TimeStructure.RTC_Hours*3600L;
  uint32_t mi_sec = (uint32_t)RTC_TimeStructure.RTC_Minutes*60L;
  uint32_t se_sec = (uint32_t)RTC_TimeStructure.RTC_Seconds;

  ret = y_sec + m_sec + d_sec + h_sec + mi_sec + se_sec;

  return ret;
}
//------------------------------------------------------------------------------------

void TRtc::RTC_TimeStampToTDMY(uint32_t ttime,int *year,int *month, int *day,int *hours,int *minutes,int *seconds)
{
	//find year
	uint32_t yearIndx = 0;
	int i;
	for(i = 0;i < 27;i++)
	{
		yearIndx += (YearsDaysArray[i]*86400L);
		if(yearIndx > ttime) break;
	}
	yearIndx -= (YearsDaysArray[i]*86400L);
	*year = 2010 + i;
	i *= 12;//get month start Indx
	int j;
	uint32_t monthIndx = yearIndx;
	for(j = 0;j < 12;j++)
	{
		monthIndx += (YearsMonthArray[i++]*86400L);
		if(monthIndx > ttime) break;
	}
	monthIndx -= (YearsMonthArray[i - 1]*86400L);
	*month = j + 1;
	*day = (ttime - monthIndx) / (86400L);
	int daysec = *day * 86400L;
	*day += 1;
	monthIndx += daysec;
	daysec = (ttime - monthIndx);
	*hours = daysec / 3600L;
	int minsec = *hours * 3600L;
	monthIndx += minsec;
	minsec = (ttime - monthIndx);
	*minutes = minsec / 60L;
	*seconds = minsec % 60L;
}
//------------------------------------------------------------------------------------

int TRtc::RTC_TimeStampToMM()
{
	uint32_t ret = TRtc::RTC_TimeRead();
	return (ret%(3600*24))/60;
}
//------------------------------------------------------------------------------------

void TRtc::RTC_GetTDMY(int *year,int *month, int *day,int *hours,int *minutes,int *seconds)
{
	uint32_t ret = TRtc::RTC_TimeRead();

	RTC_TimeStampToTDMY(ret,year,month,day,hours,minutes,seconds);
}
//------------------------------------------------------------------------------------
