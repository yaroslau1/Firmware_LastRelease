/*
    Title:
    Author:   VN Asliuk(Cardian)
    Date:     15.10.2010 16:38:20
    Purpose:
    Software:
    Hardware:
    Note:
*/
//------------------------------------------------------------------------------------
#include "TNandWrite.h"
#include "ff.h"
#include "TGuiObjectsV2.h"
#include "TSDProcessor.h"
#include "TDevice.h"
#include "TInterpreter.h"

//#include "TEcc.h"
//#include "TEepBackUp.h"
//------------------------------------------------------------------------------------
xTaskHandle TNandWrite::xhDataWrite;
FIL f1;
FIL f2;
extern bool StopModePeriod;
//------------------------------------------------------------------------------------
int   TNandWrite::DataBufferIndx = 0;
int   TNandWrite::NW_PageIndx = 0;
bool   TNandWrite::MakeECC = true;
bool  TNandWrite::ForcedWritePage = false;
bool  TNandWrite::AddDataBuffHalfTransferFlag = false;
int   TNandWrite::txBufferIndx = 0;
int   TNandWrite::ICI_IRQ_Cnt  = -50;
//NAND_ADDRESS TNandWrite::WriteAddr;
//------------------------------------------------------------------------------------
uint8_t TNandWrite::Data256Buffer[NW_DATABUFF_SZ];
//------------------------------------------------------------------------------------
volatile bool TNandWrite::BeginDataWFrame = false;
//------------------------------------------------------------------------------------
//WFrameHeader TNandWrite::wfStor;
wfType   TNandWrite::WF_Type;
uint32_t TNandWrite::DataBlockLenghtNBlockCRC = 0; 
uint32_t TNandWrite::DataBlockNum = 0;
uint32_t TNandWrite::DataBlockSample;
//------------------------------------------------------------------------------------
int  TNandWrite::RecordSize = 0;
//------------------------------------------------------------------------------------
bool TNandWrite::GlobalEnableWriting = true;
bool TNandWrite::MemoryFull = false;  
uint16_t TNandWrite::StartWriteAddrBlock = 0;
//------------------------------------------------------------------------------------
bool TNandWrite::KeepFirstBlockAsNoMemFull = false;
//------------------------------------------------------------------------------------
bool TNandWrite::UnexpectedReset = false;
//------------------------------------------------------------------------------------
volatile bool WaitWrProcess = false;
bool TNandWrite::FlashCardRemoved = false;
bool TNandWrite::NandWrProcc = false;

//------------------------------------------------------------------------------------


const unsigned char Crc8Table[256] = {
    0x00, 0x31, 0x62, 0x53, 0xC4, 0xF5, 0xA6, 0x97,
    0xB9, 0x88, 0xDB, 0xEA, 0x7D, 0x4C, 0x1F, 0x2E,
    0x43, 0x72, 0x21, 0x10, 0x87, 0xB6, 0xE5, 0xD4,
    0xFA, 0xCB, 0x98, 0xA9, 0x3E, 0x0F, 0x5C, 0x6D,
    0x86, 0xB7, 0xE4, 0xD5, 0x42, 0x73, 0x20, 0x11,
    0x3F, 0x0E, 0x5D, 0x6C, 0xFB, 0xCA, 0x99, 0xA8,
    0xC5, 0xF4, 0xA7, 0x96, 0x01, 0x30, 0x63, 0x52,
    0x7C, 0x4D, 0x1E, 0x2F, 0xB8, 0x89, 0xDA, 0xEB,
    0x3D, 0x0C, 0x5F, 0x6E, 0xF9, 0xC8, 0x9B, 0xAA,
    0x84, 0xB5, 0xE6, 0xD7, 0x40, 0x71, 0x22, 0x13,
    0x7E, 0x4F, 0x1C, 0x2D, 0xBA, 0x8B, 0xD8, 0xE9,
    0xC7, 0xF6, 0xA5, 0x94, 0x03, 0x32, 0x61, 0x50,
    0xBB, 0x8A, 0xD9, 0xE8, 0x7F, 0x4E, 0x1D, 0x2C,
    0x02, 0x33, 0x60, 0x51, 0xC6, 0xF7, 0xA4, 0x95,
    0xF8, 0xC9, 0x9A, 0xAB, 0x3C, 0x0D, 0x5E, 0x6F,
    0x41, 0x70, 0x23, 0x12, 0x85, 0xB4, 0xE7, 0xD6,
    0x7A, 0x4B, 0x18, 0x29, 0xBE, 0x8F, 0xDC, 0xED,
    0xC3, 0xF2, 0xA1, 0x90, 0x07, 0x36, 0x65, 0x54,
    0x39, 0x08, 0x5B, 0x6A, 0xFD, 0xCC, 0x9F, 0xAE,
    0x80, 0xB1, 0xE2, 0xD3, 0x44, 0x75, 0x26, 0x17,
    0xFC, 0xCD, 0x9E, 0xAF, 0x38, 0x09, 0x5A, 0x6B,
    0x45, 0x74, 0x27, 0x16, 0x81, 0xB0, 0xE3, 0xD2,
    0xBF, 0x8E, 0xDD, 0xEC, 0x7B, 0x4A, 0x19, 0x28,
    0x06, 0x37, 0x64, 0x55, 0xC2, 0xF3, 0xA0, 0x91,
    0x47, 0x76, 0x25, 0x14, 0x83, 0xB2, 0xE1, 0xD0,
    0xFE, 0xCF, 0x9C, 0xAD, 0x3A, 0x0B, 0x58, 0x69,
    0x04, 0x35, 0x66, 0x57, 0xC0, 0xF1, 0xA2, 0x93,
    0xBD, 0x8C, 0xDF, 0xEE, 0x79, 0x48, 0x1B, 0x2A,
    0xC1, 0xF0, 0xA3, 0x92, 0x05, 0x34, 0x67, 0x56,
    0x78, 0x49, 0x1A, 0x2B, 0xBC, 0x8D, 0xDE, 0xEF,
    0x82, 0xB3, 0xE0, 0xD1, 0x46, 0x77, 0x24, 0x15,
    0x3B, 0x0A, 0x59, 0x68, 0xFF, 0xCE, 0x9D, 0xAC
};
//------------------------------------------------------------------------------------

void TNandWrite::NW_Init()
{
  xTaskCreate(TASK_DataWriteProcessor,
              (signed char *) "DataWrite",
              configMINIMAL_STACK_SIZE,
              NULL, 
              tskIDLE_PRIORITY+1,
              &xhDataWrite);
}
//------------------------------------------------------------------------------------

uint32_t TNandWrite::NW_AddDataWFrame(const uint8_t *data, uint32_t size)
{
  static volatile bool Sync = true;
  
  if(!Sync) return adwfWait;
  if(WaitWrProcess) return adwfWait;
  Sync = false;  
  
  for(int i = 0;i < size;i++)
  {
    Data256Buffer[DataBufferIndx++] = data[i];
    if(DataBufferIndx >= (NW_DATABUFF_SZ)) DataBufferIndx = 0;
    if((DataBufferIndx%NW_DATABUFF_SZ_HALF) == 0)
    {
      if(DataBufferIndx == NW_DATABUFF_SZ_HALF) AddDataBuffHalfTransferFlag = true;
      else AddDataBuffHalfTransferFlag = false;
        
      WaitWrProcess = true;
      NandWrProcc = true;
      vTaskResume(xhDataWrite);
    }
  }
  
  Sync = true;
  
  return adwfNoError;
}
//------------------------------------------------------------------------------------

bool TNandWrite::NW_WriteOutWFrame() //write data is not completed before the end of the page
{
//  if(MakeECC) return false; // если данные поступают быстрее чем происходит обработка блока 256 то ошибка
  
  if(DataBufferIndx >= NW_DATABUFF_SZ_HALF)
  {
    for(int i = DataBufferIndx;i < NW_DATABUFF_SZ;i++)
    {
      Data256Buffer[i] = 0xFF;
    }
    DataBufferIndx = 0;
  }
  else
  {
    for(int i = DataBufferIndx;i < NW_DATABUFF_SZ_HALF;i++)
    {
      Data256Buffer[i] = 0xFF;
    }
    DataBufferIndx = NW_DATABUFF_SZ_HALF;
  }  
    
  AddDataBuffHalfTransferFlag = !AddDataBuffHalfTransferFlag;  
//  MakeECC = true;
//  ForcedWritePage = true;
  NandWrProcc = true;
  vTaskResume(xhDataWrite);
  
  return true;
}
//------------------------------------------------------------------------------------

bool TNandWrite::NW_WriteBlockToFile(uint8_t *data,int size)
{
  static bool Once = true;
  unsigned int BwW;
  
  FRESULT err;

  err = f_open( &f1, "KR01.RAW", FA_OPEN_EXISTING | FA_WRITE );
//    err = f_lseek(&f1 ,f1.fsize);
    
  if(Once)
  {  
    Once = false;
  }  
  else
  {
    for(int j = 0;j < sizeof(FIL);j++)
    {
      uint8_t *f1_ptr = (uint8_t *)&f1;
      uint8_t *f2_ptr = (uint8_t *)&f2;
      f1_ptr[j] = f2_ptr[j];
    }
  }
  err = f_write( &f1, data, size, &BwW );
  if(err == FR_OK) 
  {
    for(int j = 0;j < sizeof(FIL);j++)
    {
      uint8_t *f1_ptr = (uint8_t *)&f1;
      uint8_t *f2_ptr = (uint8_t *)&f2;
      f2_ptr[j] = f1_ptr[j];
    }
    
    FlashCardRemoved = false;
  }
  else 
  {
    //если запись остановлена - перегружаем f0 и f4
   
    TDevice::SystemReset();

//    FlashCardRemoved = true;
//    TSDProcessor::SDPresent = false;
//    TGuiObjects::ToolbarSet(TBFM_MEMCARD,0);
//    TAppProcessor::StartRecording = false;
//    TSDProcessor::SDStartWrPool = false;
//    TGuiObjects::GUIDesktopShowTime();
//    DataBlockLenghtNBlockCRC = 0; 
//    DataBlockNum = 0;
//    DataBufferIndx = 0;
//    NW_PageIndx = 0;
//    MakeECC = true;
//    ForcedWritePage = false;
//    AddDataBuffHalfTransferFlag = false;
//    txBufferIndx = 0;
//    WaitWrProcess = false;

    return false;
  }
  f_close( &f1 );
  if(size != BwW) 
    return false;
  else
    return true;
}
//------------------------------------------------------------------------------------

void TNandWrite::TASK_DataWriteProcessor(void *pvParameters)
{
  vTaskSuspend(NULL);
  
  uint8_t *buffPtr;
  
  while(1)
  {
//  if(MakeECC)
    
    {
      if(AddDataBuffHalfTransferFlag)
      {
        buffPtr = &Data256Buffer[0];
      }  
      else
      {
        buffPtr = &Data256Buffer[NW_DATABUFF_SZ_HALF];
      }  
      
      #ifdef NW_LED
//!!!    TDevice::LedOn();
      #endif
      
      NW_WriteBlockToFile(buffPtr,NW_DATABUFF_SZ_HALF);
      
      #ifdef NW_LED
         TDevice::LedOff();
      #endif
      
      WaitWrProcess = false;
    }
    
    //таск работает - сбрас. счетчик ICI_IRQ_Cnt 
    ICI_IRQ_Cnt = 0;
    
    NandWrProcc = false;
//  GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
    StopModePeriod = false;
    vTaskSuspend(NULL);
//  GPIO_SetBits( GPIOA, GPIO_Pin_8 );
  }
}
//------------------------------------------------------------------------------------

void TNandWrite::NW_WriteEpilog()
{
}
//------------------------------------------------------------------------------------

void TNandWrite::NW_WriteEpilogRestore()
{
}
//------------------------------------------------------------------------------------

void TNandWrite::NW_WriteProlog() // write wfStor to EEPROM 
{
//  GlobalEnableWriting = false;
}
//------------------------------------------------------------------------------------

bool TNandWrite::NW_BeginDataWFrame(wfType WFType,uint32_t spl)
{
  if(!GlobalEnableWriting) return true; //no waiting, just ignore 
  if(BeginDataWFrame) return false;
  BeginDataWFrame = true;
  
  WF_Type = WFType;
  DataBlockLenghtNBlockCRC = 0xFF000000;
  
  DataBlockSample = spl;
  
  return true;
}
//------------------------------------------------------------------------------------

uint8_t TNandWrite::NW_BlockCRC(const uint8_t *data, uint32_t size, uint8_t prevCRC)
{
  unsigned char crc = prevCRC;
 
  while(size--) crc = Crc8Table[crc ^ *data++];
 
  return crc;  
}
//------------------------------------------------------------------------------------

bool TNandWrite::NW_UpdateDataSectorOfWFrame(const uint8_t *data, uint32_t size)
{
  if(!GlobalEnableWriting) return true; //no waiting, just ignore 
  uint32_t err = NW_AddDataWFrame(data,size);
  if(err != adwfNoError) return false;
  uint32_t dbl = (DataBlockLenghtNBlockCRC & 0x00FFFFFF);
  dbl += size;
  DataBlockLenghtNBlockCRC = (DataBlockLenghtNBlockCRC & 0xFF000000) | dbl;
  //Calc CRC
  uint8_t crc = (uint8_t)((DataBlockLenghtNBlockCRC & 0xFF000000) >> 24);
  crc = NW_BlockCRC(data, size, crc);
  DataBlockLenghtNBlockCRC = (DataBlockLenghtNBlockCRC & 0x00FFFFFF) | ((uint32_t)crc << 24);
  
  DataBlockSample += size;
  
  return true;
}
//------------------------------------------------------------------------------------

bool TNandWrite::NW_EndDataWFrame(bool WriteOut, uint32_t *splPtr)
{
  if(!GlobalEnableWriting) return true; //no waiting, just ignore 

  uint8_t hdrBytes[2+4*4];

  uint32_t _type = *((uint32_t*)&WF_Type);

  hdrBytes[0] = (uint8_t)(_type & 0x000000FF);
  hdrBytes[1] = (uint8_t)((_type & 0x0000FF00) >> 8);
  hdrBytes[2] = (uint8_t)((_type & 0x00FF0000) >> 16);
  hdrBytes[3] = (uint8_t)((_type & 0xFF000000) >> 24);

  hdrBytes[4] = (uint8_t)(DataBlockLenghtNBlockCRC & 0x000000FF);
  hdrBytes[5] = (uint8_t)((DataBlockLenghtNBlockCRC & 0x0000FF00) >> 8);
  hdrBytes[6] = (uint8_t)((DataBlockLenghtNBlockCRC & 0x00FF0000) >> 16);
  hdrBytes[7] = (uint8_t)((DataBlockLenghtNBlockCRC & 0xFF000000) >> 24);

  hdrBytes[8] = (uint8_t)(DataBlockNum & 0x000000FF);
  hdrBytes[9] = (uint8_t)((DataBlockNum & 0x0000FF00) >> 8);
  hdrBytes[10] = (uint8_t)((DataBlockNum & 0x00FF0000) >> 16);
  hdrBytes[11] = (uint8_t)((DataBlockNum & 0xFF000000) >> 24);

  uint32_t _dtSpl = DataBlockSample - (DataBlockLenghtNBlockCRC & 0x00FFFFFF);

  hdrBytes[12] = (uint8_t)(_dtSpl & 0x000000FF);
  hdrBytes[13] = (uint8_t)((_dtSpl & 0x0000FF00) >> 8);
  hdrBytes[14] = (uint8_t)((_dtSpl & 0x00FF0000) >> 16);
  hdrBytes[15] = (uint8_t)((_dtSpl & 0xFF000000) >> 24);

  hdrBytes[4*4] = 0x55;
  hdrBytes[4*4+1] = 0xAA;
  
  *splPtr = DataBlockSample;// - (DataBlockLenghtNBlockCRC & 0x00FFFFFF);
  
  uint32_t err = NW_AddDataWFrame(hdrBytes,2+4*4);
  if(err != adwfNoError) return false;
  
  DataBlockNum++;
  
//  BKP_WriteBackupRegister(BKP_DR3,(uint16_t)(DataBlockNum & 0x0000FFFF));
//  BKP_WriteBackupRegister(BKP_DR4,(uint16_t)((DataBlockNum & 0xFFFF0000)>>16));

  if(WriteOut) NW_WriteOutWFrame();
  
  BeginDataWFrame = false;  
  return true;
}
//------------------------------------------------------------------------------------

void TNandWrite::UpdateNStoreRecordSize(int sz)
{
  RecordSize += sz;//NAND_PAGE_WITH_SPARE;
  // BKP_WriteBackupRegister(BKP_DR8,(uint16_t)(RecordSize & 0x0000FFFF));
  // BKP_WriteBackupRegister(BKP_DR9,(uint16_t)((RecordSize & 0xFFFF0000)>>16));
  
  // #ifdef RESTORE_BKP
    // TEepBackUp::EBK_CalcNStoreCRC16();
  // #endif    
}
//------------------------------------------------------------------------------------
