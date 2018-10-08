/************************************************************************************/
/* Title:		Low-Level functions for FatFs										*/
/* Author:		Cyber Genius														*/
/* Version:		1.0																	*/
/* Date:		06.07.2012															*/
/************************************************************************************/
/*																					*/
/************************************************************************************/
#include <string.h>

#include "diskio.h"

#ifdef SD_SDIO_MODE
	#include "sd_sdio.h"
#elif defined SD_SPI_MODE
	#include "sd_spi.h"
#else
	#error "No mode for SD card!"
#endif

#include "ff.h"
#include "main.h"

#define SECTOR_SIZE 512U



/*----------------------------------------------------------------------------------*/
/* Inidialize a Drive      				                                            */
/*----------------------------------------------------------------------------------*/
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	DSTATUS res;
	
	res = SD_Init( drv );
	
	return ((DSTATUS)res);
}



/*----------------------------------------------------------------------------------*/
/* Return Disk Status                                       			            */
/*----------------------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	if (drv) return STA_NOINIT;		/* Supports only single drive */
	return 0;
}



/*----------------------------------------------------------------------------------*/
/* Read Sector(s)                                                       			*/
/*----------------------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
#ifdef SD_SDIO_MODE
	SD_Error status = SD_OK;

	SD_ReadMultiBlocks( buff, sector << 9, 512, 1 );

	/* Check if the Transfer is finished */
	status = SD_WaitReadOperation();

	/* Обработка извлечение карты */
//	if( status == SD_DATA_TIMEOUT )
//		return RES_ERROR;

	while( SD_GetStatus() != SD_TRANSFER_OK );

	if( status == SD_OK ){
	  return RES_OK;
	}else{
	  return RES_ERROR;
	}

#elif defined SD_SPI_MODE
	DRESULT res;
	
	res = SD_ReadMultiBlocks( drv, buff, sector, count );
	
	return res;
#endif
}



/*----------------------------------------------------------------------------------*/
/* Write Sector(s)                                                   			    */
/*----------------------------------------------------------------------------------*/
#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
#ifdef SD_SDIO_MODE
	SD_Error status = SD_OK;

	SD_WriteMultiBlocks( (BYTE *)buff, sector << 9, 512, 1 );

	/* Check if the Transfer is finished */
	status = SD_WaitWriteOperation();
	while( SD_GetStatus() != SD_TRANSFER_OK );

	if( status == SD_OK )
	{
		return RES_OK;
	}
	else
	{
		return RES_ERROR;
	}

#elif defined SD_SPI_MODE
	DRESULT res;
	
	res = SD_WriteMultiBlocks( drv, buff, sector, count );

	return res;
#endif
}
#endif /* _READONLY */



/*----------------------------------------------------------------------------------*/
/* ???							                                        			*/
/*----------------------------------------------------------------------------------*/
DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
#ifdef SD_SDIO_MODE
	DRESULT res = RES_OK;
	switch (ctrl) {
	case GET_SECTOR_COUNT :	  // Get number of sectors on the disk (DWORD)
		*(DWORD*)buff = 131072;	// 4*1024*32 = 131072
		res = RES_OK;
		break;

	case GET_SECTOR_SIZE :	  // Get R/W sector size (WORD)
		*(WORD*)buff = 512;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE :	    // Get erase block size in unit of sector (DWORD)
		*(DWORD*)buff = 32;
		res = RES_OK;
	  }

	return res;
#elif defined SD_SPI_MODE
	DRESULT res;
	
	res = disk_control( drv, ctrl, buff );
					
	return res;
#endif /* SD_SDIO_MODE */
}



/*----------------------------------------------------------------------------------*/
/* Write data creation file															*/
/*----------------------------------------------------------------------------------*/
DWORD get_fattime( void )
{
	return	((2006UL-1980) << 25)	      // Year = 2006
			| (2UL << 21)	      // Month = Feb
			| (9UL << 16)	      // Day = 9
			| (22U << 11)	      // Hour = 22
			| (30U << 5)	      // Min = 30
			| (0U >> 1)	      // Sec = 0
			;
}



/*----------------------------------------------------------------------------------*/
/* Returns a start sector adress of file											*/
/*----------------------------------------------------------------------------------*/
DWORD get_file_sector_pos( FIL *file )
{
	return (( file->clust - file->sclust ) * 512 );
}

















