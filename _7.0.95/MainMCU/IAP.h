/*
    Title:
    Author:   VN Asliuk(Cardian)
    Date:     22.09.2010 12:04:42
    Purpose:
    Software:
    Hardware:
    Note:
*/
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#ifndef AIAP_H
#define AIAP_H
//------------------------------------------------------------------------------------
#include "main.h"
#include "stm32f4xx_conf.h"
#include "ae_ext_cm.h"
#include "stm32f4xx.h"
#include <stdio.h>
//#include "Pinout.h"

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
void IAP_Start();
void IAP_Delay();

extern int  UsartRcv;
//------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------