/*
    Title:    
    Author:   VN Asliuk(Cardian)
    Date:     24.08.2010 16:49:08
    Purpose:  
    Software: 
    Hardware: 
    Note:    
*/
//------------------------------------------------------------------------------------
#include "TLcdTrace.h"
#include <string.h>
//#include "TGuiObjectsV2.h"
#include "TLMX9838.h"
#include "TAppProcessor.h"
//#include "StorageFlash.h"
#include "StnFont.h"

//------------------------------------------------------------------------------------
int  TLcdTrace::CurrentLine = 0;
int  TLcdTrace::FirstLine = 0;
bool TLcdTrace::Filled = false;
char TLcdTrace::LinesBuffer[LTR_LINESNUM][LTR_LINEMAXLENGHT];
char TLcdTrace::LinesBufferBk[LTR_LINESNUM][LTR_LINEMAXLENGHT];
bool TLcdTrace::UpdatedLine = false;	
bool TLcdTrace::DisableLcdTrace = false;	
int  TLcdTrace::StartingUpStages = 0;	
//------------------------------------------------------------------------------------

void TLcdTrace::UpdateEnable()
{
	UpdatedLine = true;		
}
//------------------------------------------------------------------------------------

void TLcdTrace::AddLineX(const char *str,int num)
{
#ifdef LCD_TRACE
	static char str1[10];
	static char str2[20];

	strcpy(str2,str);
	itos(num,str1,6,HEXADECIMAL);
	strcat(str2,str1);
  AddLine(str2);
#endif //LCD_TRACE
}
//------------------------------------------------------------------------------------

void TLcdTrace::AddLine(const char *str,int num)
{
#ifdef LCD_TRACE
	static char str1[10];
	static char str2[20];

	strcpy(str2,str);
	itos(num,str1,6,DECIMAL);
	strcat(str2,str1);
  AddLine(str2);
#endif //LCD_TRACE
}
//------------------------------------------------------------------------------------

void TLcdTrace::AddLine(const char *str)
{
#ifdef LCD_TRACE
//	if(DisableLcdTrace) return;
	
	bool FillSpaces = false;

	UpdatedLine = false;		
	
	for(int i = 0;i < LTR_LINEMAXLENGHT;i++)
	{
		if(str[i] == 0) FillSpaces = true;
		if(FillSpaces)	
			LinesBuffer[CurrentLine][i] = ' ';
		else	
			LinesBuffer[CurrentLine][i] = str[i];
	}
	
	LinesBuffer[CurrentLine][LTR_LINEMAXLENGHT - 1] = 0;
	
	if(Filled == true) 
		{
			FirstLine++;
			if(FirstLine >= LTR_LINESNUM)
				{
					FirstLine = 0;
				}
		}
	
	CurrentLine++;
	if(CurrentLine >= LTR_LINESNUM)
		{
			CurrentLine = 0;
			Filled = true;
		}
		
	InvalidateLines();
		
//	vTaskDelay(ADDTRACE_DELAY/portTICK_RATE_MS);	

	StartingUpStages++;
#endif //LCD_TRACE
}
//------------------------------------------------------------------------------------

void TLcdTrace::UpdateLastLine(const char *str)
{
	if(!UpdatedLine) return;
	
	bool FillSpaces = false;
	
	CurrentLine--;
	if(CurrentLine < 0)
	{
		CurrentLine = LTR_LINESNUM - 1;
	}

	for(int i = 0;i < LTR_LINEMAXLENGHT;i++)
	{
		if(str[i] == 0) FillSpaces = true;
		if(FillSpaces)	
			LinesBuffer[CurrentLine][i] = ' ';
		else	
			LinesBuffer[CurrentLine][i] = str[i];
	}
	
	LinesBuffer[CurrentLine][LTR_LINEMAXLENGHT - 1] = 0;
	
	SBYTE y = 62 - (LTR_LINEOFFSET*(LTR_LINESNUM));
		
	//Draw Lines
//	TAppProcessor::TaskFlags |= TASKF_INV;
	if(!DisableLcdTrace) 
	{
		PutStnString(0,y,LinesBuffer[CurrentLine],clBlack,clWhite);	
	}
//	vTaskResume(TLcd::xLcdInvalidate);
//	TAppProcessor::TaskFlags &= ~TASKF_INV;
	
	CurrentLine++;
	if(CurrentLine >= LTR_LINESNUM)
		{
			CurrentLine = 0;
			Filled = true;
		}
}
//------------------------------------------------------------------------------------

void TLcdTrace::AddLine(bool key, const char *str)
{
#ifdef LCD_TRACE
//	if(DisableLcdTrace) return;

	UpdatedLine = false;		

	#ifdef LCD_TRACE_STEPBYSTEP
	  if(TFlash::SF_GetData8(START_UP_SYSTEM) == 0xFF)
	  {	
			if(key)
				{
					while(!TPushButton::GetButtonTriggerState())
					{
						vTaskDelay(50/portTICK_RATE_MS);	
					}
					TPushButton::ResetButtonTrigger();
				}
			//TSound::PlaySound(_modiShortBeep);	
		}		
	#endif		
	AddLine(str);
#endif //LCD_TRACE
}
//------------------------------------------------------------------------------------

void TLcdTrace::Clear()
{
	CurrentLine = 0;
	FirstLine = 0;
	Filled = false;
	
	for(int i = 0;i < LTR_LINESNUM;i++)
		for(int j = 0;j < LTR_LINEMAXLENGHT;j++)
			{
				if(j == (LTR_LINEMAXLENGHT-1))
					LinesBuffer[i][j] = 0;
				else	
					LinesBuffer[i][j] = ' ';
			}
}
//------------------------------------------------------------------------------------

void TLcdTrace::InvalidateLines()
{
  if(!DisableLcdTrace) 	
  {
    uint16_t y = 21;

    //Draw Lines
    int _fl = FirstLine;
    for(int h = 0;h < LTR_LINESNUM;h++)
    {
      for(int i = 0;i < LTR_LINEMAXLENGHT;i++)
      {
        if(LinesBufferBk[_fl][i] != LinesBuffer[_fl][i])
        {
          PutStnChar(i*8*2,y,LinesBuffer[_fl][i],clAqua,clBlack);
          LinesBufferBk[_fl][i] = LinesBuffer[_fl][i];
        }
      }
      _fl++;
      if(_fl >= LTR_LINESNUM) _fl = 0;
      y += LTR_LINEOFFSET*2;
    }
  }
}
//------------------------------------------------------------------------------------

void TLcdTrace::Tr_PB_LongPress()
{
//	static bool gui_nongui = false;
	
	TLMX9838::TurnOnBluetooth();
	
/*	if(gui_nongui)
	{
		TGuiObjects::Disabled = false;
		DisableLcdTrace = true;
		TGuiObjects::StartDesktop();
	}	
	else	
	{	
		TGuiObjects::DisableAll();
		DisableLcdTrace = false;
		ClearScreen();
		InvalidateLines();	
	}
	gui_nongui = !gui_nongui;*/
}
//------------------------------------------------------------------------------------
void TLcdTrace::PutStnChar(uint16_t x, uint16_t y, BYTE Char, uint16_t frontcolor, uint16_t backcolor)
{
	const BYTE *FontPtr = GetTableCharFont1(Char);
	
	
//	for(SBYTE u = 11-1;u >= 0;u--)
	for(SBYTE u = 0;u < 11;u++)
		{
			BYTE PixMask = 0x01;
			
			BYTE PixByte = FontPtr[u];
			
			for(BYTE i = 0;i < 8;i++)
				{
					uint16_t PixState = backcolor;
					if(PixByte & PixMask) PixState = frontcolor;
					FillRectangle(x+8*2-i*2,y+u*2,2,2,PixState);	
					PixMask = PixMask << 1;
				}
		}
}

void TLcdTrace::PutStnString(uint16_t x, uint16_t y, char* str, uint16_t frontcolor, uint16_t backcolor)
{
	for(int i = 0;i < LTR_LINEMAXLENGHT;i++)
	{
		PutStnChar(x+i*8*2,y,str[i],frontcolor,backcolor);
	}
}
