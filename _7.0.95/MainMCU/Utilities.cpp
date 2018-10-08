/**
  ******************************************************************************
  * @file Utilities.cpp
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
#include "Utilities.h"
#include <string.h>
//------------------------------------------------------------------------------------

void FillSrcBuffer(BYTE *Buffer,BYTE Byte2Fill)
{
	for(int i = 0;i < 1024;i++) Buffer[i] = Byte2Fill;
}
//------------------------------------------------------------------------------------

void ClearSrcBuffer(BYTE *Buffer)
{
	for(int i = 0;i < 1024;i++) Buffer[i] = 0;
}
//------------------------------------------------------------------------------------

void CopySrcBuffer(BYTE *DstBuffer,BYTE *SrcBuffer)
{
	for(int i = 0;i < 1024;i++) DstBuffer[i] = SrcBuffer[i];
}
//------------------------------------------------------------------------------------

void itos(int val,char *buf,int size,Radix r)
{
	char *offset = buf;   // a pointer to a string
  char *first = offset; // a pointer to
                        // the first digit
  int delta_y;          // the value of a digit
  int count = 0;        // a number of digits
                        // in the string

  // val is negative, so let's add '-' to
  // the begin of the string.
  if(val < 0) 
  	{
    	*offset++ = '-';
      val = -val;
      first = offset;
    }
    
  if(val == 0) 
  	{
  		int g = 0;
		  if(r == HEXADECIMAL) 
  			{
  				buf[g++] = 'x';
  				buf[g++] = '0';
  			}
  		buf[g++] = '0';
  		buf[g] = 0;
  		return;
  	}  

  // Take special actions for hexademical and
  // octople countable systems and insert to the
  // begin of the string either the "\0x" or '0'
  // prefixes respectively.
  if(r == HEXADECIMAL) 
  	{
//      *offset++ = '0';
      *offset++ = 'x';
      first = offset;
    }
  else if (r == OCTOPLE) 
  	{
      *offset++ = '0';
      first = offset;
    }

  // Read and store digits into the string.
  while((val > 0) && (size-- >= 0))
  	{
    	delta_y = (int)(val % r);
      val /= r;

      if (delta_y > 9) // The hexademical format.
      	*offset++ = (delta_y - 10 + 'A');
      else  // Binary, decimal and octople formats.
        *offset++ = (delta_y + '0');
      count++;
    }

  // Points to the last digit.
  *offset-- = '\0';

  // Now our string corresponds to the integer
  // digits but it is in the reverse order. So make
  // the backward transformation of it. If it has
  // only two characters just swap them without any
  // additional actions.
  if(count == 1)
  	return;
  else if (count == 2) 
  	{
    	char temp = (char)*offset;
      *offset-- = *first;
      *first++ = temp;      
    }
  // For more than two characters in the string
  // reverse full string in the folowing cycle.
  else 
  	{
    	delta_y = count / 2;

      while (delta_y--) 
      	{
        	char temp = (char)*first;
          *first++ = *offset;
          *offset-- = temp;
        }
    }
}
//------------------------------------------------------------------------------------

void itodts(int val,char *buf,int size)
{
	char *offset = buf;   // a pointer to a string
  char *first = offset; // a pointer to
                        // the first digit
  int delta_y;          // the value of a digit
  int count = 0;        // a number of digits
                        // in the string

	int valfix = val;
  // val is negative, so let's add '-' to
  // the begin of the string.
  if(val <= 0) 
  	{
  		buf[0] = '0';
  		buf[1] = '0';
  		buf[2] = 0;
  		return;
  	}  
  	
  // Read and store digits into the string.
  while((val > 0) && (size-- >= 0))
  	{
    	delta_y = (int)(val % DECIMAL);
      val /= DECIMAL;

      if (delta_y > 9) // The hexademical format.
      	*offset++ = (delta_y - 10 + 'a');
      else  // Binary, decimal and octople formats.
        *offset++ = (delta_y + '0');
      count++;
    }
  if(valfix < 10)
  	{
  		*offset++ = '0';
  		count++;
  	}	

  // Points to the last digit.
  *offset-- = '\0';

  // Now our string corresponds to the integer
  // digits but it is in the reverse order. So make
  // the backward transformation of it. If it has
  // only two characters just swap them without any
  // additional actions.
  if(count == 1)
  	return;
  else if (count == 2) 
  	{
    	char temp = (char)*offset;
      *offset-- = *first;
      *first++ = temp;      
    }
  // For more than two characters in the string
  // reverse full string in the folowing cycle.
  else 
  	{
    	delta_y = count / 2;

      while (delta_y--) 
      	{
        	char temp = (char)*first;
          *first++ = *offset;
          *offset-- = temp;
        }
    }
}
//------------------------------------------------------------------------------------

void ix10tosAsFloat(int val,char *buf,int size)
{
	itos(val,buf,size);
	
	int dlen = strlen(buf);
	//Offset last digit and add point
	
	
	if(val < 10)
		{
			buf[dlen+1] = buf[dlen-1];
			buf[dlen] = '.';
			buf[dlen-1] = '0';
			buf[dlen+2] = 0;
		}
	else
		{
			buf[dlen] = buf[dlen-1];
			buf[dlen-1] = '.';
			buf[dlen+1] = 0;
		}	
}
//------------------------------------------------------------------------------------

#pragma diag_suppress=Pe940
int reverseBits(int val)
{
  asm("rbit r0, r0");
//  asm("bx lr");
}
//------------------------------------------------------------------------------------

char *ftoa(float f,int Fract)
{
	long mantissa, int_part, frac_part;
	short exp2;
	LF_t x;
	char *p;
	static char outbuf[15];

	if(f == 0.0)
		{
			outbuf[0] = '0';
			outbuf[1] = '.';
			outbuf[2] = '0';
			outbuf[3] = 0;
			return outbuf;
		}
	
	x.F = f;
	exp2 = (unsigned char)(x.L >> 23) - 127;
	mantissa = (x.L & 0xFFFFFF) | 0x800000;
	frac_part = 0;
	int_part = 0;

	if(exp2 >= 31)
	{
		return 0;
	}
	else if (exp2 < -23)
		{
			return 0;
		}
		else if (exp2 >= 23)
			{
				int_part = mantissa << (exp2 - 23);
			}	
			else if (exp2 >= 0) 
				{
					int_part = mantissa >> (23 - exp2);
					frac_part = (mantissa << (exp2 + 1)) & 0xFFFFFF;
				}
				else /* if (exp2 < 0) */
					{
						frac_part = (mantissa & 0xFFFFFF) >> -(exp2 + 1);
					}
	
	p = outbuf;

	if(x.L < 0)
		*p++ = '-';

	if (int_part == 0)
		*p++ = '0';
	else
		{
			itos(int_part,p,10);
			while (*p)
				p++;
		}
	*p++ = '.';

	if(frac_part == 0)
		*p++ = '0';
	else
		{
			char m, max;

			max = sizeof (outbuf) - (p - outbuf) - 1;
			if (max > Fract)
			max = Fract;
			/* print BCD */
			for(m = 0; m < max; m++)
				{
					/* frac_part *= 10; */
					frac_part = (frac_part << 3) + (frac_part << 1); 

					*p++ = (frac_part >> 24) + '0';
					frac_part &= 0xFFFFFF;
				}
			/* delete ending zeroes */
			for (--p; p[0] == '0' && p[-1] != '.'; --p) ;
			++p;
		}

	*p = 0;

	return outbuf;
}
//------------------------------------------------------------------------------------
