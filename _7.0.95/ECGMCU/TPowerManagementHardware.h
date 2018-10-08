#ifndef TPWRHARDWARE_H
#define TPWRHARDWARE_H
/*----------------------------------------------------------------------------------*/

#define PWR_GPIO_RCC_ENABLE		    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE)
#define PWR_ADC_RCC_ENABLE		    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE)
#define PWR_DMA_RCC_ENABLE        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE)
#define PWR_TIM_RCC_ENABLE        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE)

#define PWR_PORT			            GPIOA

#define NBAT_PIN			            GPIO_Pin_3
#define LBAT_P_PIN			          GPIO_Pin_4
#define LBAT_N_PIN                GPIO_Pin_5
#define LOW_BAT_PIN               GPIO_Pin_6

#define LBAT_EN_PIN               GPIO_Pin_12
#define FAST_CHRG_EN_PIN          GPIO_Pin_7

#define PWR_ADC                   ADC1
#define PWR_DMA_CHAN              DMA1_Channel1 
#define PWR_TIM                   TIM15
#define PWR_IRQn                  TIM15_IRQn

#endif //TPWRHARDWARE_H