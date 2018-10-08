/************************************************************************************/
/* @title																			*/
/* @author		Cyber Genius														*/
/* @version		0.1																	*/
/* @date		12.10.2012															*/
/************************************************************************************/
/* @brief																			*/
/************************************************************************************/
#include "TFlash.h"
#include "TAppProcessor.h"


/*----------------------------------------------------------------------------------*/
uint8_t TFlash::SF_PageCache[STORAGEFLASH_SIZE];



/*----------------------------------------------------------------------------------*/
void TFlash::SF_Init()
{
  /* Unlock the Flash Program Erase controller */
  FLASH_Unlock();
  if( SF_GetData8(FIRST_START) != 0xBA )
  {
  	/* Init First Start */
  	SF_SetDefaults();
  }
}



/*----------------------------------------------------------------------------------*/
//void TFlash::SF_SetDefaultsAfterReset()
//{
//	SF_UpdatePage();
//
//	SF_StoreData8(FIRST_START,0xFF);
//
//	SF_CommitPage();
//}



/*----------------------------------------------------------------------------------*/
void TFlash::SF_SetDefaults()
{
		SF_UpdatePage();

		stufSerialNumber sernum;

		strcpy((char*)sernum.DeviceName,defDeviceName);
//		strcpy((char*)sernum.PCBID,defPCBID);
		sernum.Serial = defSerialNumber;
		sernum.DateOfProduction = defDataProduction;

		SF_StoreDataArray(SERIAL_NUM,(uint8_t*)&sernum,sizeof(stufSerialNumber));

//		SF_StoreData32(AD7799_CALIBRATION,AD7799_CALIBRATION_DEF);
//		SF_StoreData32(T1_CALIBRATION,T1_CALIBRATION_DEF);
//		SF_StoreData32(T2_CALIBRATION,T2_CALIBRATION_DEF);
//		SF_StoreData16(T1_ADCFIX,T1_ADCFIX_DEF);
//		SF_StoreData16(T2_ADCFIX,T2_ADCFIX_DEF);
//
//		SF_StoreData32(BAT1_CALIBRATION,BAT1_CALIBRATION_DEF);
//		SF_StoreData32(BAT2_CALIBRATION,BAT2_CALIBRATION_DEF);
//		SF_StoreData16(BAT1_ADCFIX,BAT1_ADCFIX_DEF);
//		SF_StoreData16(BAT2_ADCFIX,BAT2_ADCFIX_DEF);
//
//		SF_StoreData8(AD7799_SPLINES_STORED,0xFF);

		SF_StoreData16( LCDOFF_TIMER, 1000 );
		SF_CommitPage();

		SF_UpdatePage();
    SF_StoreData8( LCD_BRIGHT, LCD_BRIGHT_DEF );
		SF_CommitPage();

		SF_UpdatePage();
		SF_StoreData8( FIRST_START, 0xBA );

		SF_CommitPage();
}



/*----------------------------------------------------------------------------------*/
void TFlash::SF_Deinit()
{
  /* Lock the Flash Program Erase controller */
  FLASH_Lock();
}



/*----------------------------------------------------------------------------------*/
void TFlash::SF_UpdatePage()
{
	uint32_t AbsAddress;

	for(uint32_t i = 0;i < STORAGEFLASH_SIZE;i++)
		{
			AbsAddress = STORAGEFLASH_START_ADDRESS + i;
			SF_PageCache[i] = (*(__IO uint8_t*)AbsAddress);
		}
}



/*----------------------------------------------------------------------------------*/
FLASH_Status TFlash::SF_CommitPage()
{
	FLASH_Status FlashStatus = FLASH_COMPLETE;
	uint32_t AbsAddress;

	FlashStatus = FLASH_EraseSector(STORAGEFLASH_SECTOR, VoltageRange_3);

  if(FlashStatus == FLASH_COMPLETE)
  {
		for(uint32_t i = 0;i < STORAGEFLASH_SIZE;i += 2)
			{
				uint16_t WordData;

				WordData = (uint16_t)SF_PageCache[i] + (uint16_t)(SF_PageCache[i+1] << 8);

				AbsAddress = STORAGEFLASH_START_ADDRESS + i;
	      FlashStatus = FLASH_ProgramHalfWord(AbsAddress, WordData);
	      /* If program operation was failed, a Flash error code is returned */
	      if (FlashStatus != FLASH_COMPLETE)
	      {
	        break;
	      }
			}
	}
	#ifdef USE_FREERTOS_HEAP
		vPortFree(SF_PageCache);
		SF_PageCache = NULL;
	#endif

	return 	FlashStatus;
}



/*----------------------------------------------------------------------------------*/
void TFlash::SF_StoreDataArray(uint32_t Address, uint8_t *Data,int Lenght)
{
	for(int i = 0;i < Lenght;i++)
		SF_PageCache[Address-STORAGEFLASH_START_ADDRESS + i]	= Data[i];
}



/*----------------------------------------------------------------------------------*/
void TFlash::SF_StoreData8(uint32_t Address, uint8_t Data)
{
	SF_PageCache[Address-STORAGEFLASH_START_ADDRESS]	= Data;
}



/*----------------------------------------------------------------------------------*/
void TFlash::SF_StoreData16(uint32_t Address, uint16_t Data)
{
	SF_PageCache[Address-STORAGEFLASH_START_ADDRESS]	= (BYTE)(Data & 0x00FF);
	SF_PageCache[Address-STORAGEFLASH_START_ADDRESS+1]	= (BYTE)((Data & 0xFF00)>>8);
}



/*----------------------------------------------------------------------------------*/
void TFlash::SF_StoreData32(uint32_t Address, uint32_t Data)
{
	SF_PageCache[Address-STORAGEFLASH_START_ADDRESS]	= (BYTE)(Data & 0x000000FF);
	SF_PageCache[Address-STORAGEFLASH_START_ADDRESS+1]	= (BYTE)((Data & 0x0000FF00) >> 8);
	SF_PageCache[Address-STORAGEFLASH_START_ADDRESS+2]	= (BYTE)((Data & 0x00FF0000) >> 16);
	SF_PageCache[Address-STORAGEFLASH_START_ADDRESS+3]	= (BYTE)((Data & 0xFF000000) >> 24);
}



/*----------------------------------------------------------------------------------*/
uint8_t TFlash::SF_GetData8(uint32_t Address)
{
	uint32_t AbsAddress;
	AbsAddress = Address;
	return (*(__IO uint8_t*)AbsAddress);
}



/*----------------------------------------------------------------------------------*/
uint16_t TFlash::SF_GetData16(uint32_t Address)
{
	uint32_t AbsAddress;
	AbsAddress = Address;
	return (*(__IO uint16_t*)AbsAddress);
}



/*----------------------------------------------------------------------------------*/
uint32_t TFlash::SF_GetData32(uint32_t Address)
{
	uint32_t AbsAddress;
	AbsAddress = Address;
	return (*(__IO uint32_t*)AbsAddress);
}