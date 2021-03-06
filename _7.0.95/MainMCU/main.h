#ifndef MAIN_H
#define MAIN_H


/*----------------------------------------------------------------------------------*/
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "integer.h"
/*----------------------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "core_cm4.h"
/*----------------------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
/*----------------------------------------------------------------------------------*/
#include "ae_ext_cm.h"


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ������� ������
//------------------------------------------------------------------------------
// V 7.0.77  13.02.2016  �������������� � �������� ��� ������ ����� � 
//                       ������������.
//------------------------------------------------------------------------------
// V 7.0.78  22.02.2016  - ��������� ���������� �������� � ������ ������; 
//                       - ��������� ����� ��� ��������� � ���������� SD-�����;
//                       - ��������� �������� �������� ICI_IRQ_Cnt, ��� �������
//                       ���������� ������������.
//------------------------------------------------------------------------------
// V 7.0.79  05.03.2016  - ��������� ����������� �� ���� ��������� ������������
//                       ������ Bluetooth.
//------------------------------------------------------------------------------
// V 7.0.80  15.03.2016  - ��������� ����� ������.
//------------------------------------------------------------------------------
// V 7.0.85  01.07.2016  - ��������� ����� ������ � 10-��������� �������;
//                       - ���������� ��������� ������ ��� ������ � 10-���������
//                       �������.
//------------------------------------------------------------------------------
// V 7.0.86a 18.10.2016  - ���������� ������������� ADS1298 � ECGMCU;
//                       - ������������� ��������������� ���������;
//                       - �������� ������������� ������ ���������;
//                       - ���������� ������ ��� ������ � ��������� ����������
//                       ��������� �������������;
//                       - ������� ��������� ������ ���������� ��������;
//                       - ������ �������� � ��������� �����;
//                       - ���������� �������� ��������� ���������� ������� �
//                       ����������� ��������� ������ 9.01 � ���� (� �����
//                       ������ ������ ���������� �����������).
//------------------------------------------------------------------------------
// V 7.0.87  30.11.2016  - ��������� ����������� ����������� ��� ������������
//                       ������ 24:30:00.
//------------------------------------------------------------------------------
// V 7.0.88  22.12.2016  - ��������� �������� �������� ���������� � ���� ������.
//------------------------------------------------------------------------------
// V 7.0.89  03.05.2017  - ������� ������� ������� ���� ���������� ����������
//                       ������������;
//                       - ���������/���������� ��������� �� �����������
//                       ���������;
//                       - ������������ ������ ������� ���������;
//                       - ������������ ������� ���������� � ���� ������ 100��;
//                       - ������� �� 0,5� ����� ����������� ���������� ������-
//                       ������ (��� ������ � ���������� ������ LiIon);
//                       - ������� �� 3,4� ����� ���������� ���������� ������-
//                       ������ (��� ��������� �������);
//                       - ����� ������ �� �� ������� BT ����������, � �� 
//                       ��������� �������;
//                       - ����������� �������� �������� �� ������ �� BT;
//                       - ������������ ��� �������� ������ � ������� 30�.
//------------------------------------------------------------------------------
// V 7.0.90  12.05.2017  - ������� �������� ��������� R-������;
//                       - ��������� �������� ����� ��������� ���� �� BT.
//------------------------------------------------------------------------------
// V 7.0.91  17.05.2017  - ��������� �������� ����� ��������� ���� �� BT.
//------------------------------------------------------------------------------
// V 7.0.92  24.08.2017  - ���������� BT ������ ��� ������;
//                       - ������� ��������� ������ ���������� ��������;
//                       - ������� ��������� ��������� ������������.
//------------------------------------------------------------------------------
// V 7.0.93  05.04.2018  - ��������� �������� ������ � ���������;
//                       - ��������� ����������� (��������) �� ����������� ��
//                       ������������ ������;
//                       - ������� �� 3,0� ������ ����� ���������� ����������
//                       ������������ (��� ���������� ������� ������);
//                       - ������� �� 5,3� ������� ����� ���������� ����������
//                       ������������;
//                       - ��������� ��������� ��������� ��������;
//                       - ���������� ��������� ������;
//                       - ��������� �������� ������ ��������� �������;
//                       - ������� ����������� ��������������� ����������
//                       ���������.
//------------------------------------------------------------------------------
// V 7.0.94  20.04.2018  - ������� ������ ��������.
//------------------------------------------------------------------------------
// V 7.0.95  12.07.2018  - ����� ������ �������� ���  
//------------------------------------------------------------------------------
// V 7.0.96  04.10.2018  - ��������� �������������  
//------------------------------------------------------------------------------



#define FW_RELEASE       7096
#define FW_RELEASE_STR 	 "v7.0.96"
//------------------------------------------------------------------------------

#define PLL_M      2
#define PLL_N      312
#define PLL_P      8
#define PLL_Q      8



#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define ABS(a)	   (((a) < 0) ? -(a) : (a))
/*----------------------------------------------------------------------------------*/
// ���� ������ ����� - 7 ������. ��� �������� 39,936��� ~175,3��
inline void	__delay_cycles(	volatile int temp ){
	for( int i=0; i<temp; i++ ) __no_operation();
}




/*----------------------------------------------------------------------------------*/
#define VERSION 0.55
//#define DISABLE_SD_CARD
/*----------------------------------------------------------------------------------*/
#endif
