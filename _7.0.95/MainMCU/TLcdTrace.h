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
#ifndef TLCDTRACE_H
#define TLCDTRACE_H
//------------------------------------------------------------------------------------
#include "TLcd.h"
#include "Utilities.h"
//#include "TPushButton.h"
//#include "TSound.h"
//------------------------------------------------------------------------------------
#define ADDTRACE_DELAY 20
//------------------------------------------------------------------------------------
#define LTR_LINESNUM 10
#define LTR_LINEMAXLENGHT 21
#define LTR_LINEOFFSET 11
//------------------------------------------------------------------------------------
class TLcdTrace : public TLcd
{
  public:
  	static void AddLine(const char *str);
  	static void AddLine(const char *str,int num);
  	static void AddLineX(const char *str,int num);
  	static void AddLine(bool key, const char *str);
  	static void UpdateEnable();
  	static void UpdateLastLine(const char *str);
  	static void Clear();
  	
  	static void Tr_PB_LongPress();
  	static bool DisableLcdTrace;

  	static int StartingUpStages;
    
  protected:
  private:
  	static bool Filled;
  	static char LinesBuffer[LTR_LINESNUM][LTR_LINEMAXLENGHT];
  	static char LinesBufferBk[LTR_LINESNUM][LTR_LINEMAXLENGHT];
  	static int  CurrentLine;
  	static int  FirstLine;
  	static bool UpdatedLine;

  	static void InvalidateLines();
        static void PutStnChar(uint16_t x, uint16_t y, BYTE Char, uint16_t frontcolor, uint16_t backcolor);
        static void PutStnString(uint16_t x, uint16_t y, char* str, uint16_t frontcolor, uint16_t backcolor);
};
//------------------------------------------------------------------------------------
#endif
