/*----------------------------------------------------------------------------------*/
#include "main.h"
#include "THardware.h"
#include "TSoftware.h"
#include "TLcd.h"
#include "TAppProcessor.h"

#include "TPCConnection.h"

#include "integer.h"
#include "ff.h"
#include "diskio.h"
#include "sd_sdio.h"
/*----------------------------------------------------------------------------------*/
void vApplicationIdleHook( void )
{
#ifdef EV_BOARD
	THardware::Led1Control( 0 );	// показывает загрузу ЦП
#endif
	__WFI(); // Enter idle mode
}



FATFS FATFS_Obj;
FIL myfile;
FRESULT resul;
UINT s;
uint8_t w_buffer[512], r_buffer[512];
DRESULT st;

/*----------------------------------------------------------------------------------*/
/*										__        									*/
/*						  _____ _____  |__| ____  									*/
/*						 /     \\__  \ |  |/    \ 									*/
/*						|  Y Y  \/ __ \|  |   |  \									*/
/*						|__|_|  (____  /__|___|  /									*/
/*							  \/     \/        \/ 									*/
/*								  													*/
/*----------------------------------------------------------------------------------*/
int main( void )
{
//THardware::AllPHYInit();
//	
//	TSoftware::AllModulesInit();
//	
//	// задача запуска
//	xTaskCreate( TAppProcessor::TASK_Startup, "Start", configMINIMAL_STACK_SIZE+128, NULL, 
//		tskIDLE_PRIORITY+1, NULL );
//	
//	// задача обновление экана
//	xTaskCreate( TLcd::TASK_Invalidate, "Inval", configMINIMAL_STACK_SIZE+32, NULL,
//		tskIDLE_PRIORITY+1, &TLcd::xLcdInvalidate );
//
//	// задача сканирования клавиатуры
//	xTaskCreate( TAppProcessor::TASK_KeyProcessor, (signed char *)"Key", configMINIMAL_STACK_SIZE, NULL, 
//		tskIDLE_PRIORITY, NULL );
	
	volatile static SD_Error Status;
//	static int address = 0x0;

	
	for( int i=0; i<512; i++ ){
		w_buffer[ i ] = 0x53;
		r_buffer[ i ] = 0x0;
	}
	
	Status = SD_Init(); // Initialization Step as described in section A
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// SDIO Interrupt ENABLE
	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
		
	resul = f_mount( 0, &FATFS_Obj );
	
	resul = f_open( &myfile, "file2.txt", FA_CREATE_ALWAYS | FA_WRITE );
	
//	f_puts( "creating and writing ok if you see this\r", &myfile );
	
	resul = f_write( &myfile, w_buffer, 512, &s );
	
//	st = disk_write( 0, 
	
	resul = f_close( &myfile );
	
	
	while( 1 ){

	}
	
	return 0;
}



/*----------------------------------------------------------------------------------*/
/* обработка ошибок испольщования StdPeriphDriver									*/
/*----------------------------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT
void assert_failed( uint8_t* file, uint32_t line )
{
	while( 1 ){
		__no_operation();
	}
}
#endif
//------------------------------------------------------------------------------------
