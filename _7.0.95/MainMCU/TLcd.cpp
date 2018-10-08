/************************************************************************************/
/* Title:		LCD lowlevel control 												*/
/* Author:		Cyber Genius														*/
/* Version:		1.0																	*/
/* Date:		11.07.2012															*/
/************************************************************************************/
/*																					*/
/************************************************************************************/

#include "TLcd.h"
#include "THardware.h"
#include "main.h"
#include "TFlash.h"
#include "TDevice.h"
#include "StnFont.h"
#include "TAppProcessor.h"
/*----------------------------------------------------------------------------------*/





uint16_t SysPal[2];

/*----------------------------------------------------------------------------------*/
bool TLcd::LcdOff = false;
/*----------------------------------------------------------------------------------*/
#ifdef DISPLAY_FSMC
static volatile uint8_t *fsmcRegister;
static volatile uint8_t *fsmcData;
static volatile uint16_t *fsmcData16;
static volatile uint32_t *fsmcData32;
#endif

/*----------------------------------------------------------------------------------*/
void TLcd::LcdPHYInit( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	DISP_GPIO_RCC_ENABLE;

	/* RES pin */
	GPIO_ResetBits( DISP_RES_PORT, DISP_RES_PIN );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_RES_PIN;
	GPIO_Init( DISP_RES_PORT, &GPIO_InitStructure );

	/* EN pin */
	LcdEnable( false );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_EN_PIN;
	GPIO_Init( DISP_EN_PORT, &GPIO_InitStructure );

	/* BL pin */
	LcdBL( false );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_BL_PIN;
	GPIO_Init( DISP_BL_PORT, &GPIO_InitStructure );

#if defined DISPLAY_GPIO

	/* RD pin */
	GPIO_SetBits( DISP_RD_PORT, DISP_RD_PIN );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_RD_PIN;
	GPIO_Init( DISP_RD_PORT, &GPIO_InitStructure );

	/* WR pin */
	GPIO_SetBits( DISP_WR_PORT, DISP_WR_PIN );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_WR_PIN;
	GPIO_Init( DISP_WR_PORT, &GPIO_InitStructure );

	/* CS pin */
	GPIO_SetBits( DISP_CS_PORT, DISP_CS_PIN );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_CS_PIN;
	GPIO_Init( DISP_CS_PORT, &GPIO_InitStructure );

	/* DC pin */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_DC_PIN;
	GPIO_Init( DISP_DC_PORT, &GPIO_InitStructure );

	/* DATA pins */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_D0_PIN;
	GPIO_Init( DISP_D0_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D1_PIN;
	GPIO_Init( DISP_D1_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D2_PIN;
	GPIO_Init( DISP_D2_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D3_PIN;
	GPIO_Init( DISP_D3_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D4_PIN;
	GPIO_Init( DISP_D4_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D5_PIN;
	GPIO_Init( DISP_D5_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D6_PIN;
	GPIO_Init( DISP_D6_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D7_PIN;
	GPIO_Init( DISP_D7_PORT, &GPIO_InitStructure );

#elif defined DISPLAY_FSMC

	/* RD pin */
	GPIO_SetBits( DISP_RD_PORT, DISP_RD_PIN );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_RD_PIN;
	GPIO_Init( DISP_RD_PORT, &GPIO_InitStructure );

	/* WR pin */
	GPIO_SetBits( DISP_WR_PORT, DISP_WR_PIN );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_WR_PIN;
	GPIO_Init( DISP_WR_PORT, &GPIO_InitStructure );

	/* CS pin */
	GPIO_SetBits( DISP_CS_PORT, DISP_CS_PIN );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_CS_PIN;
	GPIO_Init( DISP_CS_PORT, &GPIO_InitStructure );

	/* DC pin */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_DC_PIN;
	GPIO_Init( DISP_DC_PORT, &GPIO_InitStructure );

	/* DATA pins */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_D0_PIN;
	GPIO_Init( DISP_D0_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D1_PIN;
	GPIO_Init( DISP_D1_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D2_PIN;
	GPIO_Init( DISP_D2_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D3_PIN;
	GPIO_Init( DISP_D3_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D4_PIN;
	GPIO_Init( DISP_D4_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D5_PIN;
	GPIO_Init( DISP_D5_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D6_PIN;
	GPIO_Init( DISP_D6_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D7_PIN;
	GPIO_Init( DISP_D7_PORT, &GPIO_InitStructure );

	/* FSMC Setup */
	FSMC_NORSRAMTimingInitTypeDef   FSMC_TimingStructure;
	FSMC_NORSRAMInitTypeDef		FSMC_InitStructure;

	DISP_FSMC_RCC_ENABLE;

	FSMC_TimingStructure.FSMC_AddressSetupTime = 10;
	FSMC_TimingStructure.FSMC_DataSetupTime = 10;
	FSMC_TimingStructure.FSMC_AddressHoldTime = 0x00;
	FSMC_TimingStructure.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_TimingStructure.FSMC_DataLatency = 0x00;
	FSMC_TimingStructure.FSMC_AccessMode = FSMC_AccessMode_A;
	FSMC_TimingStructure.FSMC_CLKDivision=1;

	FSMC_InitStructure.FSMC_Bank=FSMC_Bank1_NORSRAM1;
	FSMC_InitStructure.FSMC_DataAddressMux=FSMC_DataAddressMux_Disable;
	FSMC_InitStructure.FSMC_MemoryType=FSMC_MemoryType_SRAM;
	FSMC_InitStructure.FSMC_MemoryDataWidth=FSMC_MemoryDataWidth_8b;
	FSMC_InitStructure.FSMC_BurstAccessMode=FSMC_BurstAccessMode_Disable;
	FSMC_InitStructure.FSMC_WaitSignalPolarity=FSMC_WaitSignalPolarity_Low;
	FSMC_InitStructure.FSMC_WrapMode=FSMC_WrapMode_Disable;
	FSMC_InitStructure.FSMC_WaitSignalActive=FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_InitStructure.FSMC_WriteOperation=FSMC_WriteOperation_Enable;
	FSMC_InitStructure.FSMC_WaitSignal=FSMC_WaitSignal_Disable;
	FSMC_InitStructure.FSMC_ExtendedMode=FSMC_ExtendedMode_Disable;
	FSMC_InitStructure.FSMC_WriteBurst=FSMC_WriteBurst_Disable;
	FSMC_InitStructure.FSMC_ReadWriteTimingStruct=&FSMC_TimingStructure;
	FSMC_InitStructure.FSMC_WriteTimingStruct=&FSMC_TimingStructure;
	FSMC_InitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInit(&FSMC_InitStructure);
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);

	fsmcRegister = (uint8_t *) 0x60000000;
	fsmcData = (uint8_t *) 0x60020000;
	fsmcData16 = (uint16_t *) 0x60020000; 
	fsmcData32 = (uint32_t *) 0x60020000; 

#else
 #error "Please select GPIO or FSMC interface for display"
#endif

}



/*----------------------------------------------------------------------------------*/
void TLcd::LcdPHYDeInit( void )
{
	LcdEnable( 0 );

	GPIO_InitTypeDef GPIO_InitStructure;

	/* RD pin */
	GPIO_SetBits( DISP_RD_PORT, DISP_RD_PIN );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_RD_PIN;
	GPIO_Init( DISP_RD_PORT, &GPIO_InitStructure );

	/* WR pin */
	GPIO_SetBits( DISP_WR_PORT, DISP_WR_PIN );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_WR_PIN;
	GPIO_Init( DISP_WR_PORT, &GPIO_InitStructure );

	/* CS pin */
	GPIO_SetBits( DISP_CS_PORT, DISP_CS_PIN );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_CS_PIN;
	GPIO_Init( DISP_CS_PORT, &GPIO_InitStructure );

	/* DC pin */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_DC_PIN;
	GPIO_Init( DISP_DC_PORT, &GPIO_InitStructure );

	/* RES pin */
	GPIO_ResetBits( DISP_RES_PORT, DISP_RES_PIN );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_RES_PIN;
	GPIO_Init( DISP_RES_PORT, &GPIO_InitStructure );

	/* DATA pins */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_D0_PIN;
	GPIO_Init( DISP_D0_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D1_PIN;
	GPIO_Init( DISP_D1_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D2_PIN;
	GPIO_Init( DISP_D2_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D3_PIN;
	GPIO_Init( DISP_D3_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D4_PIN;
	GPIO_Init( DISP_D4_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D5_PIN;
	GPIO_Init( DISP_D5_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D6_PIN;
	GPIO_Init( DISP_D6_PORT, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = DISP_D7_PIN;
	GPIO_Init( DISP_D7_PORT, &GPIO_InitStructure );

	/* EN pin */
	LcdEnable( false );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_EN_PIN;
	GPIO_Init( DISP_EN_PORT, &GPIO_InitStructure );

	/* BL pin */
	LcdBL( false );
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Pin = DISP_BL_PIN;
	GPIO_Init( DISP_BL_PORT, &GPIO_InitStructure );

#if defined DISPLAY_GPIO

#elif defined DISPLAY_FSMC

	/* FSMC Setup */
	DISP_FSMC_RCC_DISABLE;

#else
 #error "Please select GPIO or FSMC interface for display"
#endif

}


#if defined DISPLAY_GPIO
/*----------------------------------------------------------------------------------*/
void TLcd::RDPin( bool state )
{
	if( state == 1 ) GPIO_SetBits( DISP_RD_PORT, DISP_RD_PIN );
	else GPIO_ResetBits( DISP_RD_PORT, DISP_RD_PIN );
}
#endif



#if defined DISPLAY_GPIO
/*----------------------------------------------------------------------------------*/
void TLcd::WRPin( bool state )
{
	if( state == 1 ) GPIO_SetBits( DISP_WR_PORT, DISP_WR_PIN );
	else GPIO_ResetBits( DISP_WR_PORT, DISP_WR_PIN );
}
#endif



/*----------------------------------------------------------------------------------*/
void TLcd::CSPin( bool state )
{
	if( state == 1 ) GPIO_SetBits( DISP_CS_PORT, DISP_CS_PIN );
	else GPIO_ResetBits( DISP_CS_PORT, DISP_CS_PIN );
}



/*----------------------------------------------------------------------------------*/
void TLcd::DCPin( bool state )
{
	if( state == 1 ) GPIO_SetBits( DISP_DC_PORT, DISP_DC_PIN );
	else GPIO_ResetBits( DISP_DC_PORT, DISP_DC_PIN );
}



/*----------------------------------------------------------------------------------*/
void TLcd::RESPin( bool state )
{
	if( state == 1 ) GPIO_SetBits( DISP_RES_PORT, DISP_RES_PIN );
	else GPIO_ResetBits( DISP_RES_PORT, DISP_RES_PIN );
}


/*----------------------------------------------------------------------------------*/
void TLcd::LcdEnable( bool state )
{
#ifndef DISPLAY_OFF
	if( state == 1 ) {
		GPIO_ResetBits( DISP_EN_PORT, DISP_EN_PIN );
	}
	else {
		GPIO_SetBits( DISP_EN_PORT, DISP_EN_PIN );
	}
#else
	GPIO_SetBits( DISP_EN_PORT, DISP_EN_PIN );
#endif
}


void TLcd::LcdBL( bool state )
{
#ifndef DISPLAY_OFF
//	if((state != 0) && (TAppProcessor::SysVoltageValue > 3000)) 
	if(state != 0) 
  {
		WriteCmd(TFT_INVOFF);
		GPIO_SetBits( DISP_BL_PORT, DISP_BL_PIN );
	}
	else 
  {
		GPIO_ResetBits( DISP_BL_PORT, DISP_BL_PIN );
		WriteCmd(TFT_INVON);
	}
#else
	GPIO_ResetBits( DISP_BL_PORT, DISP_BL_PIN );
#endif
}

void TLcd::ColorInversion( bool state )
{
#ifndef DISPLAY_OFF
	if( state == 0 ) {
		WriteCmd(TFT_INVOFF);
	}
	else {
		WriteCmd(TFT_INVON);
	}
#endif
}

/*----------------------------------------------------------------------------------*/
#if defined DYSPLAY_FSMC
void TLcd::WriteCmd( uint8_t cmd, uint8_t data )
{
	*fsmcRegister = cmd;
	*fsmcData = data;
}

void TLcd::WriteCmd( uint8_t cmd )
{
	*fsmcRegister = cmd;
}

void TLcd::WriteData( uint8_t data )
{
	*fsmcData = data;
}

void TLcd::WriteData( uint16_t data )
{
	*fsmcData16 = data;
}

void TLcd::WriteData( uint32_t data )
{
	*fsmcData32 = data;
}
#endif


void TLcd::SetPos( uint16_t row, uint16_t col )
{
	uint16_t x1, x2, y1, y2;

	x1 = col;
	x2 = TFT_COLS;
	y1 = row;
	y2 = TFT_ROWS;

	WriteCmd(TFT_CASET);
 
	WriteData((uint8_t)(x1 >> 8));
	WriteData((uint8_t)(x1 & 0xff));
	WriteData((uint8_t)(x2 >> 8));
	WriteData((uint8_t)(x2 & 0xff));
 
	WriteCmd(TFT_RASET);
 
	WriteData((uint8_t)(y1 >> 8));
	WriteData((uint8_t)(y1 & 0xff));
	WriteData((uint8_t)(y2 >> 8));
	WriteData((uint8_t)(y2 & 0xff));
}

void TLcd::SetPos( uint16_t x, uint16_t y, uint16_t width, uint16_t height )
{
	uint16_t x1, x2, y1, y2;

	x1 = x;
	x2 = x1 + width - 1;
	y1 = y;
	y2 = y1 + height - 1;

	WriteCmd(TFT_CASET);
 
	WriteData((uint8_t)(x1 >> 8));
	WriteData((uint8_t)(x1 & 0xff));
	WriteData((uint8_t)(x2 >> 8));
	WriteData((uint8_t)(x2 & 0xff));
 
	WriteCmd(TFT_RASET);
 
	WriteData((uint8_t)(y1 >> 8));
	WriteData((uint8_t)(y1 & 0xff));
	WriteData((uint8_t)(y2 >> 8));
	WriteData((uint8_t)(y2 & 0xff));
}



/*----------------------------------------------------------------------------------*/
void TLcd::LcdClear()
{

	WriteCmd(TFT_CASET);
	WriteData((uint32_t) ((__REV16( TFT_COLS - 1 ) << 16) + (__REV16( 0 ))));
	WriteCmd(TFT_RASET);
	WriteData((uint32_t) ((__REV16( TFT_ROWS - 1 ) << 16) + (__REV16( 0 ))));
	WriteCmd(TFT_RAMWR);
	for(int pa=0; pa < TFT_ROWS * TFT_COLS; pa++ ) {
		WriteData((uint16_t)clBlack);
	}
}



/*----------------------------------------------------------------------------------*/
void TLcd::LCD_Init( void )
{
	SetSysPal(0x0000, 0xFFFF);
	// reset
//	__delay_cycles(25000);
 	RESPin(0);
	__delay_cycles(125000);
	RESPin(1);
	__delay_cycles(25000);
 
 	// init sequence
	WriteCmd(TFT_SLPOUT);

  bool chinaLCD = false;
  const uint8_t chinaIDArr[] = {0x54, 0x8A, 0x27};
  uint8_t tmpIDArr[3];
  ReadData(TFT_RDDID, 3, &tmpIDArr[0]);
  for (uint8_t i = 0, j = 0; i < 4; i++) 
  {
    if(tmpIDArr[i] == chinaIDArr[i]) j++;
    if(j >= 3) chinaLCD = true;
  }
  
	WriteCmd(TFT_INVOFF);
	WriteCmd(TFT_IDMOFF);
	WriteCmd(TFT_NORON);
// 	WriteCmd(TFT_MEMORY_ACCESS_CONTROL,0xE0);	
 	WriteCmd(TFT_MADCTL,0xA0);	
 
	// wait
	__delay_cycles(125000);

	// select 262K colour mode
	WriteCmd(TFT_COLMOD,0x55);  //16bit
	if(!chinaLCD)
  {
    WriteCmd( TFT_COLSET );  
    for ( uint8_t i = 0; i < 32; i++ ) WriteData(( uint8_t ) ( i * 255 / 31 ));
    for ( uint8_t i = 0; i < 32; i++ ) WriteData(( uint8_t ) 0xFF);
    for ( uint8_t i = 0; i < 64; i++ ) WriteData(( uint8_t ) ( i * 255 / 63 ));
    for ( uint8_t i = 0; i < 32; i++ ) WriteData(( uint8_t ) ( i * 255 / 31 ));
    for ( uint8_t i = 0; i < 32; i++ ) WriteData(( uint8_t ) 0xFF);
  }
  LcdClear();
 
	// turn the display on
	WriteCmd(TFT_DISPON);
//	GPIO_SetBits( DISP_BL_PORT, DISP_BL_PIN );
  LcdBL(true);

}



void TLcd::PutPixel( uint16_t x, uint16_t y, uint16_t color )
{
	SetPos( x, y, x+1, y+1 );
	WriteCmd(TFT_RAMWR);
	WriteData((uint16_t) __REV16(color));
}


void TLcd::PutBitmap(uint16_t x, uint16_t y, const TBitMap *_bmp)
{
	uint8_t scale = 1;
	const uint16_t mask[] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
	SetPos(x, y, _bmp->biWidth,  _bmp->biHeight);
	WriteCmd(TFT_RAMWR);

	if((_bmp->biPixelBit == 16) || ((_bmp->biPixelBit == 8) && (_bmp->biCompression == 1))) scale++;

	for(uint32_t bmpIndx = 0; bmpIndx < _bmp->biArrSize;)
	{
		uint16_t pixblock = _bmp->biArr[bmpIndx++];
		if(scale == 2) pixblock = (pixblock << 8) + _bmp->biArr[bmpIndx++];
		uint8_t pixcount = pixblock >> _bmp->biPixelBit;
		uint16_t pixcolor = _bmp->biPal[ pixblock & mask[_bmp->biPixelBit] ];
		for( uint8_t pix = 0; pix < pixcount; pix++)
		{
			WriteData((uint16_t) __REV16(pixcolor));
		}
	}
}

void TLcd::SetSysPal(uint16_t back, uint16_t front)
{
	SysPal[0] = back;
	SysPal[1] = front;
}

void TLcd::ExchSysPal(void)
{
  uint16_t tmp = SysPal[0];
	SysPal[0] = SysPal[1];
	SysPal[1] = tmp;
}

void TLcd::FillRectangle( uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, uint16_t color )
{
	SetPos( x, y, dx, dy );
	WriteCmd(TFT_RAMWR);
	for(int pa=0; pa < dx * dy; pa++ ) 
	{
		WriteData((uint16_t) __REV16(color));
	}
}

void TLcd::ReadData( uint8_t cmd, uint8_t data_size, uint8_t *data)
{
  DISP_DC_PORT->BSRRH = DISP_DC_PIN;
  DISP_CS_PORT->BSRRH = DISP_CS_PIN;
  lcdSendByte( cmd );
  DISP_DC_PORT->BSRRL = DISP_DC_PIN;
  lcdReciveByte();
  for (uint8_t i=0; i < data_size; i++)
  {
    data[i] = lcdReciveByte();
  }
}

uint8_t TLcd::lcdReciveByte(void)
{
  volatile uint8_t byte;
  uint8_t* idrptr = (uint8_t*) &DISP_D0_PORT->IDR;

  DISP_D0_PORT->MODER = DISP_D0_PORT->MODER & 0xFFFF0000;

  DISP_RD_PORT->BSRRH = DISP_RD_PIN;
	__delay_cycles(2);
  byte = *idrptr;
  DISP_RD_PORT->BSRRL = DISP_RD_PIN;
	__delay_cycles(1);

  DISP_D0_PORT->MODER = DISP_D0_PORT->MODER | 0x00005555;

  return byte;
}

/*----------------------------------------------------------------------------------*/
/* x1, y1 - начальные значения; x2, y2 - конечные значения							*/
/*----------------------------------------------------------------------------------*/
/*
void TLcd::Line( int x1, int y1, int x2, int y2, int action )
{
	int dx = ( x2 - x1 >= 0 ? 1 : -1 );
	int dy = ( y2 - y1 >= 0 ? 1 : -1 );

	int lengthX = abs( x2 - x1 );
	int lengthY = abs( y2 - y1 );

	int length = MAX( lengthX, lengthY );

	if( length == 0 ){
		PutPixel( x1, y1, action );
	}

	if( lengthY <= lengthX ){
		// Начальные значения 
		int x = x1;
		int y = y1;
		int d = -lengthX;

		// Основной цикл 
		length++;
		while( length-- ){
			PutPixel( x, y, action );
			x += dx;
			d += 2 * lengthY;
			if( d > 0 ){
				d -= 2 * lengthX;
				y += dy;
			}
		}
	}
	else {
		// Начальные значения 
		int x = x1;
		int y = y1;
		int d = - lengthY;

		// Основной цикл
		length++;
		while( length-- ){
			PutPixel( x, y, action );
			y += dy;
			d += 2 * lengthX;
			if( d > 0 ){
				d -= 2 * lengthY;
				x += dx;
			}
		}
	}
	x_curr_pos = x2;
	y_curr_pos = y2;
}
*/

/*
bool TLcd::CheckBuff( int Indx )
{
	if( Indx >= ( 128 * 64 / 8 )) return false;

	if( OffscreenBuff0[ Indx ] != OffscreenBuff1[ Indx ]){
		return true;
	}
	return false;
}



void TLcd::ClearScreen( void )
{
	ClearSrcBuffer( OffscreenBuff0 );
}

void TLcd::ClearBackScreen()
{
	ClearSrcBuffer( OffscreenBuff1 );
}
*/

