/*-----------------------------------------------------------------------*/
/* MMCv3/SDv1/SDv2 (in SPI mode) control module  (C)ChaN, 2010           */
/*-----------------------------------------------------------------------*/
/* Only rcvr_spi(), xmit_spi(), disk_timerproc() and some macros         */
/* are platform dependent.                                               */
/*-----------------------------------------------------------------------*/
#include "sd_spi.h"
#include "ff.h"

#include "THardware.h"
#include "TLcdTrace.h"

/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/
#define SOCKWP		(false)
#define SOCKINS		(true)

/* Definitions for MMC/SDC command */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */


volatile WORD Timer;
volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */
volatile BYTE Timer1, Timer2;	/* 100Hz decrement timer */
BYTE CardType;			/* Card type flags */


/*----------------------------------------------------------------------------------*/
/* @brief	Инициализация SD карты													*/
/* @param	Номер диска																*/
/* @return	Статус																	*/
/*----------------------------------------------------------------------------------*/
DSTATUS SD_Init( BYTE drv )
{
	BYTE n, cmd, ty, ocr[4];

	if (drv) return STA_NOINIT;			/* Supports only single drive */
	if (Stat & STA_NODISK) return Stat;	/* No card in the socket */

	power_on();							/* Force socket power on */
	FCLK_FAST();
//  FCLK_SLOW();
  
//  int rr = xmit_spi(213);
//  #ifdef LCD_TRACE
//    TLcdTrace::AddLineX("SPI ",rr);
//  #endif
  
  
	for (n = 10; n; n--) rcvr_spi();	/* 80 dummy clocks */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* Enter Idle state */
		Timer1 = 100;						/* Initialization timeout of 1000 msec */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2? */
//      THardware::LedOn();
			for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();		/* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* The card can work at vdd range of 2.7-3.6V */
				while (Timer1 && send_cmd(ACMD41, 1UL << 30));	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if (Timer1 && send_cmd(CMD58, 0) == 0) {		/* Check CCS bit in the OCR */
					for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 */
				}
			}
		} else {							/* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) 	{
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
			while (Timer1 && send_cmd(cmd, 0));			/* Wait for leaving idle state */
			if (!Timer1 || send_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
				ty = 0;
		}
	}
	CardType = ty;
	deselect();

	if (ty) {			/* Initialization succeded */
		Stat &= ~STA_NOINIT;		/* Clear STA_NOINIT */
//		FCLK_FAST();
    #ifdef LCD_TRACE
      TLcdTrace::AddLine("SD Type",ty);
    #endif
	} else {			/* Initialization failed */
    #ifdef LCD_TRACE
//      TLcdTrace::AddLine("SD Failed");
    #endif
		power_off();
	}

	return Stat;
}



/*----------------------------------------------------------------------------------*/
/* @brief	Функции изменяют скорость SPI											*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void FCLK_SLOW( void )
{
	SPI_InitTypeDef SPI_InitStructure;

	/*!< SD_SPI Config */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init( SD_SPI, &SPI_InitStructure );
	
	SPI_Cmd( SD_SPI, ENABLE );
}

/*----------------------------------------------------------------------------------*/
void FCLK_FAST( void )
{
	SPI_InitTypeDef   SPI_InitStructure;

	/*!< SD_SPI Config */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init( SD_SPI, &SPI_InitStructure );
	
	SPI_Cmd( SD_SPI, ENABLE );
}



/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC via SPI  (Platform dependent)                  */
/* Приём и передача должны делаться в одно время 						 */
/*-----------------------------------------------------------------------*/
uint8_t xmit_spi( uint8_t dat )
{
	while( SPI_I2S_GetFlagStatus( SD_SPI, SPI_I2S_FLAG_TXE ) == RESET );		/*!< Wait until the transmit buffer is empty */
	SPI_I2S_SendData( SD_SPI, dat );											/*!< Send the byte */
	
	while( SPI_I2S_GetFlagStatus( SD_SPI, SPI_I2S_FLAG_RXNE ) == RESET );		/*!< Wait until a data is received */
	return SPI_I2S_ReceiveData( SD_SPI );										/*!< Get the received data */
}

/*----------------------------------------------------------------------------------*/
void rcvr_spi_m( uint8_t *dst )
{
	*dst = xmit_spi(SD_DUMMY_BYTE);
}



/*-----------------------------------------------------------------------*/
/* Receive a byte from MMC via SPI  (Platform dependent)                 */
/*-----------------------------------------------------------------------*/
uint8_t rcvr_spi( void )
{
	return xmit_spi( SD_DUMMY_BYTE  );	
}



/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/
int wait_ready (void)	/* 1:OK, 0:Timeout */
{
	BYTE d;

	Timer2 = 50;	/* Wait for ready in timeout of 500ms */
	do
		d = rcvr_spi();
	while (d != 0xFF && Timer2);

	return (d == 0xFF) ? 1 : 0;
}



/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/
void deselect (void)
{
	CS_HIGH();
	rcvr_spi();	// Dummy clock (force DO hi-z for multiple slave SPI)
}



/*-----------------------------------------------------------------------*/
/* Select the card and wait for ready                                    */
/*-----------------------------------------------------------------------*/
int select( void )	/* 1:Successful, 0:Timeout */
{
	CS_LOW();
	
	rcvr_spi();	/* Dummy clock (force DO enabled) */

	if (wait_ready()) return 1;	/* OK */
	deselect();
	return 0;	/* Timeout */
}

/*----------------------------------------------------------------------------------*/
void CS_LOW( void )
{
	while( SPI_I2S_GetFlagStatus( SD_SPI, SPI_I2S_FLAG_TXE ) == RESET );
	GPIO_ResetBits( SD_CS_PORT, SD_CS_PIN );
}

/*----------------------------------------------------------------------------------*/
void CS_HIGH( void )
{
	while( SPI_I2S_GetFlagStatus( SD_SPI, SPI_I2S_FLAG_BSY ) == SET );	/* Проверка отправки всех битов по SPI */
//	__delay_cycles(1);
	GPIO_SetBits( SD_CS_PORT, SD_CS_PIN );
}



/*-----------------------------------------------------------------------*/
/* Power Control  (Platform dependent)                                   */
/*-----------------------------------------------------------------------*/
/* When the target system does not support socket power control, there   */
/* is nothing to do in these functions and chk_power always returns 1.   */
/*-----------------------------------------------------------------------*/
int power_status(void)		/* Socket power state: 0=off, 1=on */
{
	return true;
}

/*-----------------------------------------------------------------------*/
void power_on (void)
{
	SD_Timer_Init();
	SD_LowLevel_Init();
	CS_HIGH();
	for (Timer1 = 2; Timer1; );	/* Wait for 20ms */
}

/*-----------------------------------------------------------------------*/
void power_off (void)
{
	SD_LowLevel_Deinit();
	SD_Timer_Deinit();
	Stat |= STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/
int rcvr_datablock (
	BYTE *buff,			/* Data buffer to store received data */
	UINT btr			/* Byte count (must be multiple of 4) */
)
{
	BYTE token;

//  TLcdTrace::AddLine("Read data...");


	Timer1 = 20;
	do {							/* Wait for data packet in timeout of 200ms */
		token = rcvr_spi();
	} while ((token == 0xFF) && Timer1);
	if(token != 0xFE) return 0;		/* If not valid data token, retutn with error */

//  TLcdTrace::AddLine("Read data...");
	do {							/* Receive the data block into buffer */
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
		rcvr_spi_m(buff++);
	} while (btr -= 4);
	rcvr_spi();						/* Discard CRC */
	rcvr_spi();

	return 1;						/* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/
int xmit_datablock (
	const BYTE *buff,	/* 512 byte data block to be transmitted */
	BYTE token			/* Data/Stop token */
)
{
	BYTE resp, wc;


	if (!wait_ready()) return 0;

	xmit_spi(token);					/* Xmit data token */
	if (token != 0xFD) {	/* Is data token */
		wc = 0;
		do {							/* Xmit the 512 byte data block to MMC */
			xmit_spi(*buff++);
			xmit_spi(*buff++);
		} while (--wc);
		xmit_spi(0xFF);					/* CRC (Dummy) */
		xmit_spi(0xFF);
		resp = rcvr_spi();				/* Reveive data response */
		if ((resp & 0x1F) != 0x05)		/* If not accepted, return with error */
			return 0;
	}

	return 1;
}



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/
BYTE send_cmd (		/* Returns R1 resp (bit7==1:Send failed) */
	BYTE cmd,		/* Command index */
	DWORD arg		/* Argument */
)
{
	BYTE n, res;


	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card and wait for ready */
	deselect();
	if (!select()) return 0xFF;
  
	/* Send command packet */
	xmit_spi(0x40 | cmd);				/* Start + Command index */
	xmit_spi((BYTE)(arg >> 24));		/* Argument[31..24] */
	xmit_spi((BYTE)(arg >> 16));		/* Argument[23..16] */
	xmit_spi((BYTE)(arg >> 8));			/* Argument[15..8] */
	xmit_spi((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);

	/* Receive command response */
	if (cmd == CMD12) rcvr_spi();		/* Skip a stuff byte when stop reading */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
		res = rcvr_spi();
	while((res & 0x80) && --n);

	return res;							/* Return with the response value */
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT SD_ReadMultiBlocks(
	BYTE drv,			/* Physical drive nmuber (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */
  

	if (count == 1) {	/* Single block read */
		if ((send_cmd(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
			&& rcvr_datablock(buff, 512))
    {
			count = 0;
    }
	}
	else {				/* Multiple block read */
		if (send_cmd(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!rcvr_datablock(buff, 512)) break;
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
DRESULT SD_WriteMultiBlocks(
	BYTE drv,			/* Physical drive nmuber (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (Stat & STA_PROTECT) return RES_WRPRT;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	if (count == 1) {	/* Single block write */
		if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
			&& xmit_datablock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple block write */
		if (CardType & CT_SDC) send_cmd(ACMD23, count);
		if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
DRESULT disk_control (
	BYTE drv,		/* Physical drive nmuber (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = (BYTE *)buff;
	WORD csize;


	if (drv) return RES_PARERR;

	res = RES_ERROR;

	if (ctrl == CTRL_POWER) {
		switch (ptr[0]) {
		case 0:		/* Sub control code (POWER_OFF) */
			power_off();		/* Power off */
			res = RES_OK;
			break;
		case 1:		/* Sub control code (POWER_GET) */
			ptr[1] = (BYTE)power_status();
			res = RES_OK;
			break;
		default :
			res = RES_PARERR;
		}
	}
	else {
		if (Stat & STA_NOINIT) return RES_NOTRDY;

		switch (ctrl) {
		case CTRL_SYNC :		/* Make sure that no pending write process. Do not remove this or written sector might not left updated. */
			if (select()) {
				deselect();
				res = RES_OK;
			}
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
				if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
					csize = csd[9] + ((WORD)csd[8] << 8) + 1;
					*(DWORD*)buff = (DWORD)csize << 10;
				} else {					/* SDC ver 1.XX or MMC*/
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
					*(DWORD*)buff = (DWORD)csize << (n - 9);
				}
				res = RES_OK;
			}
			break;

		case GET_SECTOR_SIZE :	/* Get R/W sector size (WORD) */
			*(WORD*)buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
			if (CardType & CT_SD2) {	/* SDv2? */
				if (send_cmd(ACMD13, 0) == 0) {	/* Read SD status */
					rcvr_spi();
					if (rcvr_datablock(csd, 16)) {				/* Read partial block */
						for (n = 64 - 16; n; n--) rcvr_spi();	/* Purge trailing data */
						*(DWORD*)buff = 16UL << (csd[10] >> 4);
						res = RES_OK;
					}
				}
			} else {					/* SDv1 or MMCv3 */
				if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {	/* Read CSD */
					if (CardType & CT_SD1) {	/* SDv1 */
						*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
					} else {					/* MMCv3 */
						*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
					}
					res = RES_OK;
				}
			}
			break;

		case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
			*ptr = CardType;
			res = RES_OK;
			break;

		case MMC_GET_CSD :		/* Receive CSD as a data block (16 bytes) */
			if (send_cmd(CMD9, 0) == 0		/* READ_CSD */
				&& rcvr_datablock(ptr, 16))
				res = RES_OK;
			break;

		case MMC_GET_CID :		/* Receive CID as a data block (16 bytes) */
			if (send_cmd(CMD10, 0) == 0		/* READ_CID */
				&& rcvr_datablock(ptr, 16))
				res = RES_OK;
			break;

		case MMC_GET_OCR :		/* Receive OCR as an R3 resp (4 bytes) */
			if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
				for (n = 4; n; n--) *ptr++ = rcvr_spi();
				res = RES_OK;
			}
			break;

		case MMC_GET_SDSTAT :	/* Receive SD status as a data block (64 bytes) */
			if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
				rcvr_spi();
				if (rcvr_datablock(ptr, 64))
					res = RES_OK;
			}
			break;

		default:
			res = RES_PARERR;
		}

		deselect();
	}

	return res;
}



/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure                                      */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */
void disk_timerproc( void )
{
	BYTE n, s;

	n = Timer1;				/* 100Hz decrement timer */
	if (n) Timer1 = --n;
	n = Timer2;
	if (n) Timer2 = --n;

	s = Stat;

	if (SOCKWP)				/* Write protected */
		s |= STA_PROTECT;
	else					/* Write enabled */
		s &= ~STA_PROTECT;

	if (SOCKINS)			/* Card inserted */
		s &= ~STA_NODISK;
	else					/* Socket empty */
		s |= (STA_NODISK | STA_NOINIT);

	Stat = s;				/* Update MMC status */
}




/**************************************************************************************/
/**************************************************************************************/
/**************************************************************************************/

/*----------------------------------------------------------------------------------*/
/* @brief	None																	*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void SD_Timer_Init( void )
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
	
	/* must be 100 Hz or 10ms */
	TIM_TimeBaseStructure.TIM_Prescaler = 6; //4;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 57050; //50000;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit( TIM3, &TIM_TimeBaseStructure );
	
	TIM_Cmd( TIM3, ENABLE );
	
	TIM_ITConfig( TIM3, TIM_IT_Update, ENABLE );
}

/*----------------------------------------------------------------------------------*/
void SD_Timer_Deinit( void )
{
	TIM_Cmd( TIM3, DISABLE );
	TIM_DeInit( TIM3 );
	TIM_SetCounter( TIM3, 0 );
}


void SD_LowLevel_Deinit( void )
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  
	GPIO_SetBits( SD_EN_PORT, SD_EN_PIN );

	
	SPI_Cmd(SD_SPI, DISABLE); /*!< SD_SPI disable */
	SPI_I2S_DeInit(SD_SPI);   /*!< DeInitializes the SD_SPI */
	
	/*!< SD_SPI Periph clock disable */
	SD_SPI_RCC_DISABLE;
	
	/*!< Configure SD_SPI pins: SCK */
	GPIO_InitStructure.GPIO_Pin = SD_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SD_SCK_PORT, &GPIO_InitStructure);
	
	/*!< Configure SD_SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin = SD_MISO_PIN;
	GPIO_Init(SD_MISO_PORT, &GPIO_InitStructure);
	
	/*!< Configure SD_SPI pins: MOSI */
	GPIO_InitStructure.GPIO_Pin = SD_MOSI_PIN;
	GPIO_Init(SD_MISO_PORT, &GPIO_InitStructure);
	
	/*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
	GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
	GPIO_Init(SD_CS_PORT, &GPIO_InitStructure);
	
	//  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
	//  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
	//  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);
}



/*----------------------------------------------------------------------------------*/
/* @brief	Initializes the SD_SPI and CS pins										*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void SD_LowLevel_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;
	
	/*!< SD_SPI_CS_GPIO, SD_SPI_MOSI_GPIO, SD_SPI_MISO_GPIO, SD_SPI_DETECT_GPIO
	and SD_SPI_SCK_GPIO Periph clock enable */
	SD_GPIO_RCC_ENABLE;
	
	/*!< SD_SPI Periph clock enable */
	SD_SPI_RCC_ENABLE;
	
	/*!< Configure SD_SPI pins: SCK*/
	GPIO_InitStructure.GPIO_Pin = SD_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_PinAFConfig(SD_SCK_PORT, SD_SCK_PINSOURCE, SD_SPI_AF);
	GPIO_Init(SD_SCK_PORT, &GPIO_InitStructure);
	
	/*!< Configure SD_SPI pins: MOSI*/
	GPIO_InitStructure.GPIO_Pin = SD_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_PinAFConfig(SD_MOSI_PORT, SD_MOSI_PINSOURCE, SD_SPI_AF);
	GPIO_Init(SD_MOSI_PORT, &GPIO_InitStructure);
	
	/*!< Configure SD_SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin = SD_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_PinAFConfig(SD_MISO_PORT, SD_MISO_PINSOURCE, SD_SPI_AF);
	GPIO_Init(SD_MISO_PORT, &GPIO_InitStructure);
	
	/*!< Configure SD_SPI_CS_PIN pin: SD Card CS pin */
	GPIO_InitStructure.GPIO_Pin = SD_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
	GPIO_Init(SD_CS_PORT, &GPIO_InitStructure);

	/*!< Configure SD_SPI_EN_PIN pin: SD Card EN pin */
	GPIO_ResetBits( SD_EN_PORT, SD_EN_PIN );
	GPIO_InitStructure.GPIO_Pin = SD_EN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
	GPIO_Init(SD_EN_PORT, &GPIO_InitStructure);
	
	//  /*!< Configure SD_SPI_DETECT_PIN pin: SD Card detect pin */
	//  GPIO_InitStructure.GPIO_Pin = SD_DETECT_PIN;
	//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	//  GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_InitStructure);
	
	/*!< SD_SPI Config */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init( SD_SPI, &SPI_InitStructure );
	
	SPI_Cmd( SD_SPI, ENABLE ); /*!< SD_SPI enable */
}