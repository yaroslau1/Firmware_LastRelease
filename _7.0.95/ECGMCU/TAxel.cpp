/********************************************************************************/
/* @title		Axel            			*/
/* @author	Aleksis							  */
/* @version	0.1							      */
/* @date		26.11.2015						*/
/********************************************************************************/
/* @brief									*/
/********************************************************************************/

#include "TAxel.h"

#include  "TAxelHardware.h"

uint16_t TAxel::Axel_AxisX = 0;
uint16_t TAxel::Axel_AxisY = 0;
uint16_t TAxel::Axel_AxisZ = 0;

void TAxel::Axel_Init(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;

	/* Configure AX_EN */
	GPIO_SetBits( AXEL_PORT, AX_EN_PIN);
  
	GPIO_InitStructure.GPIO_Pin = AX_EN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(AXEL_PORT, &GPIO_InitStructure);

	/* Configure ADC channels as analog input */
	GPIO_InitStructure.GPIO_Pin = AXEL_X_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(AXEL_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = AXEL_Y_PIN;
	GPIO_Init(AXEL_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = AXEL_Z_PIN;
	GPIO_Init(AXEL_PORT, &GPIO_InitStructure);
}  