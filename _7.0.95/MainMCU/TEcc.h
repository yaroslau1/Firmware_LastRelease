/*
    Title:
    Author:   VN Asliuk(Cardian)
    Date:     15.10.2010 16:46:47
    Purpose:
    Software:
    Hardware:
    Note:
*/
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#ifndef TECC_H
#define TECC_H
//------------------------------------------------------------------------------------
#include "ae_ext_cm3.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "TAppProcessor.h"
//#include "TNand.h"
//------------------------------------------------------------------------------------
/*****************************************************************************/
/* Integer Types                                                             */
/*****************************************************************************/

typedef enum {
	ECC_NO_ERROR			= 0,		/* no error */
	ECC_CORRECTABLE_ERROR	= 1,		/* one bit data error */
	ECC_ECC_ERROR			= 2,		/* one bit ECC error */
	ECC_UNCORRECTABLE_ERROR	= 3			/* uncorrectable error */
} eccdiff_t;


//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------

class TEcc
{
  public:
				static void make_ecc_256(unsigned char* ecc_buf, unsigned char* data_buf);
				static eccdiff_t  compare_ecc_256(uint8_t *iEccdata1, uint8_t *iEccdata2, uint8_t *pPagedata);
};        
//------------------------------------------------------------------------------------
#endif
