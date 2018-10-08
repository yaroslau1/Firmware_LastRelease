#ifndef TINTERPHARDWARE_H
#define TINTERPHARDWARE_H
/*----------------------------------------------------------------------------------*/

#define UART_GPIO_RCC_ENABLE		  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE)
#define UART_RCC_ENABLE		        RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE )
#define UART_DMA_ENABLE            RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE)

#define UART_RX_PORT			        GPIOB
#define UART_RX_PIN			          GPIO_Pin_7
#define UART_RX_PINSOURCE		      GPIO_PinSource7

#define UART_TX_PORT			        GPIOB
#define UART_TX_PIN			          GPIO_Pin_6
#define UART_TX_PINSOURCE		      GPIO_PinSource6

#define UART_AF                   GPIO_AF_0

#define INTER_PORT                USART1                                      // F0 - USART1

#define UART_IRQChannel           USART1_IRQn

#define UART_TX_DMA_CHANNEL       DMA1_Channel2

#define UART_TDR_ADDRESS          (&(USART1->TDR))

#define UART_TX_DMA_FLAG_TC       DMA1_FLAG_TC2
#define UART_TX_DMA_FLAG_GL       DMA1_FLAG_GL2

#endif //TINTERPHARDWARE_H