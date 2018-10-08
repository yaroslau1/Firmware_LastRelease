#ifndef TAXELHARDWARE_H
#define TAXELHARDWARE_H
/*----------------------------------------------------------------------------------*/

#define AXEL_GPIO_RCC_ENABLE	    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE)
#define AXEL_RCC_ENABLE		        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

#define AXEL_PORT			            GPIOA

#define AXEL_X_PIN 	              GPIO_Pin_0
#define AXEL_Y_PIN			          GPIO_Pin_1
#define AXEL_Z_PIN                GPIO_Pin_2
#define AX_EN_PIN                 GPIO_Pin_15

#endif //TAXELHARDWARE_H