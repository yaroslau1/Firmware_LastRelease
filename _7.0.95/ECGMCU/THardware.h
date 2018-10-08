/*----------------------------------------------------------------------------------*/
#ifndef HARDWARE_H
#define HARDWARE_H
/*----------------------------------------------------------------------------------*/
#include "main.h"
/*----------------------------------------------------------------------------------*/
// Axelerometer pinout
#define Axel_GPIO_CLK			RCC_AHBPeriph_GPIOA
#define Axel_GPIO_PORT			GPIOA
#define Axel_X_GPIO_PIN			GPIO_Pin_0
#define Axel_Y_GPIO_PIN			GPIO_Pin_1
#define Axel_Z_GPIO_PIN			GPIO_Pin_2
#define Axel_X_ADC_Channel		ADC_Channel_0
#define Axel_Y_ADC_Channel		ADC_Channel_1
#define Axel_Z_ADC_Channel		ADC_Channel_2
#define Axel_EN_GPIO_PIN		GPIO_Pin_15


// ECG pinout
#define ADS1298_GPIO_RCC_ENABLE		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE)
#define ADS1298_SPI_RCC_ENABLE		RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE )
#define ADS1298_CS_PORT			GPIOB
#define ADS1298_CS_PIN			GPIO_Pin_12
#define ADS1298_SPI_PORT		GPIOB
#define ADS1298_SCK_PIN			GPIO_Pin_13
#define ADS1298_SCK_PINSOURCE		GPIO_PinSource13
#define ADS1298_MISO_PIN		GPIO_Pin_14
#define ADS1298_MISO_PINSOURCE		GPIO_PinSource14
#define ADS1298_MOSI_PIN		GPIO_Pin_15
#define ADS1298_MOSI_PINSOURCE		GPIO_PinSource15
#define ADS1298_SPI			SPI2
#define ADS1298_SPI_AF			GPIO_AF_0
#define ADS1298_RESET_PORT		GPIOB
#define ADS1298_RESET_PIN		GPIO_Pin_11
#define ADS1298_DRDY_PORT		GPIOB
#define ADS1298_DRDY_PIN		GPIO_Pin_2
#define ADS1298_START_PORT		GPIOB
#define ADS1298_START_PIN		GPIO_Pin_10
#define ADS_DRDY_EXTI_PORT		EXTI_PortSourceGPIOB
#define ADS_DRDY_EXTI_PIN		GPIO_PinSource2 
#define ADS_DRDY_EXTI_LINE		EXTI_Line2 
#define ADS_DRDY_IRQChannel		EXTI2_3_IRQn 

#define ADS1292_GPIO_RCC_ENABLE		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE)
#define ADS1292_CS_PORT			GPIOA
#define ADS1292_CS_PIN			GPIO_Pin_10
#define ADS1292_RESET_PORT		GPIOA
#define ADS1292_RESET_PIN		GPIO_Pin_11

#define EEPROM_CS_PORT			GPIOA
#define EEPROM_CS_PIN			GPIO_Pin_9

/*
// NiBat pinout
	#define NiBat_GPIO_CLK			RCC_APB2Periph_GPIOB
	#define NiBat_GPIO_PORT			GPIOB
	#define NiBat_GPIO_PIN			GPIO_Pin_0
	#define NiBat_ADC_Channel		ADC_Channel_8


// LEDs, Test pin, etc pinout
*/

// ADS pinout

#define _SET_PIN( x, y )	GPIO_SetBits( x ## _PORT, y ## _PIN )
#define _CLR_PIN( x, y )	GPIO_ResetBits( x ## _PORT, y ## _PIN )

#define SET_PIN( z ) 		_SET_PIN( z ##, z ## )
#define CLR_PIN( z ) 		_CLR_PIN( z ##, z ## )
/*----------------------------------------------------------------------------------*/
typedef struct
{
	void( *Enable )( void );
	void( *Disable )( void );
	bool Status;

	struct {
		void( *On )( void );
		void( *Off )( void );
	}Led1;
	struct {
		void( *On )( void );
		void( *Off )( void );
	}Led2;
	struct {
		void( *On )( void );
		void( *Off )( void );
	}Led3;
	struct {
		void( *On )( void );
		void( *Off )( void );
	}Led4;

	struct {
		void( *On )( void );
		void( *Off )( void );
	}Led;

}DebugTools_TypeDef;

enum KeyDefs {
	ChangeButton = 0,
	SelectButton
};
/*----------------------------------------------------------------------------------*/
class THardware
{
	public:
		static void KeysInit( void );
		static void KeysDeinit( void );
		static void TestPinInit( void );
		static void TestPinDeinit( void );

		static void LedOn( void );
		static void LedOff( void );
		static void LedInvert( void );

		static DebugTools_TypeDef DebugTools;
    
		static bool BlockKeys;
    
		static volatile uint16_t ADC3_ScanResult[4];
 	


#ifdef EV_BOARD
		static void Led0_On( void );
		static void Led0_Off( void );
		static void Led1_On( void );
		static void Led1_Off( void );
		static void Led2_On( void );
		static void Led2_Off( void );
		static void Led3_On8( void );
		static void Led3_Off( void );
#endif

		static void BT_ResetControl( bool state );
		static int CheckKey( int _Key );
		static void LedsPHYInit( void );
		static void RS232PHYInit( void );
		static void VoltmeterInit( void );
		static void VoltmeterDeinit( void );

#ifndef	EV_BOARD
		static void TestPinControl( bool state );
#endif
};
/*----------------------------------------------------------------------------------*/
#endif