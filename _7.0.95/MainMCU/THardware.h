/*----------------------------------------------------------------------------------*/
#ifndef HARDWARE_H
#define HARDWARE_H
/*----------------------------------------------------------------------------------*/
#include "main.h"
/*----------------------------------------------------------------------------------*/

// SDCard pinout
#define SD_GPIO_RCC_ENABLE		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE)
#define SD_SPI_RCC_ENABLE		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE)
#define SD_SPI_RCC_DISABLE		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE)
#define SD_EN_PORT			GPIOB
#define SD_EN_PIN			GPIO_Pin_8
#define SD_CS_PORT			GPIOC
#define SD_CS_PIN			GPIO_Pin_11
#define SD_SCK_PORT			GPIOB
#define SD_SCK_PIN			GPIO_Pin_3
#define SD_SCK_PINSOURCE		GPIO_PinSource3
#define SD_MISO_PORT			GPIOB
#define SD_MISO_PIN			GPIO_Pin_4
#define SD_MISO_PINSOURCE		GPIO_PinSource4
#define SD_MOSI_PORT			GPIOB
#define SD_MOSI_PIN			GPIO_Pin_5
#define SD_MOSI_PINSOURCE		GPIO_PinSource5
#define SD_SPI				SPI1
#define SD_SPI_AF			GPIO_AF_SPI1

// Bluetooth pinout
#define LMX_USART_RCC_ENABLE		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)
#define LMX_USART_RCC_DISABLE		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE)
#define LMX_GPIO_RCC_ENABLE		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE)
#define LMX_TXD_GPIO_PORT    		GPIOB
#define LMX_TXD_GPIO_PIN     		GPIO_Pin_6
#define LMX_TXD_GPIO_PINSOURCE 		GPIO_PinSource6
#define LMX_RXD_GPIO_PORT    		GPIOB
#define LMX_RXD_GPIO_PIN     		GPIO_Pin_7
#define LMX_RXD_GPIO_PINSOURCE 		GPIO_PinSource7
#define LMX_EN_GPIO_PORT		GPIOC
#define LMX_EN_GPIO_PIN			GPIO_Pin_13
#define LMX_RST_GPIO_PORT		GPIOC
#define LMX_RST_GPIO_PIN		GPIO_Pin_14
#define LMX_LSTAT_GPIO_PORT		GPIOB
#define LMX_LSTAT_GPIO_PIN		GPIO_Pin_9
#define LMX_LSTAT_EXTI_PORT		EXTI_PortSourceGPIOB
#define LMX_LSTAT_EXTI_PIN		EXTI_PinSource9
#define LMX_LSTAT_EXTI_LINE		EXTI_Line9
#define LMX_LSTAT_IRQChannel		EXTI9_5_IRQn 
#define LMX_USART_IRQChannel		USART1_IRQn 
#define LMX_USART			USART1
#define LMX_USART_AF     		GPIO_AF_USART1

// ADS pinout
#define ADS1298_GPIO_RCC_ENABLE		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE)
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
#define ADS1298_SPI_AF			GPIO_AF_SPI2
#define ADS1298_DRDY_PORT		GPIOB
#define ADS1298_DRDY_PIN		GPIO_Pin_2
#define ADS_DRDY_EXTI_PORT		EXTI_PortSourceGPIOB
#define ADS_DRDY_EXTI_PIN		GPIO_PinSource2 
#define ADS_DRDY_EXTI_LINE		EXTI_Line2 
#define ADS_DRDY_IRQChannel		EXTI2_IRQn 

// Keys pinout
#define UB1_BUTTON_CLK			RCC_AHB1Periph_GPIOC
#define UB1_BUTTON_PORT          	GPIOC
#define UB1_BUTTON_PIN           	GPIO_Pin_1
#define UB1_EXTI_PORT                  EXTI_PortSourceGPIOC
#define UB1_EXTI_PIN                   EXTI_PinSource1 
#define UB1_EXTI_LINE                  EXTI_Line1 
#define UB1_IRQChannel                 EXTI1_IRQn 

#define UB2_BUTTON_CLK			RCC_AHB1Periph_GPIOC
#define UB2_BUTTON_PORT          	GPIOC
#define UB2_BUTTON_PIN           	GPIO_Pin_0
#define UB2_EXTI_PORT			EXTI_PortSourceGPIOC
#define UB2_EXTI_PIN			EXTI_PinSource0 
#define UB2_EXTI_LINE			EXTI_Line0 
#define UB2_IRQChannel                 EXTI0_IRQn 

// NiBat pinout
	#define NiBat_GPIO_CLK			RCC_APB2Periph_GPIOB
	#define NiBat_GPIO_PORT			GPIOB
	#define NiBat_GPIO_PIN			GPIO_Pin_0
	#define NiBat_ADC_Channel		ADC_Channel_8

// Audio pinout
#define AudioIn_GPIO_CLK		RCC_AHB1Periph_GPIOC
#define AudioIn_GPIO_PORT		GPIOC
#define AudioIn_GPIO_PIN		GPIO_Pin_2
#define AudioIn_ADC_Channel		ADC_Channel_12

#define AudioPwr_GPIO_CLK		RCC_AHB1Periph_GPIOC
#define AudioPwr_GPIO_PORT		GPIOC
#define AudioPwr_GPIO_PIN		GPIO_Pin_3

// LEDs, Test pin, etc pinout
#define TEST_GPIO_CLK			RCC_AHB1Periph_GPIOC
#define TEST_PIN			GPIO_Pin_5
#define TEST_PORT			GPIOC
//	#define WAKEUP_GPIO_PORT     		GPIOE
//	#define WAKEUP_GPIO_PIN      		GPIO_Pin_2

#define BUZZER_P_GPIO_CLK		RCC_AHB1Periph_GPIOC
#define BUZZER_P_GPIO_PORT	GPIOC
#define BUZZER_P_GPIO_PIN		GPIO_Pin_7
#define BUZZER_P_GPIO_PINSOURCE		GPIO_PinSource7

#define BUZZER_N_GPIO_CLK		RCC_AHB1Periph_GPIOB
#define BUZZER_N_GPIO_PORT	GPIOB
#define BUZZER_N_GPIO_PIN		GPIO_Pin_0
#define BUZZER_N_GPIO_PINSOURCE		GPIO_PinSource0

#define BUZZER_TIM			    TIM8
#define BUZZER_TIM_AF			  GPIO_AF_TIM8
#define BUZZER_TIM_CLK			RCC_APB2Periph_TIM8

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