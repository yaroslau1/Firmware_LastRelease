/*
    Title:
    Author:   VN Asliuk(Cardian)
    Date:     15.10.2010 16:36:29
    Purpose:
    Software:
    Hardware:
    Note:
*/
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#ifndef TNANDWRITE_H
#define TNANDWRITE_H
//------------------------------------------------------------------------------------
#include "ae_ext_cm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "TAppProcessor.h"
//#include "TNand.h"
//------------------------------------------------------------------------------------
enum _addwframeerrors { adwfNoError,
                    		adwfWait,
		                    adwfOverload};
//------------------------------------------------------------------------------------
enum _addwSplitblockstatus {  splbNoSplit,
                    					splbSplitStart,
		                    			splbSplitContinue,
		                    			splbSplitEnd};
//------------------------------------------------------------------------------------
typedef struct
{
	uint32_t 	EcgChannelNum:4,
						IsECG:1,
						IsSND:1,
						IsADXL:1,
						IsPatientCard:1,
						IsLog:1,
						IsPacked:1,
						IsSystemInfo:1,
						IsRecordParams:1,
						IsVMLog:1,	
						IsPaceDetector:1, 
						IsDummy:1,
						EndBlock:1,	
						Take:16;	
} wfType;
//------------------------------------------------------------------------------------
#define NW_DATABUFF_SZ      (8192+8192)
#define NW_DATABUFF_SZ_HALF (NW_DATABUFF_SZ/2)
//------------------------------------------------------------------------------------
//#define NW_LED
//------------------------------------------------------------------------------------
//#define MAX_RECORD_SIZE 130000000L
//------------------------------------------------------------------------------------
//typedef struct
//{
//	wfType   WF_Type;
//	uint32_t DataBlockLenght; 
//	uint32_t DataBlockNum;
//} WFrameHeader;
//------------------------------------------------------------------------------------
class TNandWrite 
{
  public:
  	static void NW_Init();
  	
  	static void NW_WriteEpilog();
  	static void NW_WriteProlog();
		static void NW_WriteEpilogRestore();

  	static bool NW_BeginDataWFrame(wfType WFType,uint32_t spl);
  	static bool NW_UpdateDataSectorOfWFrame(const uint8_t *data, uint32_t size);
  	static bool NW_EndDataWFrame(bool WriteOut, uint32_t *splPtr);
  	
    static xTaskHandle xhDataWrite;
		static void TASK_DataWriteProcessor(void *pvParameters);
		static void TASK_TestDataWrite(void *pvParameters);
		static void TASK_TestDataWrite1(void *pvParameters);
		static void UpdateNStoreRecordSize(int sz);
    
    static bool NW_WriteBlockToFile(uint8_t *data,int size);
  	
    static int  ICI_IRQ_Cnt;
    
//	static NAND_ADDRESS WriteAddr;
  	static bool MemoryFull;

  	static int  RecordSize;
  	static bool GlobalEnableWriting;
    
    static bool UnexpectedReset;
    
    static bool FlashCardRemoved;
    
    static bool NandWrProcc;
    
  protected:
  private:
  	static uint32_t NW_AddDataWFrame(const uint8_t *data, uint32_t size);
  	static bool NW_WriteOutWFrame(); //write data is not completed before the end of the page
  	
  	static void NW_EraseNextNANDBlock();

  	static uint8_t NW_BlockCRC(const uint8_t *data, uint32_t size, uint8_t prevCRC); 

  	static uint8_t Data256Buffer[NW_DATABUFF_SZ];
  	static uint8_t EccBuffer[4];
  	static int DataBufferIndx;
  	static int txBufferIndx;
  	static int NW_PageIndx;
  	static int Block256SizeIndx;
  	static bool MakeECC;
  	static bool ForcedWritePage;
  	static bool AddDataBuffHalfTransferFlag;
  	static volatile bool BeginDataWFrame;
  	
  	static wfType   WF_Type;
  	static uint32_t DataBlockLenghtNBlockCRC; 
  	static uint32_t DataBlockNum;
  	static uint32_t DataBlockSample;
  	
		static uint16_t StartWriteAddrBlock;
		
		static bool KeepFirstBlockAsNoMemFull;
};
//------------------------------------------------------------------------------------
#endif
