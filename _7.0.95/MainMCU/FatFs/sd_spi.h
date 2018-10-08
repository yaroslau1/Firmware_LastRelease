#ifndef SD_SPI_H

#include "main.h"
#include "diskio.h"


/* Card type flags (CardType) */
#define CT_MMC		0x01		/* MMC ver 3 */
#define CT_SD1		0x02		/* SD ver 1 */
#define CT_SD2		0x04		/* SD ver 2 */
#define CT_SDC		(CT_SD1|CT_SD2)	/* SD */
#define CT_BLOCK	0x08		/* Block addressing */


#ifdef __cplusplus
 extern "C" {
#endif

//#define SD_DETECT_PIN                    GPIO_Pin_11
//#define SD_DETECT_GPIO_PORT              GPIOF
//#define SD_DETECT_GPIO_CLK               RCC_APB2Periph_GPIOF

//#define SD_SPI                           SPI2
//#define SD_SPI_CLK                       RCC_APB1Periph_SPI2
//#define SD_SPI_SCK_PIN                   GPIO_Pin_13
//#define SD_SPI_SCK_GPIO_PORT             GPIOB
//#define SD_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOB
//#define SD_SPI_MISO_PIN                  GPIO_Pin_14
//#define SD_SPI_MISO_GPIO_PORT            GPIOB
//#define SD_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOB
//#define SD_SPI_MOSI_PIN                  GPIO_Pin_15
//#define SD_SPI_MOSI_GPIO_PORT            GPIOB
//#define SD_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOB
//#define SD_CS_PIN                        GPIO_Pin_8
//#define SD_CS_GPIO_PORT                  GPIOE
//#define SD_CS_GPIO_CLK                   RCC_APB2Periph_GPIOE
////#define SD_DETECT_PIN                    GPIO_Pin_11
////#define SD_DETECT_GPIO_PORT              GPIOF
////#define SD_DETECT_GPIO_CLK               RCC_APB2Periph_GPIOF

#define SD_OK							SD_RESPONSE_NO_ERROR

#define SD_DUMMY_BYTE   0xFF
	

BYTE rcvr_spi (void);
void rcvr_spi_m( uint8_t *dst );
int wait_ready (void);	/* 1:OK, 0:Timeout */
void deselect (void);
int select (void);	/* 1:Successful, 0:Timeout */
int power_status(void);		/* Socket power state: 0=off, 1=on */
void power_on (void);
void power_off (void);
int rcvr_datablock (BYTE *buff, UINT btr);
int xmit_datablock (const BYTE *buff, BYTE token);
BYTE send_cmd (BYTE cmd, DWORD arg);
void disk_timerproc (void);
DRESULT disk_control( BYTE drv, BYTE ctrl, void *buff );

//void xmit_spi( uint8_t dat );
uint8_t xmit_spi( uint8_t dat );
void CS_LOW( void );
void CS_HIGH( void );

void FCLK_SLOW( void );
void FCLK_FAST( void );
void SD_LowLevel_Init(void);
void SD_LowLevel_Deinit(void);
DSTATUS SD_Init( BYTE drv );
DRESULT SD_ReadMultiBlocks(	BYTE drv, BYTE *buff, DWORD sector,	BYTE count );
DRESULT SD_WriteMultiBlocks( BYTE drv, const BYTE *buff, DWORD sector, BYTE count );
void SD_Timer_Deinit( void );
void SD_Timer_Init( void );

#ifdef __cplusplus
}
#endif
#endif /* SD_SPI_H */