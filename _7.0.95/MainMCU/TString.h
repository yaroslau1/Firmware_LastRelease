/*
    Title:    TString C++ header
    Author:   VN Asliuk
    Date:     28.09.2004 16:02
    Purpose:  For AELAB projects
    Software: IAR EW C/EC++ AVR compilers
    Hardware: ATMEGAXXX
    Note:     Working with char arrays
*/
//------------------------------------------------------------------------------------
#ifndef TSTRING_H
#define TSTRING_H

#include "main.h"
//------------------------------------------------------------------------------------
//#include "TLog.h"
//#include "Bcd_F.h"
//------------------------------------------------------------------------------------
#define MAX_STRING_LENGHT 60 //for StrLen fn
//------------------------------------------------------------------------------------
class TString
{
  public:
//    static char* IntToStr(SWORD _IntNum, bool _IsSigned);
//    static char* DWORDToStr(DWORD _IntNum);
//
//    #ifdef STRTOINT_WITH_SIGNED_NUMBERS
//      static SWORD StrToInt(char* _Str);
//    #else
//      static WORD StrToInt(char* _Str);
//    #endif
//
//    static float StrToFloat(char* _Str);
//    static char* FloatToStr(float _FloatNum, BYTE _Precision);
//    static char*  StrCpy(char* Dest, char __generic *Src);
//    static char*  StrCpy(char* Dest, char __eeprom *Src);
//    static char*  StrCpy(char* Dest, char __flash *Src);
    static char * StrCpy( char *Dest, char *Src );
//    static char*  StrCat(char* Dest, char __generic *Src);
//    static BYTE  StrLen(char __eeprom *Src);
//    static BYTE  StrLen(char __generic *Src);
};
//------------------------------------------------------------------------------------
#endif
