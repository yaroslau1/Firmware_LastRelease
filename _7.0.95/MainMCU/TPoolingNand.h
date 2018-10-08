/*
    Title:
    Author:   VN Asliuk(Cardian)
    Date:     04.05.2011 13:52:56
    Purpose:
    Software:
    Hardware:
    Note:
*/
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#ifndef TPOOLINGNAND_H
#define TPOOLINGNAND_H
//------------------------------------------------------------------------------------
#include "ae_ext_cm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "TNandWrite.h"
//------------------------------------------------------------------------------------
typedef struct
{
	int InPtr;
	int QueueSize;
	uint32_t BlockWFType;
	uint32_t BlockTakeSample;
} stPoolParams;
//------------------------------------------------------------------------------------
#define POOL1SIZE 6144*2 //(4096+512) //(512+512)
#define POOL2SIZE (4096+2048/*+512*/)
#define POOL3SIZE 16
#define POOL4SIZE 16
#define POOL1THRESHOLD 50 // percent
#define POOL2THRESHOLD 50 // percent
#define POOL3THRESHOLD 85 // percent
#define POOL4THRESHOLD 75 // percent
//------------------------------------------------------------------------------------
#define DATAACTIVITYTOUT  100 //*10ms
//------------------------------------------------------------------------------------
enum _enErrPool {perrNoErr,perrPool1Ovr,perrPool2Ovr,perrPool3Ovr,perrPool4Ovr};
//------------------------------------------------------------------------------------
#define SBKP_PERIOD 5 //мин
#define SBKP_CNTOVR ((60*1000/((((POOL1SIZE*POOL1THRESHOLD/100)*8)/(12*3))*4))*SBKP_PERIOD)
//------------------------------------------------------------------------------------
//#define WRITE_LED
//------------------------------------------------------------------------------------
class TPoolingNand : public TNandWrite
{
  public:
  	static void PO_Init();
  	
  	static void AddDataToPool(int pool,BYTE *data,int Size,bool FromIsr);
  	static void FinishPool(int pool);
  	static void SetupPool(int pool,wfType wftype);
    
    static bool Pool1WRConfirm;
    
//  	static void UpdatePoolType(int pool,int wftype);
  protected:
    static xTaskHandle xhPoolingNand;
  	static void TASK_PoolingNand(void *pvParameters);
  private:
  	static int CriticalError;
  	
  	static bool Pool1REQ;
  	static bool Pool2REQ;
  	static bool Pool3REQ;
  	static bool Pool4REQ;
  	
  	static bool WriteOut;
  	
  	static BYTE Pool1[POOL1SIZE];
  	static BYTE Pool2[POOL2SIZE];
  	static BYTE Pool3[POOL3SIZE];
  	static BYTE Pool4[POOL4SIZE];
  	
  	static stPoolParams Pool1Params;
  	static stPoolParams Pool2Params;
  	static stPoolParams Pool3Params;
  	static stPoolParams Pool4Params;
  	
  	static int InDataActivity;
  	
  	static int StoreBKPCnt;
  	
  	static BYTE PoolState;
    
    static bool wrTaskSuspended;
};
//------------------------------------------------------------------------------------
#endif
