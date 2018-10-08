//---------------------------------------------------------------------------
#include "main.h"
#include "THardware.h"
#include "TSoftware.h"
#include "TLcd.h"
#include "TGui.h"
#include "TADS1298.h"
#include "TPCConnection.h"
#include "Pinout.h"
//--------------------------------------------------------------------------
void vApplicationIdleHook( void )
{
#ifdef EV_BOARD
	THardware::Led1Control( 0 );	// показывает загрузу ЦП
#endif
	__WFI(); //Enter idle mode
}
//---------------------------------------------------------------------------

void test_lcd( void *pvParameters )
{
	vTaskDelay( 10 / portTICK_RATE_MS );
	while( 1 ){
		vTaskDelay( 500 / portTICK_RATE_MS );
	//	TGui::GUI_PutBitmap( 0, 0, &_iMainGui_BMP, LCD_PIXEL_ON );
	//	TGui::DrawEcg1();
	//	TLcd::PutPixel( 0, 0, LCD_PIXEL_ON );
		static int s = 0;
	
		if( s == 0 ){
			TGui::GUI_PutBitmap( 0, 0, &_iMainGui_BMP, LCD_PIXEL_ON );
			s = 1;
		}
		if( s == 1 ){
			TGui::DrawEcg1();
		//	TGui::DrawEcg2();
		}
	
//		vTaskResume(TLcd::xLcdInvalidate);
//		TGui::GUI_PutBitmap( 0, 0, &_tbECG_BMP, LCD_PIXEL_ON );
//		TGui::GUI_DrawText( 0, 54, "Loading data...", 127, algLeft, fntArial9, LCD_PIXEL_ON );	
//		TGui::GUI_ClearFillRectangle( 0, 0, 127, 63, LCD_PIXEL_ON);	
//		vTaskDelete( NULL );
	}
}
//---------------------------------------------------------------------------

int main( void )
{
	THardware::AllPHYInit();
	TSoftware::AllModulesInit();
	
		
/*
	while( 1 ){
		TPCConnection::USART_SendByte( TADS1298::RegisterRead( ADS1298_REG_ID ));
		TPCConnection::USART_SendByte( TADS1298::RegisterRead( ADS1298_REG_CONFIG3 ));
			
		__delay_cycles( 1000000 );
	}
*/
	
//	TADS1298::StartConversation();
		
//	xTaskCreate( TADS1298::TASK_EcgDataReceive, (signed char *) "ads", configMINIMAL_STACK_SIZE+256, 
//		NULL, tskIDLE_PRIORITY+2, &TADS1298::xADS1298Handle );
	
	xTaskCreate( test_lcd, (signed char *) "lcd", configMINIMAL_STACK_SIZE, NULL,
		tskIDLE_PRIORITY, NULL );
	
	// задача обновление экана	
	xTaskCreate( TLcd::TASK_Invalidate, (signed char *) "Inval", configMINIMAL_STACK_SIZE + 32, NULL,
		tskIDLE_PRIORITY, &TLcd::xLcdInvalidate );

	vTaskStartScheduler();
	
  return 0;
}
//---------------------------------------------------------------------------

#ifdef  USE_FULL_ASSERT

// обработка ошибок испольщования StdPeriphDriver
void assert_failed( uint8_t* file, uint32_t line )
{ 
	
  while ( 1 )
  {
  }
}
#endif
//---------------------------------------------------------------------------
