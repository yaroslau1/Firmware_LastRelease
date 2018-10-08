/************************************************************************************/
/* @title		SD card processor													*/
/* @author		Cyber Genius														*/
/* @version																			*/
/* @date																			*/
/************************************************************************************/
/* @brief																			*/
/************************************************************************************/
#include "TSDProcessor.h"
#include "TAppProcessor.h"
#include "TRespProcessor.h"
#include "TADS1298.h"
#include "THardware.h"
#include "TDevice.h"
#include "TPoolingNand.h"
/*----------------------------------------------------------------------------------*/
FIL TSDProcessor::MainFile;
FIL TSDProcessor::TrvFile;
FATFS TSDProcessor::FATFS_Obj;
bool TSDProcessor::SDStartWrPool = false;	
bool TSDProcessor::SDPresent = false;				/* флаг присутствия карты */
bool TSDProcessor::SDWriteEnable = false;
//static unsigned int BwW;							/* Bytes was written */
bool TSDProcessor::Def;						/* reset function */
/*----------------------------------------------------------------------------------*/
uint8_t TSDProcessor::TrMode = trverrNofile;
bool TSDProcessor::ReplaceECGFile = false;
bool TSDProcessor::NoAskReplaceECGFileQuestion = false;		/* чтобы не зацикливался в программе КР01 вопрос о переписываниии файла */
/*----------------------------------------------------------------------------------*/
int8_t TSDProcessor::EcgBuffer_1[ 2048 ];
int8_t TSDProcessor::EcgBuffer_2[ 2048 ];
bool TSDProcessor::EcgBuffer_1_Full = false;
bool TSDProcessor::EcgBuffer_2_Full = false;		/* флаг отмечающий полон ли буффер */
/*----------------------------------------------------------------------------------*/
xTaskHandle TSDProcessor::xSDPresentCheck;
xTaskHandle TSDProcessor::xSDDataWrite;


/*----------------------------------------------------------------------------------*/
/* @brief	Заполняет буффер данными ЭКГ для записи на SD карту						*/
/* @param	*Data - указатель на структуру ЭКГ										*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
//-----------------------------------------------------------------------------------
// data block format: byte
// | - 0 - | - 1 - | - 2 - | - 3 - |      - 4 -     | - 5 - | - 6 - |    - 7 -     |
// |       12-bit 3ch ECG data         |4 bit flags | 16bit PD data |ADXL axis data|
// flags - |3:res|2:PD data ready|1-0:ADXL axis|
// PD data ready: 0 - not ready, 1 - valid data
// ADXL axis: 01 - X axis, 10 - Y axis, 11 - Z axis
//-----------------------------------------------------------------------------------
void TSDProcessor::FillBuffer_ECG(EcgData_t *Data)
{
  static int16_t Ecg[9];
  static uint8_t adxl_axis = 0;
  static uint8_t VoltageSource = 0;
  uint8_t VoltageValue = 0;
  uint8_t TmpDataBuff[16];
  uint8_t dt_flags;
//  uint16_t *_pdd_buff_8 = (uint16_t*)&TmpDataBuff[5];
//  uint16_t *_pdd_buff_16 = (uint16_t*)&TmpDataBuff[13];

	if (TAppProcessor::ecg_format == 2) 
	{
		for(int i = 0; i < 8; i++)
		{
      Ecg[i] = (int16_t)(Data->Channel[i].ChannelData >> 4);		/* превращаем в 12 битные данные */
    }
		Ecg[8] = (int16_t)(Data->Channel[8].ChannelData >> 4);
//		Ecg[8] = (int16_t)(Data->Channel_10.ChannelData >> 4);

		for(int i = 0;i < 9;i++) // 12bit rejection
		{
//			Ecg[i] = (Ecg[i] * 1653L)/(1223L);
			Ecg[i] = (Ecg[i] * 1653L)/(1206L);
			if(Ecg[i] > 2047L) Ecg[i] = 2047L;
			else	
				if(Ecg[i] < -2048L) Ecg[i] = -2048L;
		}

		TmpDataBuff[0]   = (uint8_t)((Ecg[0]&0x0FF0)>>4);
		TmpDataBuff[1]   = (uint8_t)((Ecg[0]&0x000F)<<4);
		TmpDataBuff[1]  |= (uint8_t)((Ecg[1]&0x0F00)>>8);
		TmpDataBuff[2]   = (uint8_t)( Ecg[1]&0x00FF);
		TmpDataBuff[3]   = (uint8_t)((Ecg[2]&0x0FF0)>>4);
		TmpDataBuff[4]   = (uint8_t)((Ecg[2]&0x000F)<<4);
		TmpDataBuff[4]  |= (uint8_t)((Ecg[3]&0x0F00)>>8);
		TmpDataBuff[5]   = (uint8_t)( Ecg[3]&0x00FF);
	  TmpDataBuff[6]   = (uint8_t)((Ecg[4]&0x0FF0)>>4);
		TmpDataBuff[7]   = (uint8_t)((Ecg[4]&0x000F)<<4);
		TmpDataBuff[7]  |= (uint8_t)((Ecg[5]&0x0F00)>>8);
		TmpDataBuff[8]   = (uint8_t)( Ecg[5]&0x00FF);
		TmpDataBuff[9]   = (uint8_t)((Ecg[6]&0x0FF0)>>4);
		TmpDataBuff[10]  = (uint8_t)((Ecg[6]&0x000F)<<4);
		TmpDataBuff[10] |= (uint8_t)((Ecg[7]&0x0F00)>>8);
		TmpDataBuff[11]  = (uint8_t)( Ecg[7]&0x00FF);
		TmpDataBuff[12]  = (uint8_t)((Ecg[8]&0x0FF0)>>4);
		TmpDataBuff[13]  = (uint8_t)((Ecg[8]&0x000F)<<4);

		if (adxl_axis == 3)
    {
      if (VoltageSource == 0) 
      {
        VoltageValue  = (uint8_t) (TAppProcessor::BatVoltageValue / 20);
        adxl_axis = 3;
        VoltageSource++;
      }
      else if (VoltageSource == 1)
      {
        VoltageValue  = (uint8_t) (TAppProcessor::LiVoltageValue / 20);
        adxl_axis = 4;
        VoltageSource++;
      }
      else if (VoltageSource == 2)
      {
        VoltageValue  = (uint8_t) (TAppProcessor::AnalogVoltageValue / 20);
        adxl_axis = 5;
        VoltageSource++;
      }
      else
      {
        VoltageValue  = (uint8_t) (TAppProcessor::SysVoltageValue / 20);
        adxl_axis = 6;
        VoltageSource = 0;
      }
      TmpDataBuff[14]  = VoltageValue;
    }
		else TmpDataBuff[14]  = TAppProcessor::AX_CoordWr[adxl_axis];

		dt_flags = adxl_axis << 1;
		dt_flags |= (uint8_t)((Data->Status.ChannelBytes[0] & 0x08) >> 3);
                TmpDataBuff[15]  = dt_flags;
  
		adxl_axis++;
		if(adxl_axis > 3) adxl_axis = 0;
  
//		TPoolingNand::AddDataToPool(1,TmpDataBuff,16,1);
		TPoolingNand::AddDataToPool(1,TmpDataBuff,16,0);
	} 
	else if (TAppProcessor::ecg_format == 1)
        {
		for(int i = 0; i < 3; i++)
		{
      Ecg[i] = (int16_t)(Data->Channel[i].ChannelData >> 4);		/* превращаем в 12 битные данные */
    }
		Ecg[3] = (int16_t)(Data->Channel[8].ChannelData >> 4);

		for(int i = 0;i < 4;i++) // 12bit rejection
		{
//			Ecg[i] = (Ecg[i] * 1653L)/(1223L);
			Ecg[i] = (Ecg[i] * 1653L)/(1206L);
			if(Ecg[i] > 2047L) Ecg[i] = 2047L;
			else	
				if(Ecg[i] < -2048L) Ecg[i] = -2048L;
		}

		TmpDataBuff[0] =  (uint8_t)((Ecg[0]&0x00000FF0)>>4);
		TmpDataBuff[1] =  (uint8_t)((Ecg[0]&0x0000000F)<<4);
		TmpDataBuff[1] |= (uint8_t)((Ecg[1]&0x00000F00)>>8);
    TmpDataBuff[2]  = (uint8_t)( Ecg[1]&0x000000FF);
		TmpDataBuff[3] =  (uint8_t)((Ecg[2]&0x00000FF0)>>4);
		TmpDataBuff[4] =  (uint8_t)((Ecg[2]&0x0000000F)<<4);
    TmpDataBuff[4] |= (uint8_t)((Ecg[3]&0x00000F00)>>8);
    TmpDataBuff[5]  = (uint8_t)( Ecg[3]&0x000000FF);
  
		if (adxl_axis == 3)
    {
      if (VoltageSource == 0) 
      {
        VoltageValue  = (uint8_t) (TAppProcessor::BatVoltageValue / 20);
        adxl_axis = 3;
        VoltageSource++;
      }
      else if (VoltageSource == 1)
      {
        VoltageValue  = (uint8_t) (TAppProcessor::LiVoltageValue / 20);
        adxl_axis = 4;
        VoltageSource++;
      }
      else if (VoltageSource == 2)
      {
        VoltageValue  = (uint8_t) (TAppProcessor::AnalogVoltageValue / 20);
        adxl_axis = 5;
        VoltageSource++;
      }
      else
      {
        VoltageValue  = (uint8_t) (TAppProcessor::SysVoltageValue / 20);
        adxl_axis = 6;
        VoltageSource = 0;
      }
      TmpDataBuff[6]  = VoltageValue;
    }
		else TmpDataBuff[6]  = TAppProcessor::AX_CoordWr[adxl_axis];

		dt_flags = adxl_axis << 1;
		dt_flags |= (uint8_t)((Data->Status.ChannelBytes[0] & 0x08) >> 3);
    TmpDataBuff[7]  = dt_flags;

		adxl_axis++;
		if(adxl_axis > 3) adxl_axis = 0;
  
//		TPoolingNand::AddDataToPool(1,TmpDataBuff,8,1);
		TPoolingNand::AddDataToPool(1,TmpDataBuff,8,0);
	}
	else if (TAppProcessor::ecg_format == 0)
        {
		for(int i = 0; i < 3; i++)
		{
      Ecg[i] = (int16_t)(Data->Channel[i].ChannelData >> 4);		/* превращаем в 12 битные данные */
    }

		for(int i = 0;i < 3;i++) // 12bit rejection
		{
//			Ecg[i] = (Ecg[i] * 1653L)/(1223L);
			Ecg[i] = (Ecg[i] * 1653L)/(1206L);
			if(Ecg[i] > 2047L) Ecg[i] = 2047L;
			else	
				if(Ecg[i] < -2048L) Ecg[i] = -2048L;
		}

		TmpDataBuff[0] =  (uint8_t)((Ecg[0]&0x00000FF0)>>4);
		TmpDataBuff[1] =  (uint8_t)((Ecg[0]&0x0000000F)<<4);
		TmpDataBuff[1] |= (uint8_t)((Ecg[1]&0x00000F00)>>8);
		TmpDataBuff[2] =  (uint8_t)(Ecg[1]&0x000000FF);
		TmpDataBuff[3] =  (uint8_t)((Ecg[2]&0x00000FF0)>>4);
		TmpDataBuff[4] =  (uint8_t)((Ecg[2]&0x0000000F)<<4);
  
		if (adxl_axis == 3)
    {
      if (VoltageSource == 0) 
      {
        VoltageValue  = (uint8_t) (TAppProcessor::BatVoltageValue / 20);
        adxl_axis = 3;
        VoltageSource++;
      }
      else if (VoltageSource == 1)
      {
        VoltageValue  = (uint8_t) (TAppProcessor::LiVoltageValue / 20);
        adxl_axis = 4;
        VoltageSource++;
      }
      else if (VoltageSource == 2)
      {
        VoltageValue  = (uint8_t) (TAppProcessor::AnalogVoltageValue / 20);
        adxl_axis = 5;
        VoltageSource++;
      }
      else
      {
        VoltageValue  = (uint8_t) (TAppProcessor::SysVoltageValue / 20);
        adxl_axis = 6;
        VoltageSource = 0;
      }
      TmpDataBuff[5]  = VoltageValue;
    }
		else TmpDataBuff[5]  = TAppProcessor::AX_CoordWr[adxl_axis];

		dt_flags = adxl_axis << 1;
		dt_flags |= (uint8_t)((Data->Status.ChannelBytes[0] & 0x08) >> 3);
    TmpDataBuff[4]  |= (dt_flags & 0x0F);

		adxl_axis++;
		if(adxl_axis > 3) adxl_axis = 0;
  
//		TPoolingNand::AddDataToPool(1,TmpDataBuff,6,1);
		TPoolingNand::AddDataToPool(1,TmpDataBuff,6,0);
	}

}
/*----------------------------------------------------------------------------------*/
/* @brief	Заполняет буффер данными ЭКГ для записи на SD карту						*/
/* @param	*Data - указатель на структуру ЭКГ										*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
//void TSDProcessor::FillBuffer_ECG_0( EcgData_t *Data )
//{
//	static int State, BufferFillValue;
//	static int Ecg1, Ecg2, Ecg3;
//	static int8_t *pBuffer = NULL;
//	enum sd_buffer_fill_state{
//		start_fill = 0,				/* подготовка буффера к записи */
//		fill,						/* заполнение 1 */
//		continue_fill				/* заполнение 2 */
//	};
//
//#ifdef EV_BOARD
//	Ecg1 = Data->Channel_4.ChannelData >> 4;		/* превращаем в 12 битные данные */
//	Ecg2 = Data->Channel_5.ChannelData >> 4;
//	Ecg3 = Data->Channel_6.ChannelData >> 4;
//#else
//	Ecg1 = Data->Channel_1.ChannelData >> 4;		/* превращаем в 12 битные данные */
//	Ecg2 = Data->Channel_2.ChannelData >> 4;
//	Ecg3 = Data->Channel_3.ChannelData >> 4;
//#endif	
//	
//	if( Ecg1 > 2047 ) Ecg1 = 2047;					/* ограничитель для 12 битного кода */
//	if( Ecg1 < -2047 ) Ecg1 = -2047;
//	if( Ecg2 > 2047 ) Ecg1 = 2047;					/* ограничитель для 12 битного кода */
//	if( Ecg2 < -2047 ) Ecg1 = -2047;
//	if( Ecg3 > 2047 ) Ecg1 = 2047;					/* ограничитель для 12 битного кода */
//	if( Ecg3 < -2047 ) Ecg1 = -2047;
//
//	switch( State ){
//		case start_fill:
//		{
//			if( EcgBuffer_1_Full == false )			/* в зависимости от того какой буффер полный выбирается для заполнения пустой */
//				pBuffer = EcgBuffer_1;
//			else if( EcgBuffer_2_Full == false )
//				pBuffer = EcgBuffer_2;
//			else if(( EcgBuffer_2_Full == true ) && ( EcgBuffer_1_Full == true ))
//			{
//				TDevice::DEVICE->DebugTools->Led3.On();
//				__no_operation(); 					/* error, all buffers are full */
//			}
//			*(pBuffer++) = 0x55;					/* ID */
//			*(pBuffer++) = 0xAA;					/* ID */
//			*pBuffer = 0x01;						/* type of data in buffer */
//			pBuffer += 12;							/* 8 service bytes in package */
//			State = fill;
//		}
//
//		/* алгоритм записи */
//		case fill:
//		{
//			*(pBuffer++) = (int8_t)Ecg1;
//			*pBuffer = (int8_t)( Ecg1 >> 8 );
//			*pBuffer &= 0xF;
//
//			*(pBuffer++) |= (int8_t)( Ecg2 << 4 );
//			*(pBuffer++) = (int8_t)( Ecg2 >> 4 );
//
//			*(pBuffer++) = (int8_t)Ecg3;
//			*pBuffer = (int8_t)( Ecg3 >> 8 );
//			*pBuffer &= 0xF;
//
//			State = continue_fill;
//			break;
//		}
//		case continue_fill:
//		{
//			*(pBuffer++) |= (int8_t)( Ecg1 << 4 );
//			*(pBuffer++) = (int8_t)( Ecg1 >> 4 );
//
//			*(pBuffer++) = (int8_t)Ecg2;
//			*pBuffer = (int8_t)( Ecg2 >> 8 );
//			*pBuffer &= 0xF;
//
//			*(pBuffer++) |=(int8_t)( Ecg3 << 4 );
//			*(pBuffer++) = (int8_t)( Ecg3 >> 4 );
//
//			State = fill;
//			BufferFillValue++;
//			break;
//		}
//	}
//
//	if( BufferFillValue == 226 )				/* буффер полон 2034/9*/
//	{		
//		if( EcgBuffer_1[0] == 0x55 )
//			EcgBuffer_1_Full = true;
//		else if( EcgBuffer_2[0] == 0x55 )
//			EcgBuffer_2_Full = true;
//		else
//			__no_operation();
//
//		vTaskResume( xSDDataWrite );
//		State = start_fill;
//		pBuffer = NULL;
//		BufferFillValue = 0;
//	}
//}



///*----------------------------------------------------------------------------------*/
///* @brief	Задача смотрит фалги буфферов, и если есть заполненные буфферы,			*/
///*			то отправляет их на запись												*/
///* @param	None																	*/
///* @return	None																	*/
///*----------------------------------------------------------------------------------*/
//void TSDProcessor::TASK_SDDataWrite( void *pvParameters )
//{
//	vTaskSuspend( NULL );
//	
//	while( 1 )
//	{
//		static int8_t *pBuffer;
//
//		if( SDPresent == false ){
//			vTaskDelay( 100 / portTICK_RATE_MS );
//			continue;
//		}
//
//		if( EcgBuffer_1_Full == true )
//		{
//			#ifdef EV_BOARD
//				TDevice::DEVICE->DebugTools->Led2.On();
//				TDevice::DEVICE->DebugTools->Led3.Off();
//			#endif
//			
//			pBuffer = EcgBuffer_1;
//			if( f_write( &MainFile, pBuffer, sizeof(EcgBuffer_1), &BwW ) != FR_OK ){
//				NVIC_SystemReset();		/* Software reset program */
//			}
//			f_sync( &MainFile );
//			memset( pBuffer, 0x00, sizeof(EcgBuffer_1) );
//			EcgBuffer_1_Full = false;
//		}
//		else if( EcgBuffer_2_Full == true )
//		{
//			#ifdef EV_BOARD
//				TDevice::DEVICE->DebugTools->Led2.Off();
//				TDevice::DEVICE->DebugTools->Led3.On();
//			#endif
//			
//			pBuffer = EcgBuffer_2;
//			if( f_write( &MainFile, pBuffer, sizeof(EcgBuffer_2), &BwW ) != FR_OK ){
//				NVIC_SystemReset();		/* Software reset program */
//			}
//			f_sync( &MainFile );
//			memset( pBuffer, 0x00, sizeof(EcgBuffer_2) );
//			EcgBuffer_2_Full = false;
//		}
//
//		else {
//			vTaskSuspend( NULL );
//		}
//	}
//}
//


///*----------------------------------------------------------------------------------*/
///* @brief	Задача проверки наличия и подготовки SD карты							*/
///* @param	None																	*/
///* @return	None																	*/
///*----------------------------------------------------------------------------------*/
//void TSDProcessor::TASK_SDPresentCheck( void *pvParameters )
//{
//	while( 1 )
//	{
//		if( disk_initialize(0) == RES_OK )
//		{
//			SDPresent = true;
//			
//			if( f_mount( 0, &FATFS_Obj ) == FR_OK )
//			{
//				if( f_open( &TrvFile, "ECG.TRV", FA_OPEN_EXISTING | FA_WRITE | FA_READ ) != FR_OK )		/* File NOT exists */
//				{
//					f_open( &TrvFile, "ECG.TRV", FA_CREATE_NEW | FA_WRITE | FA_READ );		/* Create file */
//					
//					memset( EcgBuffer_1, 0x00, 5 );
//					EcgBuffer_1[0] = 1;
//					f_write( &TrvFile, EcgBuffer_1, 5, &BwW );		/* Write 1 in first byte, mean ECG file was read */
//					TrMode = trverrGood;
//				}
//				else		/* File TRV exists */
//				{
//					f_read( &TrvFile, EcgBuffer_1, 1, &BwW );
//					if( EcgBuffer_1[0] == 0x00 )		/* File ECG.ECG was NOT read */
//					{
//						SDWriteEnable = false;
//						TrMode = trverrNotRead;
//					}
//					else{
//						TrMode = trverrGood;
//					}
//				}
//				
//				f_close( &TrvFile );
//				
//				/* Creating and overwriting ECG file */
//				if( f_open( &MainFile, "KR01.RAW", FA_CREATE_ALWAYS | FA_WRITE ) == FR_OK )
//				{
//					write_patient_card();
//					vTaskSuspend( NULL );
//				}
//			}
//		}
//		SDPresent = false;
//		SDWriteEnable = false;
//		vTaskDelay( 100 / portTICK_RATE_MS );	/* запустить заного если карты нет или файл не создан */
//	}
//}



///*----------------------------------------------------------------------------------*/
///* @brief	None																	*/
///* @param	None																	*/
///* @return	Status																	*/
///*----------------------------------------------------------------------------------*/
//uint8_t TSDProcessor::StartWriting( void )
//{
//	enum result{ writing_ERROR, writing_OK };
//	
//	if(( TrMode == trverrNotRead ) && ( ReplaceECGFile == false ))
//		return writing_OK;		/* ECG file was NOT read, return with monitoring */
//	
//	if( Def == true ) return writing_OK;		/* Если запись карты уже была */
//	
//	if( disk_initialize(0) != RES_OK )
//	{
//		SDPresent = false;
//		SDWriteEnable = false;
//		TAppProcessor::Setup.u._bits.UseWriteToCard = 0;
//		return writing_ERROR;
//	}
//	
//	f_close( &MainFile );
//
//	memset( EcgBuffer_1, 0x00, 5 );
//	f_open( &TrvFile, "ECG.TRV", FA_OPEN_EXISTING | FA_WRITE );
//	f_write( &TrvFile, EcgBuffer_1, 1, &BwW );
//	f_close( &TrvFile );
////	TrMode = trverrNotRead;			/* mark, that file ecg was NOT read */
//
//	/* Setting record start date/time */
//	TAppProcessor::PatientCard.DateTimeOfStart.Time = TAppProcessor::DeviceDateTime.Time;
//	TAppProcessor::PatientCard.DateTimeOfStart.Date = TAppProcessor::DeviceDateTime.Date;
//	TAppProcessor::PatientCard.StartPosition = sizeof( EcgBuffer_1 );
//	
//	f_open( &MainFile, "KR01.RAW", FA_CREATE_ALWAYS | FA_WRITE );
//	f_lseek( &MainFile, 0 );		/* pointer to start of the file */
//	write_patient_card();
//	
//	ReplaceECGFile = false;
//	Def = true;
//	SDWriteEnable = true;
//	SDPresent = true;
//	
//	return writing_OK;
//}
//
//
//
///*----------------------------------------------------------------------------------*/
///* @brief	None																	*/
///* @param	None																	*/
///* @return	None																	*/
///*----------------------------------------------------------------------------------*/
//void TSDProcessor::StopWriting( void )
//{
//	TAppProcessor::Setup.u._bits.UseDisableAutoTurnOff = 0;
//	TAppProcessor::Setup.u._bits.UseWriteToCard = 0;
//	TAppProcessor::Setup.u._bits.UseRWaveIndicator = 0;
//	SDPresent = true;
//	SDWriteEnable = false;
//	Def = false;
//	ReplaceECGFile = false;
//	TrMode = trverrNotRead;
//	
//	vTaskSuspend( xSDDataWrite );
//	vTaskResume( xSDPresentCheck );
//}
//
//
//
///*----------------------------------------------------------------------------------*/
///* @brief	Write patient card to SD												*/
///* @param	None																	*/
///* @return	None																	*/
///*----------------------------------------------------------------------------------*/
//void TSDProcessor::write_patient_card( void )
//{
//	memcpy( EcgBuffer_1, &TAppProcessor::PatientCard, sizeof(TAppProcessor::PatientCard) );
//	
//	f_write( &MainFile, EcgBuffer_1, sizeof(EcgBuffer_1), &BwW );
//	
//	f_sync( &MainFile );
//	
//	memset( EcgBuffer_1, 0x00, sizeof(EcgBuffer_1) );
//}
//


///*----------------------------------------------------------------------------------*/
///* @brief	None																	*/
///* @param	None																	*/
///* @return	None																	*/
///*----------------------------------------------------------------------------------*/
//bool TSDProcessor::check_ecg_for_travel( void )
//{
//	enum ecg_file_status{ ecg_file_was_read, ecg_file_was_not_read };
//	uint8_t data;
//	
//	
//}