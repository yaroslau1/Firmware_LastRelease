#ifndef TSYSTEM_H
#define TSYSTEM_H
/*----------------------------------------------------------------------------------*/
#include "THardware.h"
#include "TEcgProcessor.h"
/*----------------------------------------------------------------------------------*/

typedef struct
{
	struct{
		void( *Enable )( void );
		void( *Disable )( void );
		bool Status;
	}Lcd;

	struct{
		void( *Enable )( void );
		void( *Disable )( void );
		bool Status;
	}BattMeter;

	struct{
		void( *Enable )( void );
		void( *Disable )( void );
		bool Status;
	}Bluetooth;

	struct{
		void( *Enable )( void );
		void( *Disable )( void );
		bool Status;
	}Keyboard;

	struct{
		void( *Enable )( void );
		void( *Disable )( void );
		bool Status;
	}Ecg;

	struct{
		void( *Enable )( void );
		void( *Disable )( void );
		bool Status;
	}SD;

	Hartbeat_TypeDef *Hartbeat;

	DebugTools_TypeDef* DebugTools;

}DEVICE_TypeDef;



/*----------------------------------------------------------------------------------*/
class TDevice : public THardware
{
	public:
		static void SystemLowLevelInit( void );
		static void LcdEnable( void );
		static void LcdDisable( void );
		static void BattMeterEnable( void );
		static void BattMeterDisable( void );
		static void KeyboardEnable( void );
		static void KeyboardDisable( void );
		static void BluetoothEnable( void );
		static void BluetoothDisable( void );
		static void DebugToolsEnable( void );
		static void DebugToolsDisable( void );
		static void EcgEnable( void );
		static void EcgDisable( void );
		static void SDEnable( void );
		static void SDDisable( void );

		static void SystemReset();

		static DEVICE_TypeDef DEVICE_Obj;
		static DEVICE_TypeDef *DEVICE;
};
#endif