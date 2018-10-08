/************************************************************************************/
/* Title:		Device units control 												*/
/* Author:		Cyber Genius														*/
/* Version:																			*/
/* Date:																			*/
/************************************************************************************/
/*																					*/
/************************************************************************************/
#include "TDevice.h"
#include "TLcd.h"
#include "TLMX9838.h"
#include "TADS1298.h"
#include "TADS1292.h"
#include "TInterpreter.h"

/*----------------------------------------------------------------------------------*/
DEVICE_TypeDef TDevice::DEVICE_Obj;
DEVICE_TypeDef *TDevice::DEVICE;



/*----------------------------------------------------------------------------------*/
void TDevice::SystemLowLevelInit( void )
{
	DEVICE = &DEVICE_Obj;
	DEVICE->Hartbeat = &TEcgProcessor::Hartbeat;
	DEVICE->DebugTools = &THardware::DebugTools;

#ifdef EV_BOARD
	DEVICE->Lcd.Enable = LcdEnable;
	DEVICE->Lcd.Disable = LcdDisable;
	DEVICE->Lcd.Status = false;

	DEVICE->BattMeter.Enable = BattMeterEnable;
	DEVICE->BattMeter.Disable = BattMeterDisable;
	DEVICE->BattMeter.Status = false;

	DEVICE->Keyboard.Enable = KeyboardEnable;
	DEVICE->Keyboard.Disable = KeyboardDisable;
	DEVICE->Keyboard.Status = false;

	DEVICE->Bluetooth.Enable = BluetoothEnable;
	DEVICE->Bluetooth.Disable = BluetoothDisable;
	DEVICE->Bluetooth.Status = false;

	DEVICE->Ecg.Enable = EcgEnable;
	DEVICE->Ecg.Disable = EcgDisable;
	DEVICE->Ecg.Status = false;

	DEVICE->SD.Enable = SDEnable;
	DEVICE->SD.Disable = SDDisable;
	DEVICE->SD.Status = false;

	DEVICE->Hartbeat->Enable = TEcgProcessor::TimerInit;
	DEVICE->Hartbeat->Disable = TEcgProcessor::TimerDeinit;
	DEVICE->Hartbeat->Status = false;

	DEVICE->DebugTools->Enable = DebugToolsEnable;
	DEVICE->DebugTools->Led1.On = THardware::Led0_On;
	DEVICE->DebugTools->Led1.Off = THardware::Led0_Off;
	DEVICE->DebugTools->Led2.On = THardware::Led1_On;
	DEVICE->DebugTools->Led2.Off = THardware::Led1_Off;
	DEVICE->DebugTools->Led3.On = THardware::Led2_On;
	DEVICE->DebugTools->Led3.Off = THardware::Led2_Off;
	DEVICE->DebugTools->Led4.On = THardware::Led3_On;
	DEVICE->DebugTools->Led4.Off = THardware::Led3_Off;
	DEVICE->DebugTools->Led.On = NULL;
	DEVICE->DebugTools->Led.Off = NULL;
	DEVICE->DebugTools->Status = false;
#else
	DEVICE->Lcd.Enable = LcdEnable;
	DEVICE->Lcd.Disable = LcdDisable;
	DEVICE->Lcd.Status = false;

	DEVICE->BattMeter.Enable = BattMeterEnable;
	DEVICE->BattMeter.Disable = BattMeterDisable;
	DEVICE->BattMeter.Status = false;

	DEVICE->Keyboard.Enable = KeyboardEnable;
	DEVICE->Keyboard.Disable = KeyboardDisable;
	DEVICE->Keyboard.Status = false;

	DEVICE->Bluetooth.Enable = BluetoothEnable;
	DEVICE->Bluetooth.Disable = BluetoothDisable;
	DEVICE->Bluetooth.Status = false;

	DEVICE->Ecg.Enable = EcgEnable;
	DEVICE->Ecg.Disable = EcgDisable;
	DEVICE->Ecg.Status = false;

	DEVICE->SD.Enable = SDEnable;
	DEVICE->SD.Disable = SDDisable;
	DEVICE->SD.Status = false;

	DEVICE->Hartbeat->Enable = TEcgProcessor::TimerInit;
	DEVICE->Hartbeat->Disable = TEcgProcessor::TimerDeinit;
	DEVICE->Hartbeat->Status = false;

	DEVICE->DebugTools->Enable = DebugToolsEnable;
	DEVICE->DebugTools->Disable = DebugToolsDisable;
	DEVICE->DebugTools->Led.On = THardware::LedOn;
	DEVICE->DebugTools->Led.Off = THardware::LedOff;
	DEVICE->DebugTools->Status = false;
#endif
}



/*----------------------------------------------------------------------------------*/
void TDevice::LcdEnable( void )
{
	TLcd::LcdPHYInit();
//	TLcd::LcdOff = false;
	TLcd::LcdEnable( true );
	TLcd::LCD_Init();
	TLcd::LcdOff = false;
	DEVICE->Lcd.Status = true;
}



/*----------------------------------------------------------------------------------*/
void TDevice::LcdDisable( void )
{
  TLcd::LcdOff = true;
	TLcd::LcdPHYDeInit();
	DEVICE->Lcd.Status = false;
}



/*----------------------------------------------------------------------------------*/
void TDevice::BattMeterEnable( void )
{
	THardware::VoltmeterInit();
	DEVICE->BattMeter.Status = true;
}



/*----------------------------------------------------------------------------------*/
void TDevice::BattMeterDisable( void )
{
	THardware::VoltmeterDeinit();
	DEVICE->BattMeter.Status = false;
}



/*----------------------------------------------------------------------------------*/
void TDevice::KeyboardEnable( void )
{
	THardware::KeysInit();
	DEVICE->Keyboard.Status = true;
}



/*----------------------------------------------------------------------------------*/
void TDevice::KeyboardDisable( void )
{
	THardware::KeysDeinit();
	DEVICE->Keyboard.Status = false;
}



/*----------------------------------------------------------------------------------*/
void TDevice::BluetoothEnable( void )
{
	TLMX9838::BluetoothPHYInit();
	DEVICE->Bluetooth.Status = true;
}



/*----------------------------------------------------------------------------------*/
void TDevice::BluetoothDisable( void )
{
	TLMX9838::BluetoothPHYDeinit();
	DEVICE->Bluetooth.Status = false;
}



/*----------------------------------------------------------------------------------*/
void TDevice::EcgEnable( void )
{

	TADS1298::ADS1298PHYInit();
	TADS1298::ADS1298_Init();
#ifdef USE_ADS1292R
	TADS1292::ADS1292PHYInit();
	TADS1292::ADS1292_Init();
#endif //USE_ADS1292R

	DEVICE->Ecg.Status = true;
}



/*----------------------------------------------------------------------------------*/
void TDevice::EcgDisable( void )
{
	TADS1298::ADS1298PHYInit();
//	TADS1298::ADS1298_Deinit();
	DEVICE->Ecg.Status = false;
}



/*----------------------------------------------------------------------------------*/
void TDevice::SDEnable( void )
{
//	NVIC_InitTypeDef NVIC_InitStructure;
//
//	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init( &NVIC_InitStructure );

	DEVICE->SD.Status = true;
}



/*----------------------------------------------------------------------------------*/
void TDevice::SDDisable( void )
{
//	NVIC_InitTypeDef NVIC_InitStructure;
//
//	NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
//	NVIC_Init( &NVIC_InitStructure );

	DEVICE->SD.Status = false;
}



/*----------------------------------------------------------------------------------*/
void TDevice::DebugToolsEnable( void )
{
#ifdef EV_BOARD
	THardware::LedsPHYInit();
#else
	THardware::TestPinInit();
#endif
	DEVICE->DebugTools->Status = true;
}



/*----------------------------------------------------------------------------------*/
void TDevice::DebugToolsDisable( void )
{
#ifdef EV_BOARD

#else
	THardware::TestPinDeinit();
#endif
	DEVICE->DebugTools->Status = false;
}
/*----------------------------------------------------------------------------------*/

void TDevice::SystemReset()
{

  __delay_cycles(600000);
//vTaskDelay(100/portTICK_RATE_MS);	
  TInterpreter::SendFrameToHost(CMD_RST);
  __delay_cycles(600000);
//  vTaskDelay(1000/portTICK_RATE_MS);	
  NVIC_SystemReset();

	while(1) 
	{}	
}
/*----------------------------------------------------------------------------------*/


