/************************************************************************************/
/* Title:		Digital processing													*/
/* Author:		Cyber Genius														*/
/* Version:		1.0																	*/
/* Date:		18.07.2012															*/
/************************************************************************************/
/*																					*/
/************************************************************************************/
#include "TDSP.h"
#include "main.h"

int TDSP::MA_HP_Arr0[ N_MA_HP ];
int TDSP::MA_HP_Arr1[ N_MA_HP ];
int TDSP::MA_HP_Arr2[ N_MA_HP ];

//int TDSP::MA_HP_Arr3[ N_MA_HP_2H ];

/*----------------------------------------------------------------------------------*/
/* Фильтр НЧ 12 порядка, срез на 15 Гц								*/
/*----------------------------------------------------------------------------------*/
float xval[12];
int TDSP::FF_LP_12( int data )
{
	const float k0 = 0.0067;
	const float k1 = 0.0244;
	const float k2 = 0.0569;
	const float k3 = 0.1001;
	const float k4 = 0.1424;
	const float k5 = 0.1688;
	const int gain = 4;
	float y0;

	for (int i = 0; i < 11; i++) xval[i] = xval[i+1];
	xval[11] = (float)data;
	y0 =  gain * (int)(((xval[0] + xval[11]) * k0) + ((xval[1] + xval[10]) * k1) + ((xval[2] + xval[9]) * k2) + ((xval[3] + xval[8]) * k3) + ((xval[4] + xval[7]) * k4) + ((xval[5] + xval[6]) * k5));
	if (y0 > 32000/gain) y0 = 32000/gain;
	if (y0 < -32000/gain) y0 = -32000/gain;
	return (int) y0;
}

/*----------------------------------------------------------------------------------*/
/* Рекурсивный фильтр НЧ 4 порядка, срез на ~1.25 Гц								*/
/*----------------------------------------------------------------------------------*/
int TDSP::RFF_LP_12( int x0 )
{
	static double x[5] = { 0.0, 0.0, 0.0, 0.0, 0.0};
	static double y[5] = { 0.0, 0.0, 0.0, 0.0, 0.0};
	const double a[5] = {1, -3.91790786539199, 5.75707637911807, 
		-3.76034950769453, 0.921181929191236};
	const double b[5] = {5.84514243118228e-08, 2.33805697247291e-07,
		3.50708545870937e-07, 2.33805697247291e-07, 5.84514243118228e-08};
/*	const double a[5] = {1.0, 0.0, 0.0, 0.0, 0.0};
	const double b[5] = {9.60921115487302E-3, 9.61395506134236E-3, 9.61553667567569E-1, 9.61395506134236E-3, 9.60921115487302E-3};
*/	const int gain = 4;
	const int n = 4;

	double y0;
	y0 = b[0] * (double) (x0);
	for (int i = n; i > 0; i--) {
		x[i] = x[i - 1]; 
		y[i] = y[i - 1]; 
		y0 += b[i] * x[i] - a[i] * y[i]; 
	}
	y0 = y0 / a[0];
	x[0] = (double) (x0);
	y[0] = y0;
        y0 = gain * y0;
	if (y0 > 32000) y0 = 32000;
	if (y0 < -32000) y0 = -32000;
	return ((int)(y0));
}




/*----------------------------------------------------------------------------*/
/* Фильтр ВЧ Баттерворта первого порядка, срез на 50 Гц                       */
/* http://www-users.cs.york.ac.uk/~fisher/mkfilter/                           */
/*----------------------------------------------------------------------------*/
int32_t TDSP::FF_Butterworth_HF(int32_t data, uint8_t ch)
{
  const float div = 4.077683537e+00;
  const float koef = -0.5095254495;
  const float gain = 2.1;
  
  static float xv[2][16], yv[2][16];
  
  const uint8_t ch_num = 16;
  
  if(ch >= ch_num)
  {
    return 0;
  }

  xv[0][ch] = xv[1][ch];
  xv[1][ch] = (float)(data) / div;
  
  yv[0][ch] = yv[1][ch];
  yv[1][ch] = (xv[1][ch] - xv[0][ch]) + (koef * yv[0][ch]);
  
  return (int32_t)((yv[1][ch] * gain) + (float)(data));
}

/*----------------------------------------------------------------------------*/
/* Фильтр Баттерворта полосовой первого порядка, 50 - 120 Гц                  */
/* + ВЧ второго порядка 0.2 Гц ЧД = 250 Гц */
/* http://www-users.cs.york.ac.uk/~fisher/mkfilter/                           */
/*----------------------------------------------------------------------------*/
int32_t TDSP::Butterworth_Ecg(int32_t data, uint8_t ch)
{
  const int nzeros = 2;
  const int npoles = 2;
  
  const float gain_BP = 1.750868303e+00;
  const float gain_HF = 1.003560630e+00;
  
  static float xv_HF[nzeros+1][16], yv_HF[nzeros+1][16];
  
  static float xv_BP[nzeros+1][16], yv_BP[nzeros+1][16];
  
  const uint8_t ch_num = 16;
  
  if(ch >= ch_num)
  {
    return 0;
  }

  xv_HF[0][ch] = xv_HF[1][ch];
  xv_HF[1][ch] = xv_HF[2][ch];
  
  xv_HF[2][ch] = data / gain_HF;
  
  yv_HF[0][ch] = yv_HF[1][ch];
  yv_HF[1][ch] = yv_HF[2][ch];
  
  yv_HF[2][ch] =   (xv_HF[0][ch] + xv_HF[2][ch]) - 2 * xv_HF[1][ch]
                     + ( -0.9929165937 * yv_HF[0][ch]) + (  1.9928914171 * yv_HF[1][ch]);
  
    
  
  xv_BP[0][ch] = xv_BP[1][ch];
  xv_BP[1][ch] = (float)(data) / gain_BP;
  
  yv_BP[0][ch] = yv_BP[1][ch];
  yv_BP[1][ch] = yv_BP[2][ch];
  
  yv_BP[2][ch] = (xv_BP[2][ch] - xv_BP[0][ch])
                     + (  0.0945278312 * yv_BP[0][ch]) + ( -0.7611515380 * yv_BP[1][ch]);
  
  
  
  
  return yv_BP[2][ch] + yv_HF[2][ch];
}

//ECG Bessel HighPass 2-order; 0.2 Hz; 250 SR
int TDSP::Butterworth_Ecg_HF(int data, uint8_t ch)
{
  const int nzeros = 2;
  const int npoles = 2;
  const float gain = 1.003560630e+00;
  
  const uint8_t ch_num = 16;
  
  static float xv[nzeros+1][16], yv[npoles+1][16];
  
  if(ch >= ch_num)
  {
    return 0;
  }
 
  xv[0][ch] = xv[1][ch];
  xv[1][ch] = xv[2][ch];
  
  xv[2][ch] = data / gain;
  
  yv[0][ch] = yv[1][ch];
  yv[1][ch] = yv[2][ch];
  
  yv[2][ch] =   (xv[0][ch] + xv[2][ch]) - 2 * xv[1][ch]
                     + ( -0.9929165937 * yv[0][ch]) + (  1.9928914171 * yv[1][ch]);
  
  return yv[2][ch];
}

/*----------------------------------------------------------------------------*/
/* Фильтр ВЧ, нужен чтобы привести кардиограмму к нулю                        */
/*----------------------------------------------------------------------------*/
/* X_i - вх. сигнал, ch - канал, reset - сброс                                */
/*----------------------------------------------------------------------------*/
int32_t TDSP::FHigh_0_32_Fixed(int32_t X_i, uint8_t ch, bool Reset, uint8_t ZeroAdduction)
{
  const uint8_t ch_num = 16;
  static int32_t X_i_1[ch_num], Y_i_1[ch_num];
  
  if(ch >= ch_num)
  {
    return 0;
  }

  int64_t Y_i;
	int32_t _X_i;
	int32_t d_Y = 0;
  int32_t koef = 9991;

  if(Reset)
  {
    X_i_1[ch] = 0;
    Y_i_1[ch] = 0;
    return 0;
  }

  // Алгоритм быстрого приведения сигнала к нулю
  if( ZeroAdduction == 1 )
  {
    if(( Y_i_1[ch] > 25000 ) || ( Y_i_1[ch] < -25000 ))
    {
      koef = 9000L;
    }
  }
  else if(ZeroAdduction == 2)
  {
    koef = 9000L;
  }
  else if( ZeroAdduction == 3 )
  {
    if(Y_i_1[ch] > 25000)
    {
      d_Y = -15000;
    }
    else if(Y_i_1[ch] < -25000)
    {
      d_Y = 15000;
    }
  }

	_X_i = X_i;

	Y_i = (int32_t)((int64_t)(koef) * ((int64_t)( Y_i_1[ch] + _X_i - X_i_1[ch] )) / 10000L) + d_Y;		/* чем больше koef, тем точнее сигнал, но медленее выплывает и наоборот */
	Y_i_1[ch] = Y_i;
	X_i_1[ch] = _X_i;
	return (Y_i);
}

/*----------------------------------------------------------------------------------*/
/* Фильтр ВЧ, нужен чтобы привести реограмму к нулю								*/
/*----------------------------------------------------------------------------------*/
/* X_i - вх. сигнал, ch - канал, reset - сброс										*/
/*----------------------------------------------------------------------------------*/
int32_t TDSP::FHigh_0_32_Fixed_Resp( int32_t X_i, bool Reset )
{
	static int32_t X_i_1, Y_i_1;
	int64_t Y_i;
	static int32_t koef = 9991L;

	if( Reset ){
		X_i_1 = 0;
		Y_i_1 = 0;
		koef = 9991L;
		return 0;
	}

	int32_t _X_i;


	_X_i = X_i;
	_X_i = X_i;

	Y_i = (int64_t) koef * (int64_t)( Y_i_1 + _X_i - X_i_1 );		/* чем больше koef, тем точнее сигнал, но медленее выплывает и наоборот */
	Y_i_1 = Y_i / 10000L;
	Y_i = Y_i_1;
	if (Y_i > 2000000) Y_i = 2000000;
	if (Y_i < -2000000) Y_i = -2000000;
	X_i_1 = _X_i;
	return ( Y_i );
}



/*----------------------------------------------------------------------------------*/
/* Усредняющий фильтр для подавления помех											*/
/*----------------------------------------------------------------------------------*/
/* *data - указатель на данные														*/
/*----------------------------------------------------------------------------------*/
void TDSP::FF_Median_HS0(int *data)
{
	int iA = 0;
	static int iB = 0;
	static int iC = 0;

	iA = *data;

	BYTE eqv = 0;

	if(iA > iB) eqv |= 0x04;
	if(iA > iC) eqv |= 0x02;
	if(iB > iC) eqv |= 0x01;

	if((eqv == 0) || (eqv & 0x07)) *data = iB;
	else
		if((eqv & 0x01)	|| (eqv & 0x06)) *data = iC;
		else *data = iA;

 	iC = iB;
 	iB = iA;
}



/*----------------------------------------------------------------------------------*/
/* Усредняющий фильтр для подавления помех											*/
/*----------------------------------------------------------------------------------*/
/* *data - указатель на данные														*/
/*----------------------------------------------------------------------------------*/
void TDSP::FF_Median_HS1(int *data)
{
	int iA = 0;
	static int iB = 0;
	static int iC = 0;

	iA = *data;

	BYTE eqv = 0;

	if(iA > iB) eqv |= 0x04;
	if(iA > iC) eqv |= 0x02;
	if(iB > iC) eqv |= 0x01;

	if((eqv == 0) || (eqv & 0x07)) *data = iB;
	else
		if((eqv & 0x01)	|| (eqv & 0x06)) *data = iC;
		else *data = iA;

 	iC = iB;
 	iB = iA;
}



/*----------------------------------------------------------------------------------*/
/* Усредняющий фильтр для подавления помех											*/
/*----------------------------------------------------------------------------------*/
/* *data - указатель на данные														*/
/*----------------------------------------------------------------------------------*/
void TDSP::FF_Median_HS2(int *data)
{
	int iA = 0;
	static int iB = 0;
	static int iC = 0;

	iA = *data;

	BYTE eqv = 0;

	if(iA > iB) eqv |= 0x04;
	if(iA > iC) eqv |= 0x02;
	if(iB > iC) eqv |= 0x01;

	if((eqv == 0) || (eqv & 0x07)) *data = iB;
	else
		if((eqv & 0x01)	|| (eqv & 0x06)) *data = iC;
		else *data = iA;

 	iC = iB;
 	iB = iA;
}



/*----------------------------------------------------------------------------------*/
/* Фильтр скользящего среднего НЧ													*/
/*----------------------------------------------------------------------------------*/
/* X_i - вх. сигнал, *Mid - среднее значение, если не используется то NULL			*/
/* reset - сброс																	*/
/*----------------------------------------------------------------------------------*/
int TDSP::FFHigh_MA0( int X_i, int *Mid, bool Reset )
{
	static int ACC;
	static int ACCAddIndx;
	static int HP_Ch_A_Indx;
	int Y_i;

	int HP_A_Indx = (0 * N_MA_HP) + HP_Ch_A_Indx;

	if( Reset ){
		ACC = 0;
		ACCAddIndx = 0;
		HP_Ch_A_Indx = 0;
		return 0;
	}
	if( ACCAddIndx < N_MA_HP ){
		ACC += X_i;
		MA_HP_Arr0[HP_A_Indx] = X_i;
		HP_Ch_A_Indx++;
		if(HP_Ch_A_Indx >= N_MA_HP)
			HP_Ch_A_Indx = 0;

		ACCAddIndx++;
		return X_i;
	}
	else {
		ACC = ACC + X_i - MA_HP_Arr0[HP_A_Indx];

		int X_i_m64 = HP_A_Indx + (N_MA_HP/2);
		if(X_i_m64 >= N_MA_HP)
			X_i_m64 = X_i_m64 - N_MA_HP;

//		Y_i = X_i - (ACC / N_MA_HP);
		Y_i = MA_HP_Arr0[X_i_m64] - (ACC / N_MA_HP);
		if(Mid != NULL)
			*Mid = (ACC / N_MA_HP);

		MA_HP_Arr0[HP_A_Indx] = X_i;
		HP_Ch_A_Indx++;
		if(HP_Ch_A_Indx >= N_MA_HP)
			HP_Ch_A_Indx = 0;
	}
	return Y_i;
}



/*----------------------------------------------------------------------------------*/
/* Фильтр скользящего среднего НЧ													*/
/*----------------------------------------------------------------------------------*/
/* X_i - вх. сигнал, *Mid - среднее значение, если не используется то NULL			*/
/* reset - сброс																	*/
/*----------------------------------------------------------------------------------*/
int TDSP::FFHigh_MA1( int X_i, int *Mid, bool Reset )
{
	static int ACC;
	static int ACCAddIndx;
	static int HP_Ch_A_Indx;
	int Y_i;

	int HP_A_Indx = (0 * N_MA_HP) + HP_Ch_A_Indx;

	if(Reset)
	{
		ACC = 0;
		ACCAddIndx = 0;
		HP_Ch_A_Indx = 0;
		return 0;
	}
	if(ACCAddIndx < N_MA_HP)
	{
		ACC += X_i;
		MA_HP_Arr1[HP_A_Indx] = X_i;
		HP_Ch_A_Indx++;
		if(HP_Ch_A_Indx >= N_MA_HP)
			HP_Ch_A_Indx = 0;

		ACCAddIndx++;
		return X_i;
	}
	else
	{
		ACC = ACC + X_i - MA_HP_Arr1[HP_A_Indx];

		int X_i_m64 = HP_A_Indx + (N_MA_HP/2);
		if(X_i_m64 >= N_MA_HP)
			X_i_m64 = X_i_m64 - N_MA_HP;

//		Y_i = X_i - (ACC / N_MA_HP);
		Y_i = MA_HP_Arr1[X_i_m64] - (ACC / N_MA_HP);
		if(Mid != NULL)
			*Mid = (ACC / N_MA_HP);

		MA_HP_Arr1[HP_A_Indx] = X_i;
		HP_Ch_A_Indx++;
		if(HP_Ch_A_Indx >= N_MA_HP)
			HP_Ch_A_Indx = 0;
	}
	return Y_i;
}



/*----------------------------------------------------------------------------------*/
/* Фильтр скользящего среднего НЧ													*/
/*----------------------------------------------------------------------------------*/
/* X_i - вх. сигнал, *Mid - среднее значение, если не используется то NULL			*/
/* reset - сброс																	*/
/*----------------------------------------------------------------------------------*/
int TDSP::FFHigh_MA2( int X_i, int *Mid, bool Reset )
{
	static int ACC;
	static int ACCAddIndx;
	static int HP_Ch_A_Indx;
	int Y_i;

	int HP_A_Indx = (0 * N_MA_HP) + HP_Ch_A_Indx;

	if(Reset)
	{
		ACC = 0;
		ACCAddIndx = 0;
		HP_Ch_A_Indx = 0;
		return 0;
	}
	if(ACCAddIndx < N_MA_HP)
	{
		ACC += X_i;
		MA_HP_Arr2[HP_A_Indx] = X_i;
		HP_Ch_A_Indx++;
		if(HP_Ch_A_Indx >= N_MA_HP)
			HP_Ch_A_Indx = 0;

		ACCAddIndx++;
		return X_i;
	}
	else
	{
		ACC = ACC + X_i - MA_HP_Arr2[HP_A_Indx];

		int X_i_m64 = HP_A_Indx + (N_MA_HP/2);
		if(X_i_m64 >= N_MA_HP)
			X_i_m64 = X_i_m64 - N_MA_HP;

//		Y_i = X_i - (ACC / N_MA_HP);
		Y_i = MA_HP_Arr2[X_i_m64] - (ACC / N_MA_HP);
		if(Mid != NULL)
			*Mid = (ACC / N_MA_HP);

		MA_HP_Arr2[HP_A_Indx] = X_i;
		HP_Ch_A_Indx++;
		if(HP_Ch_A_Indx >= N_MA_HP)
			HP_Ch_A_Indx = 0;
	}
	return Y_i;
}



/*----------------------------------------------------------------------------------*/
/* Фильтр скользящего среднего НЧ, жёсткий для ЧСС									*/
/*----------------------------------------------------------------------------------*/
/* X_i - вх. сигнал, *Mid - среднее значение, если не используется то NULL			*/
/* reset - сброс																	*/
/*----------------------------------------------------------------------------------*/
//int TDSP::FFHigh_MA3( int X_i, int *Mid, bool Reset )
//{
//	static int ACC;
//	static int ACCAddIndx;
//	static int HP_Ch_A_Indx;
//	int Y_i;
//
//	int HP_A_Indx = (0 * N_MA_HP_2H) + HP_Ch_A_Indx;
//
//	if(Reset)
//	{
//		ACC = 0;
//		ACCAddIndx = 0;
//		HP_Ch_A_Indx = 0;
//		return 0;
//	}
//	if(ACCAddIndx < N_MA_HP_2H)
//	{
//		ACC += X_i;
//		MA_HP_Arr3[HP_A_Indx] = X_i;
//		HP_Ch_A_Indx++;
//		if(HP_Ch_A_Indx >= N_MA_HP_2H)
//			HP_Ch_A_Indx = 0;
//
//		ACCAddIndx++;
//		return X_i;
//	}
//	else
//	{
//		ACC = ACC + X_i - MA_HP_Arr3[HP_A_Indx];
//
//		int X_i_m64 = HP_A_Indx + (N_MA_HP_2H/2);
//		if(X_i_m64 >= N_MA_HP_2H)
//			X_i_m64 = X_i_m64 - N_MA_HP_2H;
//
////		Y_i = X_i - (ACC / N_MA_HP_2H);
//		Y_i = MA_HP_Arr3[X_i_m64] - (ACC / N_MA_HP_2H);
//		if(Mid != NULL)
//			*Mid = (ACC / N_MA_HP_2H);
//
//		MA_HP_Arr3[HP_A_Indx] = X_i;
//		HP_Ch_A_Indx++;
//		if(HP_Ch_A_Indx >= N_MA_HP_2H)
//			HP_Ch_A_Indx = 0;
//	}
//	return Y_i;
//}
