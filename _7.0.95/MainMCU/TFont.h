#pragma once

#ifndef FONT_H
#define FONT_H
/*----------------------------------------------------------------------------------*/
#include "_7seg_V3.h"
#include "_Arial_18.h"
#include "_Arial_20.h"
#include "_Courier_New_12.h"
#include "_Courier_New_48.h"
#include "_PT_Mono_8.h"
#include "_PT_Mono_18.h"
#include "_Mistral_72.h"
/*----------------------------------------------------------------------------------*/




enum _fnAlignTypes {algLeft,algCenter,algRight};

class TFont : public TLcd
{
	public:
		static void DrawText(uint16_t x, uint16_t y, const char *str, uint16_t lenpixels, uint8_t align);
		static void SetSysFont(TBitMap const *font_ptr);

	  	static void LcdPutChar(uint16_t x, uint16_t y, BYTE Char, uint16_t frontcolor, uint16_t backcolor);
	  	static void PutText(uint16_t x, uint16_t y,const char *Str);
    

		static uint16_t StringLen(const char *str);
		static uint16_t StringCnt(const char *str);

	private:


	protected:
};
/*----------------------------------------------------------------------------------*/
#endif