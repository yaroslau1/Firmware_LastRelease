/**
  ******************************************************************************
  * @file Utilities.h
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
#ifndef TUTILITIES_H
#define TUTILITIES_H
//------------------------------------------------------------------------------------
#include "ae_ext_cm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <string.h>
//------------------------------------------------------------------------------------
enum Radix {
    BINARY = 2,
    OCTOPLE = 8,
    DECIMAL = 10,
    HEXADECIMAL = 16
};
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

extern void FillSrcBuffer(BYTE *Buffer,BYTE Byte2Fill);
extern void ClearSrcBuffer(BYTE *Buffer);
extern void CopySrcBuffer(BYTE *DstBuffer,BYTE *SrcBuffer);
extern void itos(int val,char *buf,int size,Radix r=DECIMAL);
extern void itodts(int val,char *buf,int size);
extern void ix10tosAsFloat(int val,char *buf,int size);
extern int reverseBits(int val);
extern char *ftoa(float f,int Fract);

//------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------
