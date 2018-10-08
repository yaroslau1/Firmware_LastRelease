#pragma once

#ifndef LCD_H
#define LCD_H
/*----------------------------------------------------------------------------------*/
#include "main.h"
#include "THardware.h"
#include "Utilities.h"
/*----------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/
/* Display instruction */
/*----------------------------------------------------------------------------*/
#define TFT_NOP                       0x00 // No Operation
#define TFT_SWRESET                   0x01 // Software Reset
#define TFT_RDDID                     0x04 // Read Display ID
#define TFT_RDDST                     0x09 // Read Display Status
#define TFT_RDDPM                     0x0A // Read Display Power Mode
#define TFT_RDD_MADCTL                0x0B // Read Display MADCTL
#define TFT_RDD_COLMOD                0x0C // Read Display Pixel Format
#define TFT_RDDIM                     0x0D // Read Display Image Mode
#define TFT_RDDSM                     0x0E // Read Display Signal Mode
#define TFT_RDDSDR                    0x0F // Read Display Self Diagnostic Result
#define TFT_SLPIN                     0x10 // Sleep In
#define TFT_SLPOUT                    0x11 // Sleep Out
#define TFT_PTLON                     0x12 // Partial Display Mode On
#define TFT_NORON                     0x13 // Normal Display Mode On
#define TFT_INVOFF                    0x20 // Display Inversion off
#define TFT_INVON                     0x21 // Display Inversion on
#define TFT_GAMSET                    0x26 // Gamma Set
#define TFT_DISPOFF                   0x28 // Display Off
#define TFT_DISPON                    0x29 // Display On
#define TFT_CASET                     0x2A // Column Address Set
#define TFT_RASET                     0x2B // Row Address Set
#define TFT_RAMWR                     0x2C // Memory Write

#define TFT_COLSET                    0x2D // Colour Set

#define TFT_RAMRD                     0x2E // Memory Read
#define TFT_PTLAR                     0x30 // Partial Area
#define TFT_VSCRDEF                   0x33 // Vertical Scrolling Definition
#define TFT_TEOFF                     0x34 // Tearing Effect Line Off
#define TFT_TEON                      0x35 // Tearing Effect Line On
#define TFT_MADCTL                    0x36 // Memory Data Access Control
#define TFT_VSCRSADD                  0x37 // Vertical scrolling start Address of RAM
#define TFT_IDMOFF                    0x38 // Idle Mode Off
#define TFT_IDMON                     0x39 // Idle Mode On
#define TFT_COLMOD                    0x3A // Interface Pixel Format
#define TFT_RDID1                     0xDA // Read ID1
#define TFT_RDID2                     0xDB // Read ID2
#define TFT_RDID3                     0xDC // Read ID3

/*----------------------------------------------------------------------------------*/
/* Display pinout */
/*----------------------------------------------------------------------------------*/
#define DISP_GPIO_RCC_ENABLE	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE)
#define DISP_RD_PORT		GPIOA
#define DISP_RD_PIN		GPIO_Pin_10
#define DISP_WR_PORT		GPIOA
#define DISP_WR_PIN		GPIO_Pin_11
#define DISP_CS_PORT		GPIOA
#define DISP_CS_PIN		GPIO_Pin_12
#define DISP_DC_PORT		GPIOA
#define DISP_DC_PIN		GPIO_Pin_9
#define DISP_RES_PORT		GPIOC
#define DISP_RES_PIN		GPIO_Pin_4
#define DISP_D0_PORT		GPIOA
#define DISP_D0_PIN		GPIO_Pin_0
#define DISP_D1_PORT		GPIOA
#define DISP_D1_PIN		GPIO_Pin_1
#define DISP_D2_PORT		GPIOA
#define DISP_D2_PIN		GPIO_Pin_2
#define DISP_D3_PORT		GPIOA
#define DISP_D3_PIN		GPIO_Pin_3
#define DISP_D4_PORT		GPIOA
#define DISP_D4_PIN		GPIO_Pin_4
#define DISP_D5_PORT		GPIOA
#define DISP_D5_PIN		GPIO_Pin_5
#define DISP_D6_PORT		GPIOA
#define DISP_D6_PIN		GPIO_Pin_6
#define DISP_D7_PORT		GPIOA
#define DISP_D7_PIN		GPIO_Pin_7
#define DISP_EN_PORT		GPIOA
#define DISP_EN_PIN		GPIO_Pin_15
#define DISP_BL_PORT		GPIOC
#define DISP_BL_PIN		GPIO_Pin_6

#ifdef DISPLAY_GPIO
#define DISP_DATA_PORT		GPIOA
#endif

/*----------------------------------------------------------------------------------*/
#ifdef TFT
	#define clBlack			0x0000
	#define clBlue			0x001F
	#define clGreen			0x07E0
	#define clAqua			0x07FF
	#define clRed			  0xF800
	#define clFuchsia		0xF81F
	#define clYellow		0xFFE0
	#define clWhite			0xFFFF
	#define clCardian		0xFBE0
	#define TFT_COLS		320
	#define TFT_ROWS		240
#endif
/*----------------------------------------------------------------------------------*/
#define lcdSendByte( byte ) \
{ \
	uint8_t* odrptr = (uint8_t*) &DISP_WR_PORT->ODR; \
	DISP_WR_PORT->BSRRH = DISP_WR_PIN; \
	*odrptr = byte; \
	DISP_WR_PORT->BSRRL = DISP_WR_PIN; \
} \

typedef struct
{
	uint32_t biArrSize;	// size of biArr in bytes
	uint16_t biWidth;	// bitmap width in pixels
	uint16_t biHeight;	// bitmap height in pixels
	uint8_t biPixelBit;	// number of bits per pixel (must be set to 1, 2, 4, 8 or 16)
	uint8_t biColorBit;	// number of bits per color in palette (must be set to 0 (if palette not used), 12, 16, 18 or 24)
	uint8_t biCompression;	// compression method being used (must be set to 0 (if compression not used) or 1 (if RLE8 compression are used))
	const uint16_t *biPal;
	const uint8_t *biArr;
} TBitMap;


class TLcd : public THardware
{
	public:
		static void LcdPHYInit( void );
		static void LcdPHYDeInit( void );
		static void LCD_Init( void );

		static void LcdEnable( bool state );
		static void LcdBL( bool state );
		static bool LcdOff;

		static void LcdClear();
		static void PutPixel( uint16_t x, uint16_t y, uint16_t color );
//		static void Line( int x1, int y1, int x2, int y2, int action );
		static void FillRectangle( uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint16_t color );
		static void PutBitmap(uint16_t x, uint16_t y, const TBitMap *_bmp);


		static void SetSysPal(uint16_t back, uint16_t front);
    static void ExchSysPal(void);
		static void ColorInversion( bool state );

        



	private:

		static void RDPin( bool state );
		static void WRPin( bool state );
		static void CSPin( bool state );
		static void DCPin( bool state );
		static void RESPin( bool state );
//		static uint8_t LcdReadData();

		static void ReadData( uint8_t cmd, uint8_t data_size, uint8_t *data);
		static uint8_t lcdReciveByte(void);
    
		static inline void WriteCmd( uint8_t cmd, uint8_t data )
		{
			DISP_DC_PORT->BSRRH = DISP_DC_PIN;
			DISP_CS_PORT->BSRRH = DISP_CS_PIN;
			lcdSendByte( cmd );
			DISP_DC_PORT->BSRRL = DISP_DC_PIN;
			lcdSendByte( data );
			DISP_CS_PORT->BSRRL = DISP_DC_PIN;
		}

		static inline void WriteCmd( uint8_t cmd )
		{
			DISP_DC_PORT->BSRRH = DISP_DC_PIN;
			DISP_CS_PORT->BSRRH = DISP_CS_PIN;
			lcdSendByte( cmd );
			DISP_CS_PORT->BSRRL = DISP_DC_PIN;
		}


		static inline void WriteData( uint8_t data )
		{
			DISP_DC_PORT->BSRRL = DISP_DC_PIN;
			DISP_CS_PORT->BSRRH = DISP_CS_PIN;
			lcdSendByte( data );
			DISP_CS_PORT->BSRRL = DISP_DC_PIN;
		}


		static inline void WriteData( uint16_t data )
		{
			DISP_DC_PORT->BSRRL = DISP_DC_PIN;
			DISP_CS_PORT->BSRRH = DISP_CS_PIN;
			lcdSendByte( (uint8_t) (data & 0xFF) );
			lcdSendByte( (uint8_t) ((data >> 8) & 0xFF) );
			DISP_CS_PORT->BSRRL = DISP_DC_PIN;
		}

        
		static inline void WriteData( uint32_t data )
		{
			DISP_DC_PORT->BSRRL = DISP_DC_PIN;
			DISP_CS_PORT->BSRRH = DISP_CS_PIN;
			lcdSendByte( (uint8_t) (data & 0xFF) );
			lcdSendByte( (uint8_t) ((data >> 8) & 0xFF) );
			lcdSendByte( (uint8_t) ((data >> 16) & 0xFF) );
			lcdSendByte( (uint8_t) ((data >> 24) & 0xFF) );
			DISP_CS_PORT->BSRRL = DISP_DC_PIN;
		}

		static void SetPos( uint16_t row, uint16_t col );
		static void SetPos( uint16_t row, uint16_t col, uint16_t height, uint16_t width );

	protected:
};
/*----------------------------------------------------------------------------------*/
#endif