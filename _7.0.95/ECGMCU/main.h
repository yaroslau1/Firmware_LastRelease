#ifndef MAIN_H
#define MAIN_H
/*----------------------------------------------------------------------------------*/
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//#include "integer.h"
/*----------------------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"
#include "core_cm0.h"
/*----------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------*/
#include "ae_ext_cm.h"

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define ABS(a)	   (((a) < 0) ? -(a) : (a))
/*----------------------------------------------------------------------------------*/
inline void	__delay_cycles(	volatile int temp ){
	for( int i=0; i<temp; i++ ) __no_operation();
}

#define VREFINT_CAL ((__IO uint16_t *) 0x1FFFF7BA)

extern "C" uint32_t __checksum;                                 // импортируем контрольную сумму

/*----------------------------------------------------------------------------------*/
// version hystory
// 0.50  init
// 0.51  rtc. p-p buff. uart
// 0.52  adc data over uart
// 0.55  crc. ecg

#define VERSION 0.55

#define BUFFERED_TRANSFER
#define BUFF_AB_SIZE 2048 //1024
/*----------------------------------------------------------------------------------*/

bool chekCRC(uint32_t* begin, uint32_t* end, uint32_t read_crc);



#endif
