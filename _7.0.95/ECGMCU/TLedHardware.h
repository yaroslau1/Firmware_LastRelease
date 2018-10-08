#ifndef TLEDHARDWARE_H
#define TLEDHARDWARE_H
/*----------------------------------------------------------------------------------*/

#define LED_GPIO_RCC_ENABLE		    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE)
#define LED_RCC_ENABLE		        RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE )

#define LED1_PORT			            GPIOB
#define LED1_PIN			            GPIO_Pin_0
#define LED1_PINSOURCE		        GPIO_PinSource0

#define LED2_PORT			            GPIOB
#define LED2_PIN			            GPIO_Pin_1
#define LED2_PINSOURCE		        GPIO_PinSource1

#define LED_AF                    GPIO_AF_1

#endif //TLEDHARDWARE_H