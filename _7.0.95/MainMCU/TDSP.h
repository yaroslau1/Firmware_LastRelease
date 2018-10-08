#ifndef TDSP_H
#define TDSP_H

#include "main.h"
//#include "TDevice.h"

/* Moving Average points count */
#define N_MA_HP		512		/* ��� ������, ��� ������ �� ������ �����������, ������ 2 */
//#define N_MA_HP_2H	16


/*----------------------------------------------------------------------------------*/
class TDSP //: public TDevice
{
	public:
		/* ������ �� ��� ���������� ���-������ */
		static int FF_LP_12( int data );
		static int RFF_LP_12( int data );

		/* ������� �� ��� �������� ��� �� 50�� */
		static int FF_Butterworth_HF(int32_t data, uint8_t ch);
                
                /* ������ ��������� 50- 120 ��*/
                static int Butterworth_Ecg(int32_t data, uint8_t ch);
                
                //ECG Butterworth HighPass 2-order; 0.2 Hz; 250 SR
                static int Butterworth_Ecg_HF(int data, uint8_t ch);
                
             

		/* ����������� ������� */
		static void FF_Median_HS0( int *data );
		static void FF_Median_HS1( int *data );
		static void FF_Median_HS2( int *data );

		/* ������� ����������� �������� */
		static int FFHigh_MA0( int X_i, int *Mid, bool Reset );
		static int FFHigh_MA1( int X_i, int *Mid, bool Reset );
		static int FFHigh_MA2( int X_i, int *Mid, bool Reset );
		static int FFHigh_MA3( int X_i, int *Mid, bool Reset );

		static int MA_HP_Arr0[ N_MA_HP ];
		static int MA_HP_Arr1[ N_MA_HP ];
		static int MA_HP_Arr2[ N_MA_HP ];
//		static int MA_HP_Arr3[ N_MA_HP_2H ];

		/* �� ������ */
		static int32_t FHigh_0_32_Fixed(int32_t X_i, uint8_t ch, bool Reset, uint8_t ZeroAdduction);
		static int32_t FHigh_0_32_Fixed_Resp( int32_t X_i, bool Reset );

	private:

	protected:
};

#endif