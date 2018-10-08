/**
  ******************************************************************************
  * @file StnFont.h
  * @author  VN Asliuk
  * @version  V0.0.1
  * @date  16.09.2009 15:47:24
  * @brief  Device/board hw description class implementation
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, CARDIAN SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 CARDIAN</center></h2>
  */ 
//------------------------------------------------------------------------------------
//#include "TAEOS_CM3.h"
#include "ae_ext_cm.h"
//------------------------------------------------------------------------------------
extern const BYTE* GetTableCharFont1(char CharToGet);
extern void GetFont1Paremeters(BYTE *Height,BYTE *Width);
//------------------------------------------------------------------------------------
