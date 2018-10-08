#ifndef TINTERPHARDWARE_H
#define TINTERPHARDWARE_H
/*----------------------------------------------------------------------------------*/

#define UART_GPIO_RCC_ENABLE		  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE)
#define UART_RCC_ENABLE		        RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE )

#define UART_RX_PORT			        GPIOB
#define UART_RX_PIN			          GPIO_Pin_11
#define UART_RX_PINSOURCE		      GPIO_PinSource11

#define UART_TX_PORT			        GPIOB
#define UART_TX_PIN			          GPIO_Pin_10
#define UART_TX_PINSOURCE		      GPIO_PinSource10

#define UART_AF                   GPIO_AF_USART3

#define INTER_PORT                USART3                                      // F0 - USART1

#define UART_IRQChannel           USART3_IRQn

#endif //TINTERPHARDWARE_H