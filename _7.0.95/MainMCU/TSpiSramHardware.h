#ifndef TSPISRAMHARDWARE_H
#define TSPISRAMHARDWARE_H
/*----------------------------------------------------------------------------------*/

#define SPISRAM_ADDR_SIZE		3
#define SPISRAM_PAGE_SIZE		32

#define SPISRAM_GPIO_RCC_ENABLE		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE)
#define SPISRAM_SPI_RCC_ENABLE		RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE )
#define SPISRAM_CS_PORT			GPIOB
#define SPISRAM_CS_PIN			GPIO_Pin_12
#define SPISRAM_SCK_PORT		GPIOB
#define SPISRAM_SCK_PIN			GPIO_Pin_13
#define SPISRAM_SCK_PINSOURCE		GPIO_PinSource13
#define SPISRAM_MISO_PORT		GPIOB
#define SPISRAM_MISO_PIN		GPIO_Pin_14
#define SPISRAM_MISO_PINSOURCE		GPIO_PinSource14
#define SPISRAM_MOSI_PORT		GPIOB
#define SPISRAM_MOSI_PIN		GPIO_Pin_15
#define SPISRAM_MOSI_PINSOURCE		GPIO_PinSource15
#define SPISRAM_SPI			    SPI2
#define SPISRAM_SPI_AF			GPIO_AF_SPI2
#define SPISRAM_IRQ_PORT		GPIOB
#define SPISRAM_IRQ_PIN			GPIO_Pin_2

#define SPISRAM_SPI_PORT    GPIOB

//#define SPISRAM_LOWSIDE

#endif //TSPISRAMHARDWARE_H