/*
    Title:    TString C++ unit
    Author:   VN Asliuk
    Date:     21/02/2002
    Purpose:  For AELAB projects
    Software: IAR EW C/EC++ AVR compilers
    Hardware: ATMEGA323 on A1CBR.IB.1-3 board
    Note:     Working with char arrays
*/
//------------------------------------------------------------------------------------
#include "TString.h"
//#include "Bcd_F.h" //This file must be included always
#include <math.h>
////------------------------------------------------------------------------------------
//char __flash vPoint[] = {"."};
//char __flash vZero[] = {"0"};
////------------------------------------------------------------------------------------
//
//char* TString::DWORDToStr(DWORD _IntNum)
//{
//  WORD TmpByte = (WORD)(_IntNum & 0x0000FFFF);
//  return IntToStr(TmpByte,false);
//}
////------------------------------------------------------------------------------------
//
//char* TString::IntToStr(SWORD _IntNum, bool _IsSigned)
//{
//  static char _ResultStr[7];
//  BYTE StrIndx = 0;
//  CharLong ResBytes;
//
//  if(_IntNum == 0) {_ResultStr[0] = '0'; _ResultStr[1] = 0; return _ResultStr;}
//  if(_IsSigned)
//    {
//      if(_IntNum < 0) _ResultStr[StrIndx++] = '-';
//      ResBytes.Long = BinToBCD_16(abs(_IntNum));
//    }
//  else
//      ResBytes.Long = BinToBCD_16(_IntNum);
//
//  bool IsZero = false;
//  for(SBYTE i = 2;i >= 0;i--)
//    {
//      BYTE BHigh,BLow;
//
//      BHigh = __swap_nibbles(ResBytes.Char[i] & 0xF0);
//      if(BHigh != 0) IsZero = true;
//      if(IsZero)  _ResultStr[StrIndx++] = BHigh + 0x30;
//      BLow = ResBytes.Char[i] & 0x0F;
//      if(BLow != 0) IsZero = true;
//      if(IsZero)  _ResultStr[StrIndx++] = BLow + 0x30;
//    }
//  _ResultStr[StrIndx++] = 0;
//  return _ResultStr;
//}
//
////------------------------------------------------------------------------------------
//char* TString::FloatToStr(float _FloatNum, BYTE _Precision)
//{
//  WORD FMant;
//  static char FStr[10];
//
//  FMant = (WORD)fabs(_FloatNum);
//  if(_FloatNum < 0)
//    {
//      FStr[0] = '-';
//      FStr[1] = 0;
//      StrCat(FStr,IntToStr(FMant,0));
//    }
//  else
//    {
//      StrCpy(FStr,IntToStr(FMant,0));
//    }
//  if(_Precision != 0)
//    {
//      StrCat(FStr,vPoint);
//      float FTmp = (fabs(_FloatNum) - (float)FMant);
////      _FloatNum = (float)(15.4 - 15);
//      switch(_Precision)
//        {
//          case 1:
//                  FTmp *= 10.0;
//                  break;
//          case 2:
//                  FTmp *= 100.0;
//                  break;
//          default:
//                  FTmp *= 1000.0;
//        }
//      FMant = (WORD)FTmp;
//      char* Fract = IntToStr(FMant,0);
//      for(SBYTE j = 0;j < (_Precision - StrLen(Fract));j++) StrCat(FStr,vZero);
//      StrCat(FStr,Fract);
//    }
//  return FStr;
//}
////------------------------------------------------------------------------------------
//
//#ifdef STRTOINT_WITH_SIGNED_NUMBERS
//SWORD TString::StrToInt(char* _Str)
//  {
//    CharLong  ResWord;
//
//    ResWord.Long = 0;
//
//    BYTE SrcIndx = 0;
//    BYTE IsNegative = 0;
//    BYTE StrIndx = StrLen(_Str);
//
//    if(StrIndx == 0) return NULL;
//    StrIndx--;
//    if(_Str[0] == '-') IsNegative = 1;
//
//    while(1)
//      {
//        if(((BYTE)(_Str[StrIndx] - 0x30)) > 9) return NULL;
//
//        ResWord.Char[SrcIndx] = (_Str[StrIndx] - 0x30); //Set low nibble
//        if(StrIndx == IsNegative) break;//return ResWord.Long;
//        StrIndx--;
//        ResWord.Char[SrcIndx] |= __swap_nibbles(_Str[StrIndx] - 0x30); //Set high nibble
//        if(StrIndx == IsNegative) break;//return ResWord.Long;
//        StrIndx--;
//        SrcIndx++;
//      }
//    SWORD ResNum = BCDToBin_16(ResWord.Long);
//    if(IsNegative == 1) ResNum = -ResNum;
//    return ResNum;
//  }
//#else
//WORD TString::StrToInt(char* _Str)
//  {
//    CharLong  ResWord;
//
//    ResWord.Long = 0;
//
//    BYTE SrcIndx = 0;
//    BYTE StrIndx = StrLen(_Str);
//
//    if(StrIndx == 0) return NULL;
//    StrIndx--;
//
//    while(1)
//      {
//        if(((BYTE)(_Str[StrIndx] - 0x30)) > 9) return NULL;
//
//        ResWord.Char[SrcIndx] = (_Str[StrIndx] - 0x30); //Set low nibble
//        if(StrIndx == 0) break;//return ResWord.Long;
//        StrIndx--;
//        ResWord.Char[SrcIndx] |= __swap_nibbles(_Str[StrIndx] - 0x30); //Set high nibble
//        if(StrIndx == 0) break;//return ResWord.Long;
//        StrIndx--;
//        SrcIndx++;
//      }
//    return BCDToBin_16(ResWord.Long);
//  }
//#endif
////------------------------------------------------------------------------------------
//
//float TString::StrToFloat(char* _Str) // Working only with format XXX.X
//{
//  float Res = 0.0;
//  BYTE StrL = StrLen(_Str);
//  BYTE _Tmp = _Str[--StrL] - 0x30;
//  Res = _Tmp * 0.1;
//  _Str[--StrL] = 0;
//  Res += (float)StrToInt(_Str);
//  return Res;
//}
////------------------------------------------------------------------------------------
//
//BYTE  TString::StrLen(char __eeprom *Src)
//{
//  BYTE Lenght = 0;
//  while(Lenght != MAX_STRING_LENGHT) if(Src[Lenght++] == 0) return --Lenght;
//  return NULL;
//}
////------------------------------------------------------------------------------------
//
//BYTE  TString::StrLen(char __generic *Src)
//{
//  BYTE Lenght = 0;
//  while(Lenght != MAX_STRING_LENGHT) if(Src[Lenght++] == 0) return --Lenght;
//  return NULL;
//}
////------------------------------------------------------------------------------------
//
//char* TString::StrCpy(char* Dest, char __eeprom *Src)
//{
//  BYTE i;
//  BYTE SrcL = StrLen(Src);
//  for(i = 0;i < SrcL;i++)
//    Dest[i] = Src[i];
//  Dest[i] = 0;
//  return Dest;
//}
////------------------------------------------------------------------------------------
//
//char* TString::StrCpy(char* Dest, char __generic *Src)
//{
//  BYTE i;
//  BYTE SrcL = StrLen(Src);
//  for(i = 0;i < SrcL;i++)
//    Dest[i] = Src[i];
//  Dest[i] = 0;
//  return Dest;
//}
////------------------------------------------------------------------------------------

char *TString::StrCpy(char *Dest, char *Src)
{
  uint8_t i;
//  uint8_t SrcL = StrLen(Src);
  uint8_t SrcL = strlen( Src );
  for(i = 0;i < SrcL;i++)
    Dest[i] = Src[i];
  Dest[i] = 0;
  return Dest;
}
////------------------------------------------------------------------------------------
//
//char* TString::StrCat(char* Dest, char __generic *Src)
//{
//  BYTE i;
//  BYTE DestL = StrLen(Dest);
//  BYTE SrcL = StrLen(Src);
//  for(i = 0;i < SrcL;i++)
//    Dest[i + DestL] = Src[i];
//  Dest[i + DestL] = 0;
//  return Dest;
//}
////------------------------------------------------------------------------------------
//
//char* TString::StrCpy(char* Dest, char __flash *Src)
//{
//  BYTE i;
//  BYTE SrcL = StrLen(Src);
//  for(i = 0;i < SrcL;i++)
//    Dest[i] = Src[i];
//  Dest[i] = 0;
//  return Dest;
//}
////------------------------------------------------------------------------------------
