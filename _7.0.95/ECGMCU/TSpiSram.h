#ifndef TSPISRAM_H
#define TSPISRAM_H
/*----------------------------------------------------------------------------------*/
#include "main.h"
#include "TSpiSramHardware.h"
/*----------------------------------------------------------------------------------*/

#ifdef SPISRAM_LOWSIDE
  #define SPISRAM_PIN_PULL		GPIO_PuPd_NOPULL
#else
  #define SPISRAM_PIN_PULL		GPIO_PuPd_UP
#endif


#define SPISRAM_BYTEMode		0x00
#define SPISRAM_PAGEMode		0x80
#define SPISRAM_SEQMode			0x40

#define SPISRAM_READ			0x03
#define SPISRAM_WRITE			0x02
#define SPISRAM_EDIO			0x3B
#define SPISRAM_EQIO			0x38
#define SPISRAM_RSTIO			0xFF
#define SPISRAM_RDMR			0x05
#define SPISRAM_WRMR			0x01

#define SPISRAM_SIZE      4096//128*1024/8



/*----------------------------------------------------------------------------------*/
class TSPISRAM
{
	public:
		static void PHYInit( void );
		static void PHYDeinit( void );
		static void PHYReinit( void );

		static void WriteByte( uint32_t addr, uint8_t data );
		static void WriteByte( uint32_t addr, uint8_t *data );
		static void WritePage( uint32_t addr, uint8_t *data );
		static void WriteSeq ( uint32_t addr, uint32_t size, uint8_t *data );

		static uint8_t ReadByte( uint32_t addr );
		static void ReadByte( uint32_t addr, uint8_t *data );
		static void ReadPage( uint32_t addr, uint8_t *data );
		static void ReadSeq ( uint32_t addr, uint32_t size, uint8_t *data );

		static void IRQControl( bool state );

private:
		static void SSControl( bool state );
		static void SendByte( uint8_t byte );
		static void SendAddr( uint32_t addr );
		static uint8_t ReceiveByte( uint8_t data );

		static void SetOpcode( uint8_t opcode );
		static uint8_t RegisterRead( uint8_t Register );
		static uint8_t DataRead( void );
		static void RegisterWrite( uint8_t Register, uint8_t Data );
		static DMA_InitTypeDef DMA_InitStructure;

	protected:
};

#endif