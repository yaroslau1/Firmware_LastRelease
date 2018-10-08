/* Aelab extentions
*/
//------------------------------------------------------------------------------------
#ifndef TAEEXT_H
#define TAEEXT_H
//------------------------------------------------------------------------------------

typedef unsigned char   BYTE;
typedef signed char     SBYTE;

typedef unsigned short    WORD;
typedef signed short      SWORD;

typedef unsigned long   DWORD;
typedef signed long     SDWORD;


typedef union
{
  BYTE   Char[2];
  WORD   Int;
} CharInt;

typedef union
{
  BYTE    Char[4];
  float   Float;
} CharFloat;

typedef union
{
  BYTE    Char[4];
  DWORD   Long;
} CharLong;

typedef union 
{
	long L;
	float F;
} LF_t;
//------------------------------------------------------------------------------------
#endif  

