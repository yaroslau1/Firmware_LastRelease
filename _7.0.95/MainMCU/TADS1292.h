#ifndef TADS1292_H
#define TADS1292_H
/*----------------------------------------------------------------------------------*/
#include "main.h"
#include "THardware.h"
#include "TDSP.h"
/*----------------------------------------------------------------------------------*/
#define ADS1292_OPCODE_WAKEUP		0x02
#define ADS1292_OPCODE_STANDBY		0x04
#define ADS1292_OPCODE_RESET		0x06
#define ADS1292_OPCODE_START		0x08
#define ADS1292_OPCODE_STOP		0x0A
#define ADS1292_OPCODE_OFFSETCAL	0x1A
#define ADS1292_OPCODE_RDATAC		0x10
#define ADS1292_OPCODE_SDATAC		0x11
#define ADS1292_OPCODE_RDATA		0x12
#define ADS1292_OPCODE_RREG		0x20
#define ADS1292_OPCODE_WREG		0x40

#define ADS1292_REG_ID			0x00
#define ADS1292_REG_CONFIG1		0x01
#define ADS1292_REG_CONFIG2		0x02
#define ADS1292_REG_LOFF		0x03
#define ADS1292_REG_CH1SET		0x04
#define ADS1292_REG_CH2SET		0x05
#define ADS1292_REG_RLD_SENS		0x06
#define ADS1292_REG_LOFF_SENS		0x07
#define ADS1292_REG_LOFF_STAT		0x08
#define ADS1292_REG_RESP1		0x09
#define ADS1292_REG_RESP2		0x0A
#define ADS1292_REG_GPIO		0x0B

/*---- Установка скорости обработки АЦП: 250, 500 ----------------------------------*/
#define ADS1292_SAMPLING_FREQUENCY	250

/*---- Делитель для нормализации по ГОСТ, на 10Гц, должно быть 1 мВ ----------------*/
#if ADS1292_SAMPLING_FREQUENCY == 250
//	#define ADS1292_NORMALIZE_DIVIDER		175		/* PGA = 2 */
//	#define ADS1292_NORMALIZE_DIVIDER		250		/* PGA = 3 */
	#define ADS1292_NORMALIZE_DIVIDER		540		/* для PGA = 6 */
#elif ADS1292_SAMPLING_FREQUENCY == 500
	#define ADS1292_NORMALIZE_DIVIDER		180
#endif



/*----------------------------------------------------------------------------------*/
class TADS1292 : public TDSP
{
	public:
		static void ADS1292_Init( void );

		static void StartConversation( void );
		static void StopConversation( void );
		static void TestSignals( void );
		static void StandBy();
		static void WakeUp();

		static void GetConvertResult( void );

		static void ADS1292PHYInit( void );

		static void SPI_Reset( bool state );
	private:
		static void SPI_SSControl( bool state );
		static void SPI_SendByte( uint8_t byte );
		static uint8_t SPI_ReceiveByte( uint8_t data );

		static void SetOpcode( uint8_t opcode );
		static uint8_t RegisterRead( uint8_t Register );
		static uint8_t DataRead( void );
		static void RegisterWrite( uint8_t Register, uint8_t Data );

	protected:
};

#endif