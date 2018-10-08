/*
    Title:
    Author:   VN Asliuk(Cardian)
    Date:     05.09.2013 12:05:05
    Purpose:
    Software:
    Hardware:
    Note:
*/
//------------------------------------------------------------------------------------
#include "TPoolingNand.h"
#include "TNandWrite.h"
#include "TDevice.h"
#include "TGuiObjectsV2.h"

//#include "TSound.h"

//------------------------------------------------------------------------------------
xTaskHandle TPoolingNand::xhPoolingNand;
//------------------------------------------------------------------------------------
bool TPoolingNand::Pool1WRConfirm = false;
bool TPoolingNand::Pool1REQ = false;
bool TPoolingNand::Pool2REQ = false;
bool TPoolingNand::Pool3REQ = false;
bool TPoolingNand::Pool4REQ = false;
BYTE TPoolingNand::Pool1[POOL1SIZE];
BYTE TPoolingNand::Pool2[POOL2SIZE];
BYTE TPoolingNand::Pool3[POOL3SIZE];
BYTE TPoolingNand::Pool4[POOL4SIZE];
stPoolParams TPoolingNand::Pool1Params;
stPoolParams TPoolingNand::Pool2Params;
stPoolParams TPoolingNand::Pool3Params;
stPoolParams TPoolingNand::Pool4Params;
int TPoolingNand::CriticalError = perrNoErr;
int TPoolingNand::InDataActivity = -1;  
bool TPoolingNand::wrTaskSuspended = true;
//bool TPoolingNand::wrTaskReadParams = true;
//------------------------------------------------------------------------------------
bool TPoolingNand::WriteOut = false;
//------------------------------------------------------------------------------------
BYTE TPoolingNand::PoolState = 0;
bool TaskRun = false;
extern bool StopModePeriod;
//------------------------------------------------------------------------------------

void TPoolingNand::PO_Init()
{
  xTaskCreate(TASK_PoolingNand,
              (signed char *) "PoolingNand",
              configMINIMAL_STACK_SIZE,
              NULL, 
              tskIDLE_PRIORITY+2,
              &xhPoolingNand);
  CriticalError = perrNoErr;            
}
//------------------------------------------------------------------------------------

void TPoolingNand::TASK_PoolingNand(void *pvParameters)
{
  volatile int FixIn;
  volatile int FixSize;
  wfType Type;
  
  while(1)
  {
    if(PoolState == 0)
    {
      wrTaskSuspended = true;
//      GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
       vTaskSuspend(NULL);
//      GPIO_SetBits( GPIOA, GPIO_Pin_8 );
      wrTaskSuspended = false;
    }
//    vTaskDelay(20/portTICK_RATE_MS); 
    
    if(CriticalError != perrNoErr)
    {
#ifdef WRITE_LED
//!!!      TDevice::LedOn();
#endif
      while(1) vTaskDelay(2/portTICK_RATE_MS); 
    }
    
    if(Pool1REQ)
    {
//      GPIO_SetBits( GPIOA, GPIO_Pin_8 );
      if(Pool1Params.QueueSize == 0) 
      {
        Pool1REQ = false; //106
//        GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
        continue;
      }
        
#ifdef WRITE_LED
//!!!      TDevice::LedOn();
#endif

      taskENTER_CRITICAL();
      FixIn = Pool1Params.InPtr;
      FixSize = Pool1Params.QueueSize;
      Pool1Params.QueueSize = 0;
      taskEXIT_CRITICAL();
      
      Type = *((wfType*)&Pool1Params.BlockWFType);
      
      while(!TNandWrite::NW_BeginDataWFrame(Type,Pool1Params.BlockTakeSample)) vTaskDelay(2/portTICK_RATE_MS); 
        
      int _fin = FixIn;
      int isz_diff = (_fin-FixSize);
      
      if(isz_diff < 0)
      {
        int OutPtr = POOL1SIZE + isz_diff;
        int cSz = POOL1SIZE - OutPtr;
        while(!TNandWrite::NW_UpdateDataSectorOfWFrame(&Pool1[OutPtr],cSz)) vTaskDelay(2/portTICK_RATE_MS); 
        while(!TNandWrite::NW_UpdateDataSectorOfWFrame(&Pool1[0],FixIn)) vTaskDelay(2/portTICK_RATE_MS); 
      }  
      else  
      {
        while(!TNandWrite::NW_UpdateDataSectorOfWFrame(&Pool1[isz_diff],FixSize)) vTaskDelay(2/portTICK_RATE_MS); 
      }
      
      while(!TNandWrite::NW_EndDataWFrame(false,&Pool1Params.BlockTakeSample)) vTaskDelay(2/portTICK_RATE_MS); 
        
      Pool1REQ = false;
/*      
      if (!start_confirmed)
      {
        TSound::PlaySound(_modiShortBeep);
        start_confirmed = true;
      }
*/      
      Pool1WRConfirm = true;
      
      
//      vTaskDelay(20/portTICK_RATE_MS); 
#ifdef WRITE_LED
      TDevice::LedOff();
#endif
      TaskRun = false;
      if(TNandWrite::NandWrProcc == false) 
      {
//        GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
        StopModePeriod = false;
      }
      continue;  
    }
    StopModePeriod = false;
//    GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
    if(Pool2REQ)
    {
      if(Pool2Params.QueueSize == 0) 
      {
        Pool2REQ = false;
        continue;
      }
#ifdef WRITE_LED
//!!!      TDevice::LedOn();
#endif
      taskENTER_CRITICAL();
      FixIn = Pool2Params.InPtr;
      FixSize = Pool2Params.QueueSize;
      Pool2Params.QueueSize = 0;
      taskEXIT_CRITICAL();
      
      Type = *((wfType*)&Pool2Params.BlockWFType);
      
      while(!TNandWrite::NW_BeginDataWFrame(Type,Pool2Params.BlockTakeSample)) vTaskDelay(2/portTICK_RATE_MS); 
        
      int _fin = FixIn;
      int isz_diff = (_fin-FixSize);
      if(isz_diff < 0)
      {
        int OutPtr = POOL2SIZE + isz_diff;
        int cSz = POOL2SIZE - OutPtr;
        while(!TNandWrite::NW_UpdateDataSectorOfWFrame(&Pool2[OutPtr],cSz)) vTaskDelay(2/portTICK_RATE_MS); 
        while(!TNandWrite::NW_UpdateDataSectorOfWFrame(&Pool2[0],FixIn)) vTaskDelay(2/portTICK_RATE_MS); 
      }  
      else  
      {
        while(!TNandWrite::NW_UpdateDataSectorOfWFrame(&Pool2[isz_diff],FixSize)) vTaskDelay(2/portTICK_RATE_MS); 
      }
      
      while(!TNandWrite::NW_EndDataWFrame(false,&Pool2Params.BlockTakeSample)) vTaskDelay(2/portTICK_RATE_MS); 
        
      Pool2REQ = false;
//      vTaskDelay(20/portTICK_RATE_MS); 
      #ifdef WRITE_LED
        TDevice::LedOff();
      #endif
      continue;  
    }
//    if(Pool3REQ)
//    {
//      if(Pool3Params.QueueSize == 0) continue;
//      #ifdef WRITE_LED
//        TDevice::LedOn();
//      #endif
//      taskENTER_CRITICAL();
//      FixIn = Pool3Params.InPtr;
//      FixSize = Pool3Params.QueueSize;
//      Pool3Params.QueueSize = 0;
//      taskEXIT_CRITICAL();
//      
//      Type = *((wfType*)&Pool3Params.BlockWFType);
//      
//      while(!TNandWrite::NW_BeginDataWFrame(Type,Pool3Params.BlockTakeSample)) vTaskDelay(2/portTICK_RATE_MS); 
//        
//      int _fin = FixIn;
//      int isz_diff = (_fin-FixSize);
//      if(isz_diff < 0)
//      {
//        int OutPtr = POOL3SIZE + isz_diff;
//        int cSz = POOL3SIZE - OutPtr;
//        while(!TNandWrite::NW_UpdateDataSectorOfWFrame(&Pool3[OutPtr],cSz)) vTaskDelay(2/portTICK_RATE_MS); 
//        while(!TNandWrite::NW_UpdateDataSectorOfWFrame(&Pool3[0],FixIn)) vTaskDelay(2/portTICK_RATE_MS); 
//      }  
//      else  
//      {
//        while(!TNandWrite::NW_UpdateDataSectorOfWFrame(&Pool3[isz_diff],FixSize)) vTaskDelay(2/portTICK_RATE_MS); 
//      }
//      
//      while(!TNandWrite::NW_EndDataWFrame(false,&Pool3Params.BlockTakeSample)) vTaskDelay(2/portTICK_RATE_MS); 
//        
//      Pool3REQ = false;
//      vTaskDelay(2/portTICK_RATE_MS); 
//      #ifdef WRITE_LED
//        TDevice::LedOff();
//      #endif
//      continue;  
//    }
//    if(Pool4REQ)
//    {
//      if(Pool4Params.QueueSize == 0) continue;
//      #ifdef WRITE_LED
//        TDevice::LedOn();
//      #endif
//      taskENTER_CRITICAL();
//      FixIn = Pool4Params.InPtr;
//      FixSize = Pool4Params.QueueSize;
//      Pool4Params.QueueSize = 0;
//      taskEXIT_CRITICAL();
//      
//      Type = *((wfType*)&Pool4Params.BlockWFType);
//      
//      while(!TNandWrite::NW_BeginDataWFrame(Type,Pool4Params.BlockTakeSample)) vTaskDelay(2/portTICK_RATE_MS); 
//        
//      int _fin = FixIn;
//      int isz_diff = (_fin-FixSize);
//      if(isz_diff < 0)
//      {
//        int OutPtr = POOL4SIZE + isz_diff;
//        int cSz = POOL4SIZE - OutPtr;
//        while(!TNandWrite::NW_UpdateDataSectorOfWFrame(&Pool4[OutPtr],cSz)) vTaskDelay(2/portTICK_RATE_MS); 
//        while(!TNandWrite::NW_UpdateDataSectorOfWFrame(&Pool4[0],FixIn)) vTaskDelay(2/portTICK_RATE_MS); 
//      }  
//      else  
//      {
//        while(!TNandWrite::NW_UpdateDataSectorOfWFrame(&Pool4[isz_diff],FixSize)) vTaskDelay(2/portTICK_RATE_MS); 
//      }
//      
//      while(!TNandWrite::NW_EndDataWFrame(false,&Pool4Params.BlockTakeSample)) vTaskDelay(2/portTICK_RATE_MS); 
//        
//      Pool4REQ = false;
//      vTaskDelay(2/portTICK_RATE_MS); 
//      #ifdef WRITE_LED
//        TDevice::LedOff();
//      #endif
//      continue;  
//    }
    
    vTaskDelay(2/portTICK_RATE_MS);     
  }
}
//------------------------------------------------------------------------------------
    
void TPoolingNand::AddDataToPool(int pool,BYTE *data,int Size,bool FromIsr)
{
  InDataActivity = DATAACTIVITYTOUT;
  switch(pool)
  {
    case 1:
      {
        if((PoolState & 0x01) == 0) break;
        for(int i = 0;i < Size;i++)
        {
          Pool1[Pool1Params.InPtr++] = data[i];
          if(Pool1Params.InPtr >= POOL1SIZE) Pool1Params.InPtr = 0;
          Pool1Params.QueueSize++;
          if(Pool1Params.QueueSize > POOL1SIZE) 
          {
            CriticalError = perrPool1Ovr;
            TGuiObjects::ToolbarSet(TBFM_ERROR,1);
            vTaskSuspend(xhPoolingNand);
            return;
          }
          const int percsz0 = ((POOL1SIZE * POOL1THRESHOLD) / 100);  
          if(Pool1Params.QueueSize > percsz0) 
          {
            Pool1REQ = true;
            if(wrTaskSuspended)
            {  
              TaskRun = true;
              if(FromIsr)
                xTaskResumeFromISR(xhPoolingNand);
              else  
                vTaskResume(xhPoolingNand);
            }
          }
//          else GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
        }
//        if(TaskRun == false) GPIO_ResetBits( GPIOA, GPIO_Pin_8 );
      }
      break;
    case 2:
      {
        if((PoolState & 0x02) == 0) break;
        if((Pool2Params.QueueSize+Size) > POOL2SIZE)
        {
          TGuiObjects::ToolbarSet(TBFM_ERROR,1);
          break;
        }
        for(int i = 0;i < Size;i++)
        {
          Pool2[Pool2Params.InPtr] = data[i];
          Pool2Params.QueueSize++;
//          if(Pool2Params.QueueSize > POOL2SIZE) 
//            {
//              Pool2Params.QueueSize--;
////              CriticalError = perrPool2Ovr;
//              TGuiObjects::ToolbarSet(TBFM_ERROR,1);
////              vTaskSuspend(xhPoolingNand);
//              return;
//            }
          Pool2Params.InPtr++;
          if(Pool2Params.InPtr >= POOL2SIZE) Pool2Params.InPtr = 0;
          
          const int percsz1 = ((POOL2SIZE * POOL2THRESHOLD) / 100);  
          if(Pool2Params.QueueSize > percsz1) 
            {
              Pool2REQ = true;
              if(wrTaskSuspended)
              {  
                if(FromIsr)
                  xTaskResumeFromISR(xhPoolingNand);
                else  
                  vTaskResume(xhPoolingNand);
              }
            }
        }
      }
      break;
//    case 3:
//      {
//        if((PoolState & 0x04) == 0) break;
//        for(int i = 0;i < Size;i++)
//        {
//          Pool3[Pool3Params.InPtr++] = data[i];
//          if(Pool3Params.InPtr >= POOL3SIZE) Pool3Params.InPtr = 0;
//          Pool3Params.QueueSize++;
//          if(Pool3Params.QueueSize > POOL3SIZE) 
//            {
//              CriticalError = perrPool3Ovr;
//              TGuiObjects::ToolbarSet(TBFM_ERROR,1);
//              vTaskSuspend(xhPoolingNand);
//              return;
//            }
//          const int percsz = ((POOL3SIZE * POOL3THRESHOLD) / 100);  
//          if(Pool3Params.QueueSize > percsz) 
//            {
//              Pool3REQ = true;
//              if(wrTaskSuspended)
//              {  
//                if(FromIsr)
//                  xTaskResumeFromISR(xhPoolingNand);
//                else  
//                  vTaskResume(xhPoolingNand);
//              }
//            }
//        }
//      }
//      break;
//    case 4:
//      {
//        if((PoolState & 0x08) == 0) break;
//        for(int i = 0;i < Size;i++)
//        {
//          Pool4[Pool4Params.InPtr++] = data[i];
//          if(Pool4Params.InPtr >= POOL4SIZE) Pool4Params.InPtr = 0;
//          Pool4Params.QueueSize++;
//          if(Pool4Params.QueueSize > POOL4SIZE) 
//            {
//              CriticalError = perrPool4Ovr;
//              TGuiObjects::ToolbarSet(TBFM_ERROR,1);
//              vTaskSuspend(xhPoolingNand);
//              return;
//            }
//          const int percsz = ((POOL4SIZE * POOL4THRESHOLD) / 100);  
//          if(Pool4Params.QueueSize > percsz) 
//            {
//              Pool4REQ = true;
//              if(wrTaskSuspended)
//              {  
//                if(FromIsr)
//                  xTaskResumeFromISR(xhPoolingNand);
//                else  
//                  vTaskResume(xhPoolingNand);
//              }
//            }
//        }
//      }
//      break;
    default:
      return;  
  }
}
//------------------------------------------------------------------------------------

void TPoolingNand::FinishPool(int pool)
{
//  wfType *wf;
  
  switch(pool)
  {
    case 1: 
      Pool1REQ = true;
      PoolState &= ~0x01;
      break;
    case 2: 
      Pool2REQ = true;
      PoolState &= ~0x02;
      break;
    case 3: 
      Pool3REQ = true;
      PoolState &= ~0x04;
      break;
    case 4: 
      Pool4REQ = true;
      PoolState &= ~0x08;
      break;
    default:
      return;  
  }
  vTaskResume(xhPoolingNand);
}
//------------------------------------------------------------------------------------

void TPoolingNand::SetupPool(int pool,wfType wftype)
{
  uint32_t wft = *((uint32_t*) &wftype);
  
  switch(pool)
  {
    case 1:
      PoolState |= 0x01;
      Pool1REQ = false;
      Pool1Params.InPtr = 0;
      Pool1Params.QueueSize = 0;
      Pool1Params.BlockWFType = wft;
      Pool1Params.BlockTakeSample = 0;
      break;
    case 2:
      PoolState |= 0x02;
      Pool2REQ = false;
      Pool2Params.InPtr = 0;
      Pool2Params.QueueSize = 0;
      Pool2Params.BlockWFType = wft;
      Pool2Params.BlockTakeSample = 0;
      break;
    case 3:
      PoolState |= 0x04;
      Pool3REQ = false;
      Pool3Params.InPtr = 0;
      Pool3Params.QueueSize = 0;
      Pool3Params.BlockWFType = wft;
      Pool3Params.BlockTakeSample = 0;
      break;
    case 4:
      PoolState |= 0x08;
      Pool4REQ = false;
      Pool4Params.InPtr = 0;
      Pool4Params.QueueSize = 0;
      Pool4Params.BlockWFType = wft;
      Pool4Params.BlockTakeSample = 0;
      break;
    default:
      return;  
  }
}
//------------------------------------------------------------------------------------
