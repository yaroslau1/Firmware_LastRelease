#ifndef TFLASH_H
#define TFLASH_H
/*----------------------------------------------------------------------------------*/
//#include "TAppProcessor.h"
#include "main.h"
#include "common.h"
#include "TLcd.h"
/*----------------------------------------------------------------------------------*/
/* STORAGEFLASH start address in Flash */
#define STORAGEFLASH_START_ADDRESS    ((uint32_t)0x08010000) 
#define STORAGEFLASH_SIZE                     (uint32_t)0x10000  /* Page size = 64KByte */
#define STORAGEFLASH_SECTOR           FLASH_Sector_4

/* Page base and end addresses */
#define STORAGEFLASH_PAGE_BASE_ADDRESS      ((uint32_t)(STORAGEFLASH_START_ADDRESS + 0x000))
#define STORAGEFLASH_PAGE_END_ADDRESS       ((uint32_t)(STORAGEFLASH_START_ADDRESS + (PAGE_SIZE - 1)))
/*----------------------------------------------------------------------------------*/
/* Storage Flash Default Values */
const char defDeviceName[] = {"Cardian KR01-SNXXXX-XXXX"};
const char defPCBID[] = {"XXX"};
#define defSerialNumber 			0
#define defDataProduction 		1 //1.1.2010
//#define AD7799_CALIBRATION_DEF 	4113
//#define T1_CALIBRATION_DEF 		260
//#define T2_CALIBRATION_DEF 		650
//#define T1_ADCFIX_DEF 			1939
//#define T2_ADCFIX_DEF 			1723
//#define BAT1_CALIBRATION_DEF 		2400
//#define BAT2_CALIBRATION_DEF 		2900
//#define BAT1_ADCFIX_DEF 			2955
//#define BAT2_ADCFIX_DEF 			3574
#define LCDOFF_TIMER_DEF			1000
#define LCD_BRIGHT_DEF				0x20
/*----------------------------------------------------------------------------------*/
/* STORAGE FLASH ADDRESS MEMORY MAP */
//#define FIRST_START					STORAGEFLASH_START_ADDRESS
//#define SERIAL_NUM					(FIRST_START + sizeof(uint8_t))
//#define AD7799_SPLINES 				(SERIAL_NUM + sizeof(stufSerialNumber))
//#define AD7799_SPLINES_STORED 		(AD7799_SPLINES + (sizeof(Spline_tuple)*SPLINE_NODES_CNT))
//#define AD7799_CALIBRATION      		(AD7799_SPLINES_STORED + sizeof(uint8_t)) //sizeof(uint8_t) is size of AD7799_SPLINES_STORED
//#define T1_CALIBRATION 				(AD7799_CALIBRATION + sizeof(uint32_t))
//#define T2_CALIBRATION 				(T1_CALIBRATION + sizeof(uint32_t))
//#define T1_ADCFIX 					(T2_CALIBRATION + sizeof(uint32_t))
//#define T2_ADCFIX 					(T1_ADCFIX + sizeof(uint16_t))
//#define BAT1_CALIBRATION 				(T2_ADCFIX + sizeof(uint16_t))
//#define BAT2_CALIBRATION 				(BAT1_CALIBRATION + sizeof(uint32_t))
//#define BAT1_ADCFIX 					(BAT2_CALIBRATION + sizeof(uint32_t))
//#define BAT2_ADCFIX 					(BAT1_ADCFIX + sizeof(uint16_t))
//#define DEFAULT_SCRIPT			    (BAT2_ADCFIX + sizeof(uint16_t))
//#define START_UP_SYSTEM			    (DEFAULT_SCRIPT + 1700)
//#define BLUETOOTH_ADDR			    (START_UP_SYSTEM + sizeof(uint8_t))
//#define _NEXT 						(BLUETOOTH_ADDR + 6)
#define FIRST_START					STORAGEFLASH_START_ADDRESS
#define LCDOFF_TIMER				(FIRST_START + sizeof(uint8_t))		/* sizeof(uint8_t) определяет место отведённое под прошлую переменную */
#define LCD_BRIGHT					(LCDOFF_TIMER + sizeof(uint16_t))
#define SERIAL_NUM					(LCD_BRIGHT + sizeof(uint16_t))
#define LCD_NEXT  					(SERIAL_NUM + sizeof(stufSerialNumber))
/*----------------------------------------------------------------------------------*/
class TFlash
{
	private:
		static uint8_t SF_PageCache[STORAGEFLASH_SIZE];

	public:
		static void SF_Init();
		static void SF_Deinit();
		static void SF_SetDefaults();
//		static void SF_SetDefaultsAfterReset();
		static void SF_UpdatePage();
		static FLASH_Status SF_CommitPage();
		static void SF_StoreDataArray(uint32_t Address, uint8_t *Data,int Lenght);
		static void SF_StoreData8(uint32_t Address, uint8_t Data);
		static void SF_StoreData16(uint32_t Address, uint16_t Data);
		static void SF_StoreData32(uint32_t Address, uint32_t Data);
		static uint8_t SF_GetData8(uint32_t Address);
		static uint16_t SF_GetData16(uint32_t Address);
		static uint32_t SF_GetData32(uint32_t Address);
    static inline __IO uint8_t* SF_GetDataPtr(uint32_t Address) {return ((__IO uint8_t*)Address);} 
};
#endif