/******************************************************************************/
/* Title:                                                                     */
/* Author:                                                                    */
/* Version:             1.0                                                   */
/* Date:                26.11.2015                                            */
/******************************************************************************/

#include "TLcd.h"
#include "TFont.h"
//------------------------------------------------------------------------------






TBitMap const* SysFont = &_Courier_New_12[0];
TBitMap const* ToolbarFont = &_Courier_New_12[0];



void TFont::PutText(uint16_t x,uint16_t y,const char *Str)
{
	int sLen = strlen(Str);
	
	int CursorPos = x;
	
	for(int i = 0;i < sLen;i++)
	{
		PutBitmap(CursorPos,y,&_Courier_New_12[Str[i]]);
//		PutBitmap(CursorPos,y,&SysFont[Str[i]]);
//		CursorPos += LCD_CURSOR_OFFSET;
//		CursorPos += _Courier_New_12[i]->biWidth;
		CursorPos += 10;
		if(CursorPos >= (320)) break;
	}
}



/*----------------------------------------------------------------------------------*/
void TFont::DrawText(uint16_t x, uint16_t y, const char *str, uint16_t lenpixels, uint8_t align)
{
	static bool Sync = true;
	
	if(Sync == false) return;
	
	Sync = false;
	
	int sLen = strlen(str);
	int xpos = x;

	int rightpos = x+lenpixels;
	if(align != algLeft)
	{
		int pxllen = StringLen(str);
		if(rightpos > x+lenpixels) rightpos = x+lenpixels;
				
		if(align == algCenter) xpos = (x+(lenpixels/2)) - (pxllen/2);
		else xpos = rightpos - pxllen;	
	}
	
        TLcd::FillRectangle(x,y,xpos-x,SysFont[0].biHeight,SysPal[0]);
		
	for(int i = 0;i < sLen;i++)
	{
		PutBitmap(xpos,y,&SysFont[str[i]]);
		xpos += SysFont[str[i]].biWidth;
		if(xpos >= rightpos)
		{
			Sync = true;
			return;
		}
	}
        TLcd::FillRectangle(xpos,y,rightpos-xpos,SysFont[0].biHeight,SysPal[0]);
	Sync = true;
	return;
}

void TFont::SetSysFont(TBitMap const *font_ptr)
{
	SysFont = font_ptr;
}

uint16_t TFont::StringLen(const char *str)
{
	uint16_t res = 0;
  for (int i = 0; i < strlen(str); i++) res += SysFont[str[i]].biWidth;  
	return res;
}

uint16_t TFont::StringCnt(const char *str)
{
	uint16_t res = 1;
  for (int i = 0; i < strlen(str); i++) 
    if (str[i] == 0x0A) res ++;  
	return res;
}

