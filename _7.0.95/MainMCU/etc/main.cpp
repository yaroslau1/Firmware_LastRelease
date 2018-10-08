/************************************************************************************/
/* @title		MAIN application file												*/
/* @author		Cyber Genius														*/
/* @version		0.1																	*/
/* @date		10.07.2012															*/
/************************************************************************************/
/* @brief		Application for holter KP-01WTF										*/
/* @cpu			STM32F103VE															*/
/* @IDE			IAR 6.30 С++ Embedded												*/
/* @OS			FreeRTOS 7.2.0														*/
/************************************************************************************/
#include "main.h"
#include "common.h"

#include "THardware.h"
#include "TAppProcessor.h"
#include "TSDProcessor.h"
#include "TLcd.h"
#include "TAudio.h"
#include "IAP.h"
#include "TDevice.h"



/*----------------------------------------------------------------------------------*/
/* @brief	Функция простоя у FreeRTOS												*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void vApplicationIdleHook( void )
{
	__WFI();	/* Enter idle mode */
}



/*----------------------------------------------------------------------------------*/
void TASK_TEST_SDCARD( void *pvParameters )
{
	static int STATE;
	static bool a;
	static uint8_t buff[512];
	static unsigned int s;
	while( 1 )
	{
		switch( STATE )
		{
			case 0:
				if( disk_initialize(0) == RES_OK ){
					if( f_mount( 0, &TSDProcessor::FATFS_Obj ) == FR_OK ){
						if( f_open( &TSDProcessor::MainFile, "data.dat", FA_CREATE_ALWAYS | FA_WRITE ) == FR_OK ){
							TDevice::DEVICE->DebugTools->Led1.On();
							STATE++;
						}
					}
				}
				break;

			case 1:
				memset( &buff, 0xAA, sizeof(buff) );
				STATE++;
				if( a == true ){
					TDevice::DEVICE->DebugTools->Led2.On();
					TDevice::DEVICE->DebugTools->Led3.Off();
					a = false;
				}
				else {
					TDevice::DEVICE->DebugTools->Led3.On();
					TDevice::DEVICE->DebugTools->Led2.Off();
					a = true;
				}
				vTaskDelay( 450 / portTICK_RATE_MS );
				break;

			case 2:
				f_write( &TSDProcessor::MainFile, &buff, sizeof(buff), &s );
				f_sync( &TSDProcessor::MainFile );
				STATE = 1;
				break;
		}
	}
}



/*----------------------------------------------------------------------------------*/
/* @brief	Main																	*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
int main( void )
{
#ifdef BOOTLOADER
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, BOOTLSTART_ADDR );
	IAP_Start();
	while( 1 );

#else
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, APPSTART_ADDR );

	/* Init backup registers */
//	RCC_APB1PeriphClockCmd( RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE );
//	PWR_BackupAccessCmd( ENABLE );
//	BKP_TamperPinCmd( DISABLE );
//	BKP_WriteBackupRegister(BKP_DR2,BOOTLOADER_IN_MARKER);

	TDevice::SystemLowLevelInit();

//	TDevice::DEVICE->Lcd.Enable();
//	TDevice::DEVICE->BattMeter.Enable();
//	TDevice::DEVICE->Keyboard.Enable();
//	TDevice::DEVICE->Bluetooth.Enable();
//	TDevice::DEVICE->Ecg.Enable();
//	TDevice::DEVICE->SD.Enable();
//	TDevice::DEVICE->Hartbeat->Enable();
	TDevice::DEVICE->DebugTools->Enable();

//	TDevice::DEVICE->Audio.Enable();

	/* задача загрузки приложения */
//	xTaskCreate( TAppProcessor::TASK_Loading, "Startup", configMINIMAL_STACK_SIZE+128, NULL,
//		tskIDLE_PRIORITY+1, NULL );

	/* задача обновления экана */
	if( TDevice::DEVICE->Lcd.Status == true ){
		xTaskCreate( TLcd::TASK_Invalidate, "Invalidate", configMINIMAL_STACK_SIZE+32, NULL,
			tskIDLE_PRIORITY+1, &TLcd::xLcdInvalidate );
	}

	/* задача сканирования клавиатуры */
	if( TDevice::DEVICE->Keyboard.Status == true ){
		xTaskCreate( TAppProcessor::TASK_KeyProcessor, "KeyProcessor", configMINIMAL_STACK_SIZE, NULL,
			tskIDLE_PRIORITY+0, &TAppProcessor::xKeyProcessor );
	}

	/* задача измерения напряжения батарей */
	if( TDevice::DEVICE->BattMeter.Status == true ){
		xTaskCreate( TAppProcessor::TASK_MeasureBatVolage, "BatteryVoltage", configMINIMAL_STACK_SIZE,
			NULL, tskIDLE_PRIORITY+0, NULL );
	}

//	TAudio::Speex_WriteInit();

//	xTaskCreate( TAudio::TASK_WriteOnSD, "sound", configMINIMAL_STACK_SIZE+512, NULL,
//			tskIDLE_PRIORITY+1, &TAudio::xWriteOnSD );

//	xTaskCreate(  TASK_TEST_SDCARD, "test", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );

//	vTaskStartScheduler();

//	static uint8_t _buf[ 2048 ];
//	static unsigned int s;
//	static FRESULT res;
//	static DWORD sect;
//	static FIL *fileptr;
//		
//	memset( _buf, 0x2A, sizeof(_buf) );
//	
//	if( disk_initialize(0) == RES_OK ){
//		if( f_mount( 0, &TSDProcessor::FATFS_Obj ) == FR_OK ){
//			if( f_open( &TSDProcessor::MainFile, "data.dat", FA_CREATE_ALWAYS | FA_WRITE ) == FR_OK ){
//				res = f_write( &TSDProcessor::MainFile, _buf, sizeof(_buf), &s );
//				if( res == FR_OK )
//					TDevice::DEVICE->DebugTools->Led2.On();
//				res = f_sync( &TSDProcessor::MainFile );
//			}
//		}
//	}

//	fileptr = &TSDProcessor::MainFile;
//	
//	if( disk_initialize(0) == 0 ){
//		f_mount( 0, &TSDProcessor::FATFS_Obj );
//		
//		res = f_open( fileptr, "FILE.DAT", FA_WRITE );
//		
//		if( res == FR_OK )
//			TDevice::DEVICE->DebugTools->Led2.On();
//		
//		fileptr->clust = fileptr->sclust;
//		
//		sect = clust2sect( &TSDProcessor::FATFS_Obj, fileptr->clust );
//		
//		static int dx=0;
//		while( 1 )
//		{
//			if( disk_write(0, _buf, sect, 1) != 0 )
//				__no_operation();
//			
//			_buf[3] = (uint8_t)dx;
//			_buf[2] = (uint8_t)(dx>>8);
//			_buf[1] = (uint8_t)(dx>>16);
//			_buf[0] = (uint8_t)(dx>>24);
//			
//			dx++;
//			sect++;
//		}
////		if( disk_read( 0, _buf, sect, 1 ) != 0 ){
////			__no_operation();
////		}
//	}
	
	while( 1 ){
	}
#endif /* BOOTLOADER */
}



/*----------------------------------------------------------------------------------*/
/* @brief	Обработка ошибок использования StdPeriphDriver							*/
/* @param	Not used																*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
#ifdef USE_FULL_ASSERT
void assert_failed( uint8_t* file, uint32_t line )
{
//	while( 1 ){
		__no_operation();
//		ITM_EVENT8_WITH_PC( 2, 0xAF );
//	}
}
#endif /* USE_FULL_ASSERT */
