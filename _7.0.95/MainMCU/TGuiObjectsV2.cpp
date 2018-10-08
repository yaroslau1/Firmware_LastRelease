/*
    Title:    
    Author:   VN Asliuk(Cardian)
    Date:     09.11.2010 13:45:07
    Purpose:  
    Software: 
    Hardware: 
    Note:    
*/
//------------------------------------------------------------------------------------
#include "TGuiObjectsV2.h"
#include "TLcdTrace.h"
#include "TDevice.h"
#include "TAppProcessor.h"
#include "TSDProcessor.h"
#include "TFlash.h"
#include "TADS1292.h"
#include "TADS1298.h"
#include <stdlib.h>
#include "THardware.h"
#include "TRtc.h"
#include "TAudio.h"
#include "TFont.h"
#include "TInterpreter.h"
#include "TLMX9838.h"
#include "ConstBitmap.h"


extern TBitMap const* SysFont;


//------------------------------------------------------------------------------------
//#define TEST_ON_TOP
//------------------------------------------------------------------------------------
volatile int  TGuiObjects::GUIMode = guioNone;
xTaskHandle TGuiObjects::xGUIObj;
xTaskHandle TGuiObjects::xGUIScrOff;
float TGuiObjects::t_k = 0.0;
//float TGuiObjects::b_k = 0.0;  
//------------------------------------------------------------------------------------
int  TGuiObjects::ToolbarsFlags = 0;
bool  TGuiObjects::ToolbarsFlagsShow = true;
//------------------------------------------------------------------------------------
int TGuiObjects::CountDown = 0;
//------------------------------------------------------------------------------------
int TGuiObjects::HiddenWindow = -1;
//------------------------------------------------------------------------------------
char *TGuiObjects::messagePtr = NULL;
//------------------------------------------------------------------------------------
int TGuiObjects::MenuSel = 0;
int TGuiObjects::MenuUnderWindow = 0;
//------------------------------------------------------------------------------------
const char mitDef0[] = {"Режим \"Кнопка\""};
const char mitDef1[] = {"Режим \"Манометр\""};
const char mitDef2[] = {"Отмена"};
const char mitDef3[] = {"Отмена"};
//------------------------------------------------------------------------------------
const char *TGuiObjects::menuItem0Ptr;
const char *TGuiObjects::menuItem1Ptr;
const char *TGuiObjects::menuItem2Ptr;
const char *TGuiObjects::menuItem3Ptr;
int TGuiObjects::menuMax = 3;
//------------------------------------------------------------------------------------
int TGuiObjects::remainTime = 0;
//------------------------------------------------------------------------------------
bool TGuiObjects::decDigits = true;
//------------------------------------------------------------------------------------
int TGuiObjects::TestWInt0 = -1;
int TGuiObjects::TestWInt1 = -2;
int TGuiObjects::TestWInt2 = -3;
int TGuiObjects::TestWInt3 = -4;
//------------------------------------------------------------------------------------
int TGuiObjects::UploadPercent = 100; //1071
//------------------------------------------------------------------------------------
bool TGuiObjects::MeasureNewStyle = false;
int  TGuiObjects::MNSFixTime = 0;
bool TGuiObjects::MeasureHeartBeatEvent = 0;
//------------------------------------------------------------------------------------
bool TGuiObjects::HoldDig = false;
bool TGuiObjects::HoldOneUpdate = false;  
//------------------------------------------------------------------------------------
bool TGuiObjects::ECG_Enstead_Time = false;
//int  TGuiObjects::SampleToShow = 0;  
int  TGuiObjects::SelectECGChannelToShow = 0;  
volatile bool TGuiObjects::EcgOnScreen = false;
int MoveAfterMsg = 0;

bool TGuiObjects::StandbyLedEnable = true;

bool TGuiObjects::ButtonModeStart = false;

//bool TGuiObjects::ScreenOrientation = true;
bool TGuiObjects::ScreenOrientation = false;

bool TGuiObjects::WaitingECGStartRecord = false;

int TGuiObjects::ToolbarOffset = 6;

extern bool EnterStopMode;

int TGuiObjects::Timeout = LCDBLOFF_TIMER;


//------------------------------------------------------------------------------------

void TGuiObjects::GUIObj_Init()
{
#ifdef DISABLE_GUI
  return;
#endif
  
  xTaskCreate(GO_TASK_GUIObj,
              (signed char *) "GUI",
              configMINIMAL_STACK_SIZE+128,
              NULL, 
              tskIDLE_PRIORITY+1,
              &xGUIObj);
  xTaskCreate(GO_TASK_ScrOff,
              (signed char *) "ScreenOff",
              configMINIMAL_STACK_SIZE,
              NULL, 
              tskIDLE_PRIORITY,
              &xGUIScrOff);
  GUIUpdateTempNBatt();        
  menuSetDefaults();    
}
//------------------------------------------------------------------------------------

void TGuiObjects::GUIUpdateTempNBatt()
{
}
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
/*
int TGuiObjects::MenuBox(const char *message, const char *menutext, uint32_t timeout)
{
#ifdef DISABLE_GUI
  return -1;
#endif

  if(message == NULL)  return -1;
  if(itemnum > 5)      return -1;
  if(menutext == NULL) return -1;

  char errstr2[40];
//  char errstr[5];

  SetSysFont(&_Arial_18[0]);
  int strlmess = strlen(message);
  int strlmenu = strlen(menutext);

  struct
  {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
  } mb, msg, menu[5];

  //mb
  mb.x         = 0;
  mb.y         = 51;
  mb.width     = 320;
  mb.height    = 138;

  //msg
  msg.x        = mb.x;
  msg.y        = mb.y;
  msg.width    = mb.width;
  msg.height   = mb.height;

  if(showmenu)
  {
    //menu2
    menu2.height = SysFont[0].biHeight;
    menu2.y      = mb.y + mb.height - menu2.height;
    menu2.width  = MIN(2 * StringLen(menutext2), mb.width);
    menu2.width  = MAX(menu2.width, 160);

    //menu1
    menu1.height = SysFont[0].biHeight;
    menu1.y      = menu2.y - menu1.height;
    menu1.width  = MIN(2 * StringLen(menutext1), mb.width);
    menu1.width  = MAX(menu1.width, 160);

    //menu1, menu2
    menu1.width  = MAX(menu1.width, menu2.width);
    menu1.x      = (mb.x + mb.width) / 2 - (menu1.width / 2);
    menu2.width  = menu1.width;
    menu2.x      = menu1.x;

    //msg
    msg.height   = msg.height - menu1.height - menu2.height;
  }

  FillRectangle(mb.x, mb.y, mb.width, mb.height, SysPal[0]);

  WORD stry = msg.y + (msg.height - MIN(StringCnt(message) * SysFont[0].biHeight, msg.height)) / 2;
  WORD strindx = 0;
  bool draw = false;
  
  
  for(int i = 0;i < strl;i++)
  {
    errstr2[strindx] = message[i];
    if(message[i] == 0x0A) 
    {
      errstr2[strindx] = 0;
      strindx = 0;
      draw = true;
    }
    else
    {
      strindx++;
    }
    if(i == (strl-1))
    {
      errstr2[strindx] = 0;
      draw = true;
    }
    if(draw) 
    {
      draw = false;
      DrawText(msg.x, stry, errstr2, msg.width, algCenter);
      stry += SysFont[0].biHeight;
      if (stry >= msg.y + msg.height - SysFont[0].biHeight) break;
    }
  }

  if(timeout == 0) return 0;
  if(timeout < (0xFFFFFFFE / 100)) timeout = timeout * 100;
  else timeout = 0xFFFFFFFE;

  uint8_t menustate = 1, prevmenustate = 2;
  uint8_t chgbtnstate = 2, prevchgbtnstate = 2;
  uint8_t selbtnstate = 2, prevselbtnstate = 2;
  
  int i = timeout;
  while( i > 0)
  {
    i--;
    if(showmenu)
    {
      if(menustate != prevmenustate)
      {
        prevmenustate = menustate;
        if(menustate == 1)
        {
          ExchSysPal();
          DrawText(menu1.x, menu1.y, menutext1, menu1.width, algCenter);
          ExchSysPal();
          DrawText(menu2.x, menu2.y, menutext2, menu2.width, algCenter);
        }
        if(menustate == 2)
        {
          DrawText(menu1.x, menu1.y, menutext1, menu1.width, algCenter);
          ExchSysPal();
          DrawText(menu2.x, menu2.y, menutext2, menu2.width, algCenter);
          ExchSysPal();
        }
      }

      prevchgbtnstate = chgbtnstate;
      if(THardware::CheckKey(ChangeButton) == 0)
      {
        chgbtnstate = 0;
      }
      else if (chgbtnstate == 0) 
      {
        chgbtnstate = 1;
      }
      if (prevchgbtnstate != chgbtnstate)
      {
        i = timeout;
      }
      if ((prevchgbtnstate == 1) && (chgbtnstate == 0))
      {
        if (menustate == 1)
        {
          menustate = 2;
        }
        else
        {
          menustate = 1;
        }
      }

      prevselbtnstate = selbtnstate;
      if(THardware::CheckKey(SelectButton) == 0)
      {
        selbtnstate = 0;
      }
      else if (selbtnstate == 0) 
      {
        selbtnstate = 1;
      }
      if ((prevselbtnstate == 1) && (selbtnstate == 0))
      {
        return menustate;
      }
    }
    vTaskDelay(10/portTICK_RATE_MS);    
  }
  return 0;
}
*/
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
int TGuiObjects::MessageBox(const char *message, bool showmenu, const char *menutext1, const char *menutext2, uint32_t timeout)
{
#ifdef DISABLE_GUI
  return -1;
#endif

  if(message == NULL) return -1;

  char errstr2[40];
//  char errstr[5];

//  SetSysFont(&_Arial_18[0]);
  int strl = strlen(message);

  struct
  {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
  } mb, msg, menu1, menu2;

  //mb
  mb.x         = 0;
  mb.width     = 320;
  if (GUIMode <= guioStartUpProgressBar)
  {
    mb.y         = 51-32;
    mb.height    = 202;
  }
  else
  {
    mb.y         = 51;
    mb.height    = 138;
  }

  //msg
  msg.x        = mb.x;
  msg.y        = mb.y;
  msg.width    = mb.width;
  msg.height   = mb.height;

  if(showmenu)
  {
    //menu2
    menu2.height = SysFont[0].biHeight;
    menu2.y      = mb.y + mb.height - menu2.height;
    menu2.width  = MIN(2 * StringLen(menutext2), mb.width);
    menu2.width  = MAX(menu2.width, 160);

    //menu1
    menu1.height = SysFont[0].biHeight;
    menu1.y      = menu2.y - menu1.height;
    menu1.width  = MIN(2 * StringLen(menutext1), mb.width);
    menu1.width  = MAX(menu1.width, 160);

    //menu1, menu2
    menu1.width  = MAX(menu1.width, menu2.width);
    menu1.x      = (mb.x + mb.width) / 2 - (menu1.width / 2);
    menu2.width  = menu1.width;
    menu2.x      = menu1.x;

    //msg
    msg.height   = msg.height - menu1.height - menu2.height;
  }

  FillRectangle(mb.x, mb.y, mb.width, mb.height, SysPal[0]);
  LcdBL(true);

  WORD stry = msg.y + (msg.height - MIN(StringCnt(message) * SysFont[0].biHeight, msg.height)) / 2;
  WORD strindx = 0;
  bool draw = false;
  
  
  for(int i = 0;i < strl;i++)
  {
    errstr2[strindx] = message[i];
    if(message[i] == 0x0A) 
    {
      errstr2[strindx] = 0;
      strindx = 0;
      draw = true;
    }
    else
    {
      strindx++;
    }
    if(i == (strl-1))
    {
      errstr2[strindx] = 0;
      draw = true;
    }
    if(draw) 
    {
      draw = false;
      DrawText(msg.x, stry, errstr2, msg.width, algCenter);
      stry += SysFont[0].biHeight;
      if (stry >= msg.y + msg.height - SysFont[0].biHeight) break;
    }
  }

  if(timeout == 0) return 0;
  if(timeout < (0xFFFFFFFE / 100)) timeout = timeout * 100;
  else timeout = 0xFFFFFFFE;

  uint8_t menustate = 1, prevmenustate = 2;
  uint8_t chgbtnstate = 2, prevchgbtnstate = 2;
  uint8_t selbtnstate = 2, prevselbtnstate = 2;
  
  int i = timeout;
  while( i > 0)
  {
    i--;
    if(showmenu)
    {
      if(menustate != prevmenustate)
      {
        prevmenustate = menustate;
        if(menustate == 1)
        {
          ExchSysPal();
          DrawText(menu1.x, menu1.y, menutext1, menu1.width, algCenter);
          ExchSysPal();
          DrawText(menu2.x, menu2.y, menutext2, menu2.width, algCenter);
        }
        if(menustate == 2)
        {
          DrawText(menu1.x, menu1.y, menutext1, menu1.width, algCenter);
          ExchSysPal();
          DrawText(menu2.x, menu2.y, menutext2, menu2.width, algCenter);
          ExchSysPal();
        }
      }

      prevchgbtnstate = chgbtnstate;
      if(THardware::CheckKey(ChangeButton) == 0)
      {
        chgbtnstate = 0;
      }
      else if (chgbtnstate == 0) 
      {
        chgbtnstate = 1;
      }
      if (prevchgbtnstate != chgbtnstate)
      {
        i = timeout;
      }
      if ((prevchgbtnstate == 1) && (chgbtnstate == 0))
      {
        if (menustate == 1)
        {
          menustate = 2;
        }
        else
        {
          menustate = 1;
        }
      }

      prevselbtnstate = selbtnstate;
      if(THardware::CheckKey(SelectButton) == 0)
      {
        selbtnstate = 0;
      }
      else if (selbtnstate == 0) 
      {
        selbtnstate = 1;
      }
      if ((prevselbtnstate == 1) && (selbtnstate == 0))
      {
        return menustate;
      }
    }
    vTaskDelay(10/portTICK_RATE_MS);    
  }
  return 0;
}

//------------------------------------------------------------------------------------
/*
int TGuiObjects::MessageBox(uint16_t error,const char *message, bool showchange, bool showselect, const char *change, const char *select, uint32_t timeout)
{
#ifdef DISABLE_GUI
  return -1;
#endif

  if(message == NULL) return -1;

  char errstr2[40];
  char errstr[5];

  int strl = strlen(message);

  struct
  {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
  } img, msg, btns, mb, ch_img, ch_msg, sel_img, sel_msg;

  const uint8_t border = 2;
  const uint8_t indent = 10;

  //img
  if(error == 0)
  {
    img.width  = _warning_BitMap.biWidth;
    img.height = _warning_BitMap.biHeight;
  }
  else
  {
    strcpy(errstr, "E");
    itos(error, errstr2, 3, DECIMAL);
    strcat(errstr, errstr2);
    img.width  = MAX(_error_BitMap.biWidth, StringLen(errstr));
    img.height = _error_BitMap.biHeight + SysFont[0].biHeight + indent;
  }

  //msg
  msg.width = 0;
  int strcnt = 0;
  WORD strindx = 0;
  bool draw = false;
  for(int i = 0;  i < strl; i++)
  {
    errstr2[strindx] = message[i];
    if(message[i] == 0x0A) 
    {
      errstr2[strindx] = 0;
      strindx = 0;
      strcnt++;
      draw = true;
                }
    else
    {
      strindx++;
                }
    if(i == (strl-1))
    {
      errstr2[strindx] = 0;
      draw = true;
    }
    if(draw) 
    {
      draw = false;
      msg.width = MAX(msg.width, StringLen(errstr2));
    }
  }
  msg.height = (strcnt + 1) * SysFont[0].biHeight;

  //ch_img
  ch_img.width  = _button_change_BitMap.biWidth;
  ch_img.height = _button_change_BitMap.biHeight;

  //ch_msg
  ch_msg.width  = StringLen(change);
  ch_msg.height = SysFont[0].biHeight;

  //sel_img
  sel_img.width  = _button_select_BitMap.biWidth;
  sel_img.height = _button_select_BitMap.biHeight;

  //sel_msg
  sel_msg.width  = StringLen(select);
  sel_msg.height = SysFont[0].biHeight;

  //btns
  btns.width  = 0;
  btns.height = 0;
  if(showchange)
  {
    btns.width  += ch_img.width + ch_msg.width + indent;
    btns.height = MAX(btns.height, MAX(ch_img.height, ch_msg.height));
  }
  if(showselect)
  {
    btns.width  += sel_img.width + sel_msg.width + indent;
    btns.height = MAX(btns.height, MAX(sel_img.height, sel_msg.height));
  }
  if(showchange && showselect)
  {
    btns.width  += indent;
  }

  //mb
  if(showselect || showchange)
  {
    mb.width  = MAX(btns.width, img.width + msg.width + indent) + 2 * (border + indent);
    mb.height = MAX(img.height, msg.height) + btns.height + indent + 2 * (border + indent);
  }
  else
  {
    mb.width  = img.width + msg.width + indent + 2 * (border + indent);
    mb.height = MAX(img.height, msg.height) + 2 * (border + indent);
  }
  mb.x = (320 - mb.width) / 2;
  mb.y = 17 + 32 + ((240 - 2 * 17 - 32) - mb.height) / 2;

  //img
  img.x = mb.x + border + indent;
  img.y = mb.y + border + indent + (MAX(img.height, msg.height) - img.height) / 2;

  //msg
  msg.x = mb.x + border + 2 * indent + img.width;
  msg.width = mb.width - ( img.width + 2 * border + 3* indent);
  msg.y = mb.y + border + indent + (MAX(img.height, msg.height) - msg.height) / 2;

  //btns
  btns.x = mb.x + (mb.width - btns.width) / 2;
  btns.y = mb.y + border + 2 * indent + MAX(img.height, msg.height);


//        SetSysPal(0x001F, 0xFFFF);
        SetSysPal(0xC618, 0x0000);
  FillRectangle(mb.x,                     mb.y,                      mb.width, mb.height, SysPal[0]);
  FillRectangle(mb.x,                     mb.y,                      mb.width, border,    SysPal[1]);
  FillRectangle(mb.x + mb.width - border, mb.y,                      border,   mb.height, SysPal[1]);
  FillRectangle(mb.x,                     mb.y + mb.height - border, mb.width, border,    SysPal[1]);
  FillRectangle(mb.x,                     mb.y,                      border,   mb.height, SysPal[1]);

  if(error != 0)
  {
    PutBitmap(img.x + (img.width - _error_BitMap.biWidth) / 2, img.y, &_error_BitMap);
    DrawText(img.x, img.y + indent + _error_BitMap.biHeight, errstr, img.width, algCenter);
  }
  else
  {
    PutBitmap(img.x, img.y, &_warning_BitMap);
  }

  WORD strtindx = 0;
  strindx = 0;
  draw = false;
  for(int i = 0;i < strl;i++)
  {
    errstr2[strindx] = message[i];
    if(message[i] == 0x0A) 
    {
      errstr2[strindx] = 0;
      strindx = 0;
      draw = true;
    }
    else
    {
      strindx++;
//      if(strindx > 16) strindx = 16;
    }
    if(i == (strl-1))
    {
      errstr2[strindx] = 0;
      draw = true;
    }
    if(draw) 
    {
      draw = false;
      DrawText(msg.x, msg.y + strtindx, errstr2, msg.width, algCenter);
      strtindx += SysFont[0].biHeight;
    }
  }

  if(showchange && showselect)
  {
    //ch_img
    ch_img.x = btns.x;
    ch_img.y = btns.y + (btns.height - ch_img.height) / 2;
    //ch_img
    ch_msg.x = ch_img.x + ch_img.width + indent;
    ch_msg.y = btns.y + (btns.height - ch_msg.height) / 2;
    //sel_img
    sel_img.x = ch_msg.x + ch_msg.width + indent;
    sel_img.y = btns.y + (btns.height - sel_img.height) / 2;
    //ch_img
    sel_msg.x = sel_img.x + sel_img.width + indent;
    sel_msg.y = btns.y + (btns.height - sel_msg.height) / 2;

    PutBitmap(ch_img.x, ch_img.y, &_button_change_BitMap);
    DrawText(ch_msg.x, ch_msg.y, change, ch_msg.width, algLeft);
    PutBitmap(sel_img.x, sel_img.y, &_button_select_BitMap);
    DrawText(sel_msg.x, sel_msg.y, select, sel_msg.width, algLeft);
  }
  else if(showchange)
  {
    //ch_img
    ch_img.x = btns.x;
    ch_img.y = btns.y + (btns.height - ch_img.height) / 2;
    //ch_img
    ch_msg.x = ch_img.x + ch_img.width + indent;
    ch_msg.y = btns.y + (btns.height - ch_msg.height) / 2;

    PutBitmap(ch_img.x, ch_img.y, &_button_change_BitMap);
    DrawText(ch_msg.x, ch_msg.y, change, ch_msg.width, algLeft);
  }
  else if(showselect)
  {
    //sel_img
    sel_img.x = btns.x;
    sel_img.y = btns.y + (btns.height - sel_img.height) / 2;
    //ch_img
    sel_msg.x = sel_img.x + sel_img.width + indent;
    sel_msg.y = btns.y + (btns.height - sel_msg.height) / 2;

    PutBitmap(sel_img.x, sel_img.y, &_button_select_BitMap);
    DrawText(sel_msg.x, sel_msg.y, select, sel_msg.width, algLeft);
  }

  if(timeout < (0xFFFFFFFE / 10)) timeout = timeout * 10;
  if(timeout == 0) timeout = 0xFFFFFFFE;

  for(int i = 0; i < timeout; i++)
  {
    vTaskDelay(100/portTICK_RATE_MS);    
    if(showchange & (THardware::CheckKey(ChangeButton) != 0)) return 1;
    if(showselect & (THardware::CheckKey(SelectButton) != 0)) return 2;
  }
  return 0;
}
//------------------------------------------------------------------------------------
*/
void TGuiObjects::GO_TASK_GUIObj(void *pvParameters)
{
  portTickType xLastWakeTime;
  static char sTr[24];
  static char sTrv[48];
//  char sTrv2[48];
//  int bx_s = 10;
//  int OneSecUpd = 1;

  xLastWakeTime = xTaskGetTickCount();
  vTaskSuspend(NULL);
  int PrevGUIMode = guioNone;


  while(1)
  {
    SetSysFont(&_Arial_18[0]);
    SetSysPal(clBlack, clWhite);
    switch(GUIMode)
    {
      case guioNone:
      {
        if(GUIMode != PrevGUIMode)
        {
          PrevGUIMode = GUIMode; 
          FillRectangle(0, 0, 320, 240, SysPal[0]);
          LcdBL(false);
        }
        while(GUIMode == guioNone)
        {
          vTaskDelay(300/portTICK_RATE_MS);
        }
      }
      break;
      case guioWait:
      {
        if(GUIMode != PrevGUIMode)
        {
          PrevGUIMode = GUIMode; 
          LcdBL(false);
          if((!TAppProcessor::ExitFromRecordMode) && (!TAppProcessor::ErrorInRecordMode))
          {  
            SetSysFont(&_Mistral_72[0]);
            SetSysPal(clBlack, clCardian);
            MessageBox("Кардиан", false, "Да", "Нет", 0);
//            DrawText(0, 120-SysFont[0].biHeight/2, "Кардиан", 320, algCenter);
            SetSysPal(clBlack, clWhite);
            SetSysFont(&_Arial_18[0]);
          }
        }
        while(GUIMode == guioWait)
        {
          vTaskDelay(300/portTICK_RATE_MS);
        }
      }
      break;
      case guioError:
      {
        if(GUIMode != PrevGUIMode)
        {
          PrevGUIMode = GUIMode; 
          LcdBL(false);
          SetSysFont(&_Arial_18[0]);
//          SetSysFont(&_Arial_20[0]);
//          MessageBox("Ошибка!\n\nобратитесь в\nсервисную службу", false, "Да", "Нет", 0);
          MessageBox("Произошла ошибка.\nПерезапустите\nрегистратор.\nПри повторении\nобратитесь в\nсервисный центр.", false, "Да", "Нет", 0);
          SetSysFont(&_Arial_18[0]);
        }
        while(GUIMode == guioError)
        {
          vTaskSuspend(NULL);
          vTaskDelay(300/portTICK_RATE_MS);
        }
      }
      break;
      case guioCharge:
      {
        if(GUIMode != PrevGUIMode)
        {
          PrevGUIMode = GUIMode; 
//          LcdBL(false);
          SetSysFont(&_Arial_20[0]);
          MessageBox("Идет зарядка\nрезервного\nаккумулятора.\nПодождите...", false, "Да", "Нет", 0);
          SetSysFont(&_Arial_18[0]);
        }
        while(GUIMode == guioCharge)
        {
          vTaskDelay(1000/portTICK_RATE_MS);
           SetSysFont(&_PT_Mono_8[0]);
          float voltage = (float)TAppProcessor::SysVoltageValue / 1000.;
          DrawText(240, 20, ftoa(voltage,2), 80, algRight);
          SetSysFont(&_Arial_18[0]);
        }
      }
      break;
      case guioFirmwareUpdate:
      {
        if(GUIMode != PrevGUIMode)
        {
          Timeout = LCDBLOFF_TIMER;
          PrevGUIMode = GUIMode; 
          LcdBL(false);
          SetSysFont(&_Arial_20[0]);
          MessageBox("Идет обновление\nпрограмного\nобеспечения.\nПодождите...", false, "Да", "Нет", 0);
          SetSysFont(&_Arial_18[0]);
        }
        while(GUIMode == guioFirmwareUpdate)
        {
          vTaskDelay(300/portTICK_RATE_MS);
        }
      }
      break;
      case guioStartUpProgressBar:
      {
//        const uint8_t MaxProgressBar = 16;
//        const uint16_t PixelOffset = 240 / MaxProgressBar;
//        uint16_t newleft = 40;
//        uint16_t ProgressBarPositionStore = 0;
//        uint16_t ProgressBarPosition = 0;

        if(GUIMode != PrevGUIMode)
        {
          FillRectangle(0, 19, 320, 32, clBlack);
          PrevGUIMode = GUIMode; 

          SystemCoreClockUpdate();
          if(SystemCoreClock != 39936000)
          {
              SetSysPal(clBlack, clRed);
              float fsysclk = SystemCoreClock / 1000000.;
              char *ss = ftoa(fsysclk, 6);
              strcpy(sTrv, "Неверная\nтактовая частота\n");
              strcat(sTrv, ss);
              strcat(sTrv, " МГц");
              MessageBox(sTrv, false, "Да", "Нет", 1);
              SetSysPal(clBlack, clWhite);
          }
          else
          {
            if(TAppProcessor::ErrorInRecordMode)
            {
              SetSysPal(clBlack, clWhite);
              MessageBox("Ошибка при\nпродготовке записи.\n\nПопробуйте еще раз",false, "Да", "Нет", 0);
            }
            else if(!TAppProcessor::ExitFromRecordMode)
            {  
#ifdef DEMO_VERSION
              SetSysPal(clBlack, clRed);
              MessageBox("Кардиорегистратор\nКР-01\n\nДемонстрационная\nверсия", false, "Да", "Нет", 0);
#else //DEMO_VERSION
              SetSysPal(clBlack, clCardian);
              MessageBox("Кардиорегистратор\nКР-01", false, "Да", "Нет", 0);
#endif //DEMO_VERSION
              strcpy(sTrv, FW_RELEASE_STR);
              SetSysFont(&_PT_Mono_8[0]);
              DrawText(240, 20, sTrv, 80, algRight);
              SetSysFont(&_Arial_18[0]);
              SetSysPal(clBlack, clWhite);
            }
            else 
            {
              SetSysPal(clBlack, clWhite);
              SetSysFont(&_Arial_20[0]);
              MessageBox("Запись\nостановлена!",false, "Да", "Нет", 0);
//              DrawText(0, 120-SysFont[0].biHeight/2, "Запись остановлена!", 320, algCenter);
              SetSysFont(&_Arial_18[0]);
            }
          }
        }

        while(GUIMode == guioStartUpProgressBar)
        {
/*          if(ProgressBarPositionStore < TLcdTrace::StartingUpStages)
          {  
            uint16_t trytimes = (uint16_t)(TLcdTrace::StartingUpStages-ProgressBarPositionStore);
            ProgressBarPositionStore = TLcdTrace::StartingUpStages;
                  
            ProgressBarPosition += (PixelOffset*trytimes);
                  
            if(!TAppProcessor::ExitFromRecordMode)
            {  
              FillRectangle(newleft,240-35,ProgressBarPosition - newleft,2,TFT_CardianColor);
            }
            newleft = ProgressBarPosition;
                
            if(ProgressBarPositionStore >= MaxProgressBar) 
            {
              ProgressBarPosition = MaxProgressBar;
              break;
            }
          }
*/        vTaskDelay(100/portTICK_RATE_MS);    
         }
      }
      break;
      case guioDesktop:
      {
        Timeout = LCDBLOFF_TIMER;
        if(GUIMode != PrevGUIMode)
        {
          PrevGUIMode = GUIMode; 
//          MessageBox(" ",false, "Да", "Нет", 1);
          FillRectangle(0,0,320,240,clBlack);
//          PutBitmap(248,66, &_Mic_64x140_3_BitMap);
        }

        ToolbarsFlagsShow = true;

        while(GUIMode == guioDesktop)
        {
          static int  updrate = 0;
              
          if(updrate > 5)
          {  
            ShowToolbar();
            updrate = 0;
            ShowBattery();
          }
          updrate++;
              
          vTaskDelay(50/portTICK_RATE_MS); 
              
          if((TAppProcessor::StartECGRecording) && (TAppProcessor::StartNow))
          {
            if(!TSDProcessor::SDPresent)
            {
              TAppProcessor::StartECGRecording = false;
              MessageBox("Карта памяти\nотсутствует.\nЗапись\nневозможна.",false, "Да", "Нет", 5);
              MessageBox(" ",false, "", "", 0);
              TInterpreter::SetStartUSARTMonitioring(false);
              TDevice::SystemReset();
              break;
            }
            else if(TAppProcessor::cable_state == ctNoCable)
            {
              TAppProcessor::StartECGRecording = false;
              MessageBox("Кабель\nне подключен.\nЗапись\nневозможна.", false, "Да", "Нет", 5);
              MessageBox(" ",false, "", "", 0);
              TInterpreter::SetStartUSARTMonitioring(false);
              TDevice::SystemReset();
              break;
            }
            else
            {  
              TAppProcessor::StartRecording = true;

              WaitingECGStartRecord = true;
               MessageBox("Идет\nподготовка\nзаписи.\nЖдите...",false, "Да", "Нет", 1);
              while(TAppProcessor::ECGRecordingStarted == false)
              {
                vTaskDelay(100/portTICK_RATE_MS);
              }
              WaitingECGStartRecord = false;

              strcpy(sTrv,"");
              strcat(sTrv,(const char*)TAppProcessor::PatientCard.Name);
              strcat(sTrv, "\n");
              strcat(sTrv,(const char*)TAppProcessor::PatientCard.DateOfBirth);
              MessageBox(sTrv, false, "Да", "Нет", 0);
              
              GUIMode = guioRecMode;
              ButtonModeStart = true;
              vTaskDelay(100/portTICK_RATE_MS);
              break;
            }
          }
          else
          {
            if(THardware::CheckKey(SelectButton) != 0)
            {
              if(THardware::CheckKey(ChangeButton) != 0)
              {
                GUIMode = guioTestWindow;
                break;
              }
              if(TAppProcessor::cable_state == ctNoCable)
              {
                MessageBox("Кабель\nне подключен.\nЗапись\nневозможна.", false, "Да", "Нет", 5);
                MessageBox(" ", false, "Да", "Нет", 1);
                break;
              }
              if(TSDProcessor::SDPresent) 
              {
                int tmp = 0;
                
              
                tmp = MessageBox("Регистратор\nбудет запущен.\nПродолжить?",true, "Нет", "Да", 10);
                if (tmp != 2)
                {
                  FillRectangle(32,19+32,320-32,240-(19+32),0x0000);
                  DrawTime(40, 136 - _7seg_32[0].biHeight/2, 320-80, algCenter);
                  break;
                }

                if(TSDProcessor::TrMode != trverrGood)
                {
                  tmp = MessageBox("Карта памяти\nне считана.\nПродолжить?",true, "Нет", "Да", 10);
                }
                if (tmp != 2)
                {
                  FillRectangle(32,19+32,320-32,240-(19+32),0x0000);
                  DrawTime(40, 136 - _7seg_32[0].biHeight/2, 320-80, algCenter);
                  break;
                }

                if(!TSDProcessor::SDPresent)
                {
                  MessageBox("Карта памяти\nотсутствует.\nЗапись\nневозможна.",false, "Да", "Нет", 5);
                  FillRectangle(32,19+32,320-32,240-(19+32),0x0000);
                  break;
                }
                    
                GUIMode = guioECG;
                break;
              }
              else 
              {
                TGuiObjects::MessageBox("Карта памяти\nотсутствует.\nЗапись\nневозможна.",false, "Да", "Нет", 5);
                FillRectangle(32,19+32,320-32,240-(19+32),0x0000);
              }
              break;
            }
          }
        }
      }
      break;
      case guioECG:
      {
//        const int left = 0;
//        const int bot = 0;
//        const int height = 64;
//        const int width = 128;
//        const int top = bot + height;
//        const int right = left + width;
//        #define MID  (((64-12)/2)+0)
//        #define MID  104
        
        #define MID1  78
        #define MID2  136
        #define MID3  194
        static int leadPos = 0;
        static int Prevsawt1 = MID1;
        static int Prevsawt2 = MID2;
        static int Prevsawt3 = MID3;
//        static int indata = 0;
        int sawtooth1;
        int sawtooth2;
        int sawtooth3;
        static int timeUpdate = 60000/15;
//        bool PrevScreenOrient = ScreenOrientation;
          
        leadPos = 50;
        timeUpdate = 60000/15;

        Timeout = LCDBLOFF_TIMER;

        FillRectangle(0,50,320,191,clBlack);

        EcgOnScreen = true;
        if (TFrame::StartMonitioring /*|| TSDProcessor::SDStartWrPool*/ || TGuiObjects::EcgOnScreen) TInterpreter::SetStartUSARTMonitioring(true);
        else TInterpreter::SetStartUSARTMonitioring(false);
          
        TEcgProcessor::HR_Led_Enable = true;
      
        while(GUIMode == guioECG)
        {
          SetSysPal(clBlack, clWhite);
          SetSysFont(&_Arial_18[0]);
          PrevGUIMode = GUIMode; 

          if(leadPos == 50) 
          {
            if (TAppProcessor::cable_state == ct10Leads) 
            {
              if(SelectECGChannelToShow > 3) SelectECGChannelToShow = 0;
              if(SelectECGChannelToShow == 0)
              {
                DrawText(0, MID1 - SysFont[0].biHeight / 2, "I", 50, algCenter);
                DrawText(0, MID2 - SysFont[0].biHeight / 2, "II", 50, algCenter);
                DrawText(0, MID3 - SysFont[0].biHeight / 2, "III", 50, algCenter);
              }
              else if(SelectECGChannelToShow == 1)
              {
                DrawText(0, MID1 - SysFont[0].biHeight / 2, "aVR", 50, algCenter);
                DrawText(0, MID2 - SysFont[0].biHeight / 2, "aVL", 50, algCenter);
                DrawText(0, MID3 - SysFont[0].biHeight / 2, "aVF", 50, algCenter);
              }
              else if(SelectECGChannelToShow == 2)
              {
                DrawText(0, MID1 - SysFont[0].biHeight / 2, "V1", 50, algCenter);
                DrawText(0, MID2 - SysFont[0].biHeight / 2, "V2", 50, algCenter);
                DrawText(0, MID3 - SysFont[0].biHeight / 2, "V3", 50, algCenter);
              }
              else if(SelectECGChannelToShow == 3)
              {
                DrawText(0, MID1 - SysFont[0].biHeight / 2, "V4", 50, algCenter);
                DrawText(0, MID2 - SysFont[0].biHeight / 2, "V5", 50, algCenter);
                DrawText(0, MID3 - SysFont[0].biHeight / 2, "V6", 50, algCenter);
              }
            }
            else
            {
              if(SelectECGChannelToShow > 0) SelectECGChannelToShow = 0;
              if(SelectECGChannelToShow == 0)
              {
                DrawText(0, MID1 - SysFont[0].biHeight / 2, "1", 50, algCenter);
                DrawText(0, MID2 - SysFont[0].biHeight / 2, "2", 50, algCenter);
                DrawText(0, MID3 - SysFont[0].biHeight / 2, "3", 50, algCenter);
              }
            }
              
              
          }

          FillRectangle(leadPos,50,1,222-50+1,clBlack);
          FillRectangle(leadPos+1,MID1-5,2,11,clRed);
          FillRectangle(leadPos+1,MID2-5,2,11,clRed);
          FillRectangle(leadPos+1,MID3-5,2,11,clRed);
  
          sawtooth1  = (GetDataForScreen(0)/128) + MID1;
          sawtooth2 = (GetDataForScreen(1)/128) + MID2;
          sawtooth3 = (GetDataForScreen(2)/128) + MID3;

          if(sawtooth1 > 222) sawtooth1 = 222;
          else if(sawtooth1 < 51) sawtooth1 = 51;
          if(sawtooth1 < Prevsawt1) FillRectangle(leadPos,sawtooth1,1,Prevsawt1 - sawtooth1,clCardian);
          else if(sawtooth1 > Prevsawt1) FillRectangle(leadPos,Prevsawt1,1,sawtooth1 - Prevsawt1,clCardian);
          else FillRectangle(leadPos,sawtooth1,1,1,clCardian);

          if(sawtooth2 > 222) sawtooth2 = 222;
          else if(sawtooth2 < 51) sawtooth2 = 51;
          if(sawtooth2 < Prevsawt2) FillRectangle(leadPos,sawtooth2,1,Prevsawt2 - sawtooth2,clCardian);
          else if(sawtooth2 > Prevsawt2) FillRectangle(leadPos,Prevsawt2,1,sawtooth2 - Prevsawt2,clCardian);
          else FillRectangle(leadPos,sawtooth2,1,1,clCardian);

          if(sawtooth3 > 222) sawtooth3 = 222;
          else if(sawtooth3 < 51) sawtooth3 = 51;
          if(sawtooth3 < Prevsawt3) FillRectangle(leadPos,sawtooth3,1,Prevsawt3 - sawtooth3,clCardian);
          else if(sawtooth3 > Prevsawt3) FillRectangle(leadPos,Prevsawt3,1,sawtooth3 - Prevsawt3,clCardian);
          else FillRectangle(leadPos,sawtooth3,1,1,clCardian);

          Prevsawt1 = sawtooth1;   
          Prevsawt2 = sawtooth2;   
          Prevsawt3 = sawtooth3;   

          timeUpdate--;
          leadPos++;
          if(leadPos > 320) 
          {
            leadPos = 50;
            if (TAppProcessor::cable_state == ct10Leads) 
            {
              SelectECGChannelToShow++;
              if(SelectECGChannelToShow > 3) SelectECGChannelToShow = 0;
            }
            else
            {
              SelectECGChannelToShow = 0;
            }
              
              
          }
          if((leadPos % 30) == 0)
          {
/*            static int iFreq = 0;
            static uint8_t y = 0;
            static int AverageValue = 0;
              
            AverageValue += TEcgProcessor::Hartbeat.Value;
            y++;

            if( y == 3 )
            {
              AverageValue /= 3;
              float floatFreq = ( 1.0F / ((1.0F/((float)SystemCoreClock)) * ((float)AverageValue) * ((float)TIM_GetPrescaler( TIM2 ))) ) * 60.0F ;
              floatFreq += 0.5;
              iFreq = (int)floatFreq;
              y = 0;
              AverageValue = 0;
              if((iFreq > 20) && (iFreq < 310))   
              {
                if(TEcgProcessor::PaceInFrontEnd) strcpy(sTr,"---");
                else itos(iFreq,sTr,19);
              }
              else
              {
                strcpy(sTr,"---");
              }
              SetSysPal(TFT_BackColor,0xF800);
              TLcd::DrawText(128, 21, sTr, 48, algLeft);
            }

*/          }
              
          static int cnt = 4;
          if(cnt < 0)
          {                
            cnt = 4;
//            vTaskResume(xLcdInvalidate);
          }
          cnt--;
          vTaskDelay(16/portTICK_RATE_MS); 
          if(THardware::CheckKey(SelectButton) != 0)
          {
            int tmp = 0;
            FillRectangle(0,50,320,191,clBlack);
            tmp = MessageBox("Регистратор\nбудет запущен.\nПродолжить?",true, "Нет", "Да", 5);
            FillRectangle(0,50,320,191,clBlack);
            if (tmp == 2) 
            {
              if(TAppProcessor::cable_state == ctNoCable)
              {
                TAppProcessor::StartECGRecording = false;
                TInterpreter::SetStartUSARTMonitioring(false);
                MessageBox("Кабель\nне подключен.\nЗапись\nневозможна.",false, "Да", "Нет", 5);
                FillRectangle(32,19+32,320-32,240-(19+32),0x0000);
                TDevice::SystemReset();
                break;
              }
              
              if(!TSDProcessor::SDPresent)
              {
                TAppProcessor::StartECGRecording = false;
                TInterpreter::SetStartUSARTMonitioring(false);
                MessageBox("Карта памяти\nотсутствует.\nЗапись\nневозможна.",false, "Да", "Нет", 5);
                FillRectangle(32,19+32,320-32,240-(19+32),0x0000);
                TDevice::SystemReset();
                break;
              }

              TAppProcessor::StartRecording = true;

              WaitingECGStartRecord = true;
               MessageBox("Идет\nподготовка\nзаписи.\nЖдите...",false, "Да", "Нет", 1);
              while(TAppProcessor::ECGRecordingStarted == false)
              {
                vTaskDelay(100/portTICK_RATE_MS);
              }
              WaitingECGStartRecord = false;

              MessageBox("Регистратор\nзапущен\nв режим\nзаписи ЭКГ.",false, "Да", "Нет", 0);
              TLMX9838::TurnOffBluetooth();
              GUIMode = guioRecMode;
              ButtonModeStart = true;
              vTaskDelay(100/portTICK_RATE_MS);
              break;
            }
            else 
            {
              GUIMode = guioDesktop;
              break;
            }
          }
        }
        EcgOnScreen = false;
        if (TFrame::StartMonitioring /*|| TSDProcessor::SDStartWrPool*/ || TGuiObjects::EcgOnScreen) TInterpreter::SetStartUSARTMonitioring(true);
        else TInterpreter::SetStartUSARTMonitioring(false);
      }
      break;
      case guioRecMode:
      {
        ToolbarsFlagsShow = true;
        Timeout = LCDBLOFF_TIMER;

        if(GUIMode != PrevGUIMode)
        {
          PrevGUIMode = GUIMode; 
          vTaskDelay(5000/portTICK_RATE_MS);
          FillRectangle(0,19,320,240-19,0x0000);
//          LcdBL( false );
          ShowBattery();
          ShowToolbar();
        }

        while(GUIMode == guioRecMode)
        {
          ShowToolbar();
          ShowBattery();
//          if(TLcd::LcdOff) 
          if(GPIO_ReadInputDataBit(DISP_BL_PORT, DISP_BL_PIN) == Bit_RESET) 
          {
            EnterStopMode = true;
            vTaskSuspend(NULL);
            ShowToolbar();
          }
          else
          {
            vTaskDelay(100/portTICK_RATE_MS);
          }

          if(((THardware::CheckKey(ChangeButton) == 1) || (THardware::CheckKey(SelectButton) == 1)) && (TAudio::VoiceRec))
//          if(TAudio::VoiceRec)
          {
            LcdBL( true );
/*            
            int tmp = 0; 
            SetSysFont(&_Arial_18[0]);
            tmp = MessageBox("Записать\nголосовое\nсообщение?",true, "Да", "Нет", 5);
            if (tmp == 1) 
            {
              GUIMode = guioSoundRec; 
              break;
            }
            else 
            {
              FillRectangle(0,0,320,240,0x0000);
              ShowToolbar();
              LcdBL( false );
            }
*/
            GUIMode = guioSoundRec; 
            
          }
        }    
      }
      break;
      case guioSoundRec:
      {
        int tout = -1;
        int secs = 0;
        int mins = 0;

        PrevGUIMode = GUIMode; 
        MessageBox("Для записи\nсообщения\nнажмите и\nудерживайте\nлюбую кнопку", false, "Да", "Нет", 0);
        int timer = 20;
        while((timer > 0) && (tout <= 0))
        {
          if((THardware::CheckKey(ChangeButton) == 1) || (THardware::CheckKey(SelectButton) == 1))
          {
            tout = 3;
          }
          vTaskDelay(500/portTICK_RATE_MS);
          timer--;
        }
        if (tout <=0)
        {
          GUIMode = guioRecMode;
          break;
        }
        
        FillRectangle(0,0,320,240,0x0000);
        ShowToolbar();
        ShowBattery();
        PutBitmap(239, 17+32+(240-17-17-32-140)/2, &_Mic_64x140_green_BitMap);


        xLastWakeTime = xTaskGetTickCount();        
        while(tout > 0)
        {
          itodts(mins,sTrv,19);
          strcat(sTrv,":");  
          itodts(secs++,sTr,19);
          strcat(sTrv,sTr);      
          if(secs > 59) 
          {
            secs = 0;
            mins++;
          }

          SetSysFont(&_Courier_New_48[0]);
          DrawText(17,180-_Courier_New_48[0].biHeight/2,sTrv,320-64-17*3,algCenter);
          SetSysFont(&_Arial_18[0]);
          ShowToolbar();

            
          GPIO_SetBits(AudioPwr_GPIO_PORT,AudioPwr_GPIO_PIN);
          FillRectangle( 15 + 4 * 46, 71, 34, 64, clBlack);
          PutBitmap(15 + 0 * 46, 71, &_Wave_34x62_BitMap);
          vTaskDelayUntil(&xLastWakeTime, (200 / portTICK_RATE_MS));
          FillRectangle( 15 + 0 * 46, 71, 34, 64, clBlack );
          PutBitmap(15 + 1 * 46, 71, &_Wave_34x62_BitMap);
          vTaskDelayUntil(&xLastWakeTime, (200 / portTICK_RATE_MS));

          TAppProcessor::AudioRec = true;

          FillRectangle( 15 + 1 * 46, 71, 34, 64, clBlack);
          PutBitmap(15 + 2 * 46, 71, &_Wave_34x62_BitMap);
          vTaskDelayUntil(&xLastWakeTime, (200 / portTICK_RATE_MS));
          FillRectangle( 15 + 2 * 46, 71, 34, 64, clBlack);
          PutBitmap(15 + 3 * 46, 71, &_Wave_34x62_BitMap);
          vTaskDelayUntil(&xLastWakeTime, (200 / portTICK_RATE_MS));
          FillRectangle( 15 + 3 * 46, 71, 34, 64, clBlack);
          PutBitmap(15 + 4 * 46, 71, &_Wave_34x62_BitMap);
          vTaskDelayUntil(&xLastWakeTime, (200 / portTICK_RATE_MS));

          if((THardware::CheckKey(ChangeButton) == 1)||(THardware::CheckKey(SelectButton) == 1)) 
          {
            tout = 3;
          }
          else 
          {
            tout--;
          }
        }
        
        if(tout == 0)
        {
          FillRectangle( 0, 17+32, 320, 240-17-32-17, clBlack);
          MessageBox("Сообщение\nзаписано", false, "Да", "Нет", 5);
          ShowToolbar();
          tout = -1;
          TAppProcessor::AudioRec = false;
          GPIO_ResetBits(AudioPwr_GPIO_PORT,AudioPwr_GPIO_PIN);
        }
        GUIMode = guioRecMode;
      }
      break;
      case guioServiceMenu:
      {
        FillRectangle(0,19+32,320,240-19+32,0x0000);
        SetSysPal(0x0000, 0xFFFF);
        SetSysFont(&_PT_Mono_18[0]);
        DrawText(12,19+32+  32, "X :      g", 10*14, algLeft);  
        DrawText(12,19+32+  59, "Y :      g", 10*14, algLeft);  
        DrawText(12,19+32+  86, "Z :      g", 10*14, algLeft);  
        DrawText(12,19+32+ 113, "Av:      g", 10*14, algLeft);  
        DrawText(167,19+32+ 32, "Vb:      V", 10*14, algLeft);
        DrawText(167,19+32+ 59, "Vl:      V", 10*14, algLeft);
        DrawText(167,19+32+ 86, "Va:      V", 10*14, algLeft);
        DrawText(167,19+32+113, "Vs:      V", 10*14, algLeft);
        while(GUIMode == guioServiceMenu)
        {
          vTaskDelay(200/portTICK_RATE_MS);
            
          ShowBattery();

          signed char adxc = TAppProcessor::AX_CoordWr[0];
          int iadxc = (int)adxc;

          float fadxc = (float)iadxc / 71.;
          float fadxcAv = fadxc * fadxc;
          char *ss = ftoa(fadxc,2);
          DrawText(12+3*14,19+32+32,ss,6*14,algRight);  
          adxc = TAppProcessor::AX_CoordWr[1];
          iadxc = (int)adxc;
          fadxc = (float)iadxc / 71.;
          fadxcAv += fadxc * fadxc;
          ss = ftoa(fadxc,2);
          DrawText(12+3*14,19+32+59,ss,6*14,algRight);  
          adxc = TAppProcessor::AX_CoordWr[2];
          iadxc = (int)adxc;
          fadxc = (float)iadxc / 71.;
          fadxcAv += fadxc * fadxc;
          ss = ftoa(fadxc,2);
          DrawText(12+3*14,19+32+86,ss,6*14,algRight);  

          fadxcAv = sqrt(fadxcAv);
          ss = ftoa(fadxcAv,2);
          DrawText(12+3*14,19+32+113,ss,6*14,algRight);  
              
          float voltage = (float)TAppProcessor::BatVoltageValue / 1000.;
          DrawText(167+3*14,19+32+32,ftoa(voltage,2),6*14,algRight);  
          voltage = (float)TAppProcessor::LiVoltageValue / 1000.;
          DrawText(167+3*14,19+32+59,ftoa(voltage,2),6*14,algRight);  
          voltage = (float)TAppProcessor::AnalogVoltageValue / 1000.;
          DrawText(167+3*14,19+32+86,ftoa(voltage,2),6*14,algRight);  
          voltage = (float)TAppProcessor::SysVoltageValue / 1000.;
          DrawText(167+3*14,19+32+113,ftoa(voltage,2),6*14,algRight);  

          if(THardware::CheckKey(SelectButton) != 0)
          {
            while(THardware::CheckKey(SelectButton) != 0) vTaskDelay(15/portTICK_RATE_MS); 
            FillRectangle(0,19+32,320,240-19+32,0x0000);
            GUIMode = guioDesktop;
            vTaskDelay(100/portTICK_RATE_MS);
            break;
          }
        }
      }
      break;
      case guioTestWindow:
      {
        FillRectangle(0,19+32,320,240-19+32,0x0000);
        SetSysPal(0x0000, 0xFFFF);
        SetSysFont(&_PT_Mono_18[0]);
        DrawText(12,19+32+  32, "X :      g", 10*14, algLeft);  
        DrawText(12,19+32+  59, "Y :      g", 10*14, algLeft);  
        DrawText(12,19+32+  86, "Z :      g", 10*14, algLeft);  
        DrawText(12,19+32+ 113, "Av:      g", 10*14, algLeft);  
        DrawText(167,19+32+ 32, "Vb:      V", 10*14, algLeft);
        DrawText(167,19+32+ 59, "Vl:      V", 10*14, algLeft);
        DrawText(167,19+32+ 86, "Va:      V", 10*14, algLeft);
        DrawText(167,19+32+113, "Vs:      V", 10*14, algLeft);
        while(GUIMode == guioTestWindow)
        {
          vTaskDelay(200/portTICK_RATE_MS);
            
          ShowBattery();

          signed char adxc = TAppProcessor::AX_CoordWr[0];
          int iadxc = (int)adxc;
          
          float fadxc = (float)iadxc / 71.;
          float fadxcAv = fadxc * fadxc;
          char *ss = ftoa(fadxc,2);
          DrawText(12+3*14,19+32+32,ss,6*14,algRight);  
          adxc = TAppProcessor::AX_CoordWr[1];
          iadxc = (int)adxc;
          fadxc = (float)iadxc / 71.;
          fadxcAv += fadxc * fadxc;
          ss = ftoa(fadxc,2);
          DrawText(12+3*14,19+32+59,ss,6*14,algRight);  
          adxc = TAppProcessor::AX_CoordWr[2];
          iadxc = (int)adxc;
          fadxc = (float)iadxc / 71.;
          fadxcAv += fadxc * fadxc;
          ss = ftoa(fadxc,2);
          DrawText(12+3*14,19+32+86,ss,6*14,algRight);  

          fadxcAv = sqrt(fadxcAv);
          ss = ftoa(fadxcAv,2);
          DrawText(12+3*14,19+32+113,ss,6*14,algRight);  
              
          float voltage = (float)TAppProcessor::BatVoltageValue / 1000.;
          DrawText(167+3*14,19+32+32,ftoa(voltage,2),6*14,algRight);  
          voltage = (float)TAppProcessor::LiVoltageValue / 1000.;
          DrawText(167+3*14,19+32+59,ftoa(voltage,2),6*14,algRight);  
          voltage = (float)TAppProcessor::AnalogVoltageValue / 1000.;
          DrawText(167+3*14,19+32+86,ftoa(voltage,2),6*14,algRight);  
          voltage = (float)TAppProcessor::SysVoltageValue / 1000.;
          DrawText(167+3*14,19+32+113,ftoa(voltage,2),6*14,algRight);  

          if(THardware::CheckKey(SelectButton) != 0)
          {
            while(THardware::CheckKey(SelectButton) != 0) vTaskDelay(100/portTICK_RATE_MS); 
/*
            int tmp = 0; 
            SetSysFont(&_Arial_18[0]);
            tmp = MessageBox("Отключить\nрезервный\nаккумулятор?",true, "Нет", "Да", 5);
            if (tmp == 2) 
            {
              TInterpreter::SendFrameToHost(CMD_LIBAT_OFF);
            }
*/
            FillRectangle(0,19+32,320,240-19+32,0x0000);
            GUIMode = guioDesktop;
            vTaskDelay(100/portTICK_RATE_MS);
            break;
          }
        }
      }
      break;
      default:
      {
        vTaskSuspend(NULL);
      }
      break;
    }
    vTaskDelay(100/portTICK_RATE_MS); 
  }
}
//------------------------------------------------------------------------------------

void TGuiObjects::ToolbarSet(int flags,bool Set)
{
#ifdef DISABLE_GUI
  return;
#endif
  static int ToolbarsFlagsStore = 100;

  if(Set)
    ToolbarsFlags |= flags;
  else   
    ToolbarsFlags &= ~flags;

  if(ToolbarsFlags != ToolbarsFlagsStore) ToolbarsFlagsShow = true;
  
  ToolbarsFlagsStore = ToolbarsFlags;

  if(ToolbarsFlagsShow)  
    {
      if(GUIMode == guioDesktop)
      {
        GUIDesktopShowTime();
      }
    }
}
//------------------------------------------------------------------------------------

void TGuiObjects::GUIDesktopShowProgressBar()
{
#ifdef DISABLE_GUI
  return;
#endif
//  if(!DisableLcdTrace) return;
  GUIMode = guioStartUpProgressBar;  
  vTaskResume(xGUIObj);
}
//------------------------------------------------------------------------------------
void TGuiObjects::GUIDesktopShowNone()
{
#ifdef DISABLE_GUI
  return;
#endif
//  if(!DisableLcdTrace) return;
  GUIMode = guioNone;  
  vTaskResume(xGUIObj);
}
//------------------------------------------------------------------------------------
void TGuiObjects::GUIDesktopShowWait()
{
#ifdef DISABLE_GUI
  return;
#endif
//  if(!DisableLcdTrace) return;
  GUIMode = guioWait;  
  vTaskResume(xGUIObj);
}
//------------------------------------------------------------------------------------
void TGuiObjects::GUIDesktopShowError()
{
#ifdef DISABLE_GUI
  return;
#endif
//  if(!DisableLcdTrace) return;
  GUIMode = guioError;  
  vTaskResume(xGUIObj);
}
//------------------------------------------------------------------------------------
void TGuiObjects::GUIDesktopShowCharge()
{
#ifdef DISABLE_GUI
  return;
#endif
//  if(!DisableLcdTrace) return;
  GUIMode = guioCharge;  
  vTaskResume(xGUIObj);
}
//------------------------------------------------------------------------------------
void TGuiObjects::GUIDesktopShowFirmwareUpdate()
{
#ifdef DISABLE_GUI
  return;
#endif
//  if(!DisableLcdTrace) return;
  GUIMode = guioFirmwareUpdate;  
  vTaskResume(xGUIObj);
}
//------------------------------------------------------------------------------------

void TGuiObjects::GUIDesktopShowTime()
{
#ifdef DISABLE_GUI
  return;
#endif
  #ifdef TEST_ON_TOP
    GUITestWindowShow(); //////////
  #else
//    if(!DisableLcdTrace) return;
    GUIMode = guioDesktop;
    if(!TLcd::LcdOff) vTaskResume(xGUIObj);
  #endif
}
//------------------------------------------------------------------------------------

void TGuiObjects::GUITestWindowShow()
{
#ifdef DISABLE_GUI
  return;
#endif
  GUIMode = guioTestWindow;
  vTaskResume(xGUIObj);
}
//------------------------------------------------------------------------------------

void TGuiObjects::GUIShowECG()
{
#ifdef DISABLE_GUI
  return;
#endif
  GUIMode = guioECG;
  ECG_Enstead_Time = true;
  SelectECGChannelToShow = 0;  
  vTaskResume(xGUIObj);
}
//------------------------------------------------------------------------------------

void TGuiObjects::GUIHideECG()
{
#ifdef DISABLE_GUI
  return;
#endif
  ECG_Enstead_Time = false;
}
//------------------------------------------------------------------------------------

void TGuiObjects::GUIHideBatteryFail()
{
#ifdef DISABLE_GUI
  return;
#endif
  vTaskResume(xGUIObj);
}
//------------------------------------------------------------------------------------

void TGuiObjects::DrawTime(uint16_t x, uint16_t y, uint16_t lenpixels, uint8_t align)
{
  char timestr[6];
  int yr, m, d, hr, mi, se;
  TRtc::RTC_GetTDMY(&yr,&m,&d,&hr,&mi,&se);
  timestr[0] = 0x30 + (hr / 10);
  timestr[1] = 0x30 + (hr % 10);
  timestr[2] = ':';
  timestr[3] = 0x30 + (mi / 10);
  timestr[4] = 0x30 + (mi % 10);
  timestr[5] = 0;
//  SetSysFont(&_7seg_32[0]);
//  DrawText(x, y, timestr, lenpixels, align);
  SetSysFont(&_Arial_18[0]);
}

void TGuiObjects::DrawDate(uint16_t x, uint16_t y, uint16_t lenpixels, uint8_t align)
{
  char datestr[11];
  int yr,m,d,hr,mi,se;
  TRtc::RTC_GetTDMY(&yr,&m,&d,&hr,&mi,&se);
  datestr[0] = 0x30 + (d / 10);
  datestr[1] = 0x30 + (d % 10);
  datestr[2] = '.';
  datestr[3] = 0x30 + (m / 10);
  datestr[4] = 0x30 + (m % 10);
  datestr[5] = '.';
  datestr[6] = 0x30 + (yr / 1000);
  yr = yr % 1000;
  datestr[7] = 0x30 + (yr / 100);
  yr = yr % 100;
  datestr[8] = 0x30 + (yr / 10);
  datestr[9] = 0x30 + (yr % 10);
  datestr[10] = 0;
//  DrawText(x, y, datestr, lenpixels, align);
}

void TGuiObjects::ShowToolbar()
{
#ifdef DISABLE_GUI
  return;
#endif
  
  const uint16_t top = 19;
//  const uint16_t height = 32;
//  const uint16_t bot = top + height;
  const uint16_t right = 320 - 54;
  const uint16_t left = 150;
//  const uint16_t width = right - left;
  uint16_t tbIndex = right;


  if(ToolbarsFlagsShow /*&& (GUIMode != guioRecMode)*/)
  {
    Timeout = LCDBLOFF_TIMER;
    TLcd::SetSysPal(0x0000, 0xFFFF);
//    DrawTime(0, 21, 80, algCenter);
//    DrawDate(0, 21, 150, algCenter);
//    FillRectangle(left,top,width,height,0x0000);
      
    if(ToolbarsFlags & TBFM_BLUETOOTH)
    {
     tbIndex -= _BtActive_32x32_BitMap.biWidth;
     PutBitmap(tbIndex, top, &_BtActive_32x32_BitMap);  
    }
    if(ToolbarsFlags & TBFM_BLUETOOTHONAIR)
    {
     tbIndex -= _BtOnLine_32x32_BitMap.biWidth;
     PutBitmap(tbIndex, top, &_BtOnLine_32x32_BitMap);  
    }
    if(ToolbarsFlags & TBFM_MEMCARD)
    {
      tbIndex -= _SDCard_42x32_BitMap.biWidth;
      PutBitmap(tbIndex, top, &_SDCard_42x32_BitMap);
    }
    if(ToolbarsFlags & TBFM_ECG)
    {
//      tbIndex -= _tbECG_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbECG_BMP);
    }
    if(ToolbarsFlags & TBFM_ECGBP)
    {
//      tbIndex -= _tbECG_BP_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbECG_BP_BMP);
    }
    if(ToolbarsFlags & TBFM_PACE)
    {
//      tbIndex -= _tbPace_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbPace_BMP);
    }
    if(ToolbarsFlags & TBFM_PATIENT)
    {
//      tbIndex -= _tbPatient_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbPatient_BMP);
    }
    if(ToolbarsFlags & TBFM_DEFAULTPATIENT)
    {
//      tbIndex -= _tbPatientDef_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbPatientDef_BMP);
    }
    if(ToolbarsFlags & TBFM_STORE)
    {
//      tbIndex -= _tbStore2_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbStore2_BMP);
    }
    if(ToolbarsFlags & TBFM_NOSOUND)
    {
//      tbIndex -= _tbNoSound_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbNoSound_BMP);
    }
    if(ToolbarsFlags & TBFM_BATTERYFAIL)
    {
//      tbIndex -= _tbBattFail_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbBattFail_BMP);
    }
    if(ToolbarsFlags & TBFM_SLEEP)
    {
//      tbIndex -= _tbSleep_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbSleep_BMP);
    }
    if(ToolbarsFlags & TBFM_MANUALSTART)
    {
//      tbIndex -= _tbHand_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbHand_BMP);
    }
    if(ToolbarsFlags & TBFM_PLANNEDSTART)
    {
//      tbIndex -= _tbPlanned_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbPlanned_BMP);
    }
    if(ToolbarsFlags & TBFM_REPEATEDSTART)
    {
//      tbIndex -= _tbRepeated_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbRepeated_BMP);
    }
    if(ToolbarsFlags & TBFM_STORED)
    {
//      tbIndex -= _tbStored2_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbStored2_BMP);
    }
    if(ToolbarsFlags & TBFM_SYNC)
    {
//      tbIndex -= _tbSync_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbSync_BMP);
    }
    if(ToolbarsFlags & TBFM_NOLEAD)
    {
//      tbIndex -= _Lead_N_42x32_BitMap.biWidth;
//      PutBitmap(tbIndex, top, &_Lead_N_42x32_BitMap);  
    }
    if(ToolbarsFlags & TBFM_LEAD5)
    {
//      tbIndex -= _Lead_5_42x32_BitMap.biWidth;
//      PutBitmap(tbIndex, top, &_Lead_5_42x32_BitMap);  
    }
    if(ToolbarsFlags & TBFM_LEAD7)
    {
//      tbIndex -= _Lead_7_42x32_BitMap.biWidth;
//      PutBitmap(tbIndex, top, &_Lead_7_42x32_BitMap);  
    }
    if(ToolbarsFlags & TBFM_LEAD10)
    {
//      tbIndex -= _Lead_10_42x32_BitMap.biWidth;
//      PutBitmap(tbIndex, top, &_Lead_10_42x32_BitMap);  
    }
    if(ToolbarsFlags & TBFM_ERROR)
    {
//      tbIndex += _tbErr_BMP.biWidth;
//      PutBitmap(tbIndex, top, &_tbErr_BMP);
    }
    
    if(GUIMode != guioECG)
    {
      if(TAppProcessor::StartRecording)
      {
        SetSysPal(clBlack, clWhite);
//        DrawText(0, 17 + 32 / 2 - SysFont[0].biHeight/2,"R",_Red_BitMap.biWidth,algCenter);
        PutBitmap(0, top, &_Record_BitMap);
      }
      else
      {
        BYTE EnMode[1];
      
        if((!(ToolbarsFlags & TBFM_NOLEAD)) && (ToolbarsFlags & TBFM_MEMCARD))
        {
          PutBitmap(0, top, &_Green_BitMap);
          EnMode[0] = 0;
          TInterpreter::SendFrameToHost(CMD_SET_LEDMODE,1,EnMode);
        }
        else
        {
          PutBitmap(0, top, &_Red_BitMap);  
          EnMode[0] = 1;
          TInterpreter::SendFrameToHost(CMD_SET_LEDMODE,1,EnMode);
        }
      }
    }
#ifdef DEMO_VERSION
    SetSysPal(clBlack, clRed);
    DrawText(_Green_BitMap.biWidth, top + 2, "DEMO", StringLen("DEMO") + 20, algCenter);
    SetSysPal(clBlack, clWhite);
#endif    

    FillRectangle(left,19,tbIndex-left,32,0x0000);

    ToolbarsFlagsShow = false;
    ShowBattery();
  }  
}
//------------------------------------------------------------------------------------

#define NICD_CHARGE_LVL0_THR      2250
#define NICD_CHARGE_LVL1_THR      2410
#define NICD_CHARGE_LVL2_THR      2450
#define NICD_CHARGE_LVL3_THR      2490
#define NICD_CHARGE_LVL4_THR      2550
#define NICD_CHARGE_LVL_HYST        10


void TGuiObjects::ShowBattery(void)
{
  const uint16_t left = 319-52;
  const uint16_t top = 19;
//  const int width = 109;//128;

//  int b_x = TAppProcessor::BatVoltageValue;
//  uint8_t prevLvl = TAppProcessor::BatVoltageLevel;
  uint8_t currLvl = TAppProcessor::BatVoltageLevel;
  
  do
  {
    currLvl = TAppProcessor::BatVoltageLevel;
    switch (TAppProcessor::BatVoltageLevel)
    {
      case 0:
        if (TAppProcessor::BatVoltageValue > NICD_CHARGE_LVL0_THR + NICD_CHARGE_LVL_HYST) 
          TAppProcessor::BatVoltageLevel++;
        break;
      case 1:
        if (TAppProcessor::BatVoltageValue > NICD_CHARGE_LVL1_THR + NICD_CHARGE_LVL_HYST) 
          TAppProcessor::BatVoltageLevel++;
        else if (TAppProcessor::BatVoltageValue < NICD_CHARGE_LVL0_THR - NICD_CHARGE_LVL_HYST) 
          TAppProcessor::BatVoltageLevel--;
        break;
      case 2:
        if (TAppProcessor::BatVoltageValue > NICD_CHARGE_LVL2_THR + NICD_CHARGE_LVL_HYST) 
          TAppProcessor::BatVoltageLevel++;
        else if (TAppProcessor::BatVoltageValue < NICD_CHARGE_LVL1_THR - NICD_CHARGE_LVL_HYST) 
          TAppProcessor::BatVoltageLevel--;
        break;
      case 3:
        if (TAppProcessor::BatVoltageValue > NICD_CHARGE_LVL3_THR + NICD_CHARGE_LVL_HYST) 
          TAppProcessor::BatVoltageLevel++;
        else if (TAppProcessor::BatVoltageValue < NICD_CHARGE_LVL2_THR - NICD_CHARGE_LVL_HYST) 
          TAppProcessor::BatVoltageLevel--;
        break;
      case 4:
        if (TAppProcessor::BatVoltageValue > NICD_CHARGE_LVL4_THR + NICD_CHARGE_LVL_HYST) 
          TAppProcessor::BatVoltageLevel++;
        else if (TAppProcessor::BatVoltageValue < NICD_CHARGE_LVL3_THR - NICD_CHARGE_LVL_HYST) 
          TAppProcessor::BatVoltageLevel--;
        break;
      case 5:
        if (TAppProcessor::BatVoltageValue < NICD_CHARGE_LVL4_THR - NICD_CHARGE_LVL_HYST) 
          TAppProcessor::BatVoltageLevel--;
        break;
      default:
        TAppProcessor::BatVoltageLevel = 0;
    } 
  } while (currLvl != TAppProcessor::BatVoltageLevel);

/*
  if(b_x < 2400) bx_set = 0;
  else  if(b_x < 2466) bx_set = 1;  
        else if(b_x < 2533) bx_set = 2;  
             else if(b_x < 2600) bx_set = 3;  
                  else bx_set = 4;
*/
//  if(TAppProcessor::BatVoltageLevel != prevLvl)      
  {
    switch(TAppProcessor::BatVoltageLevel)
    {
      default:
      case 0:
        PutBitmap(left,top, &_tbBattery0_52x32_BitMap);  
        break;
      case 1:
        PutBitmap(left,top, &_tbBattery1_52x32_BitMap);  
        break;
      case 2:
        PutBitmap(left,top, &_tbBattery2_52x32_BitMap);  
        break;
      case 3:
        PutBitmap(left,top, &_tbBattery3_52x32_BitMap);  
        break;
      case 4:
        PutBitmap(left,top, &_tbBattery4_52x32_BitMap);  
        break;
      case 5:
        PutBitmap(left,top, &_tbBattery5_52x32_BitMap);  
        break;
    }
  }        
}
//------------------------------------------------------------------------------------

void TGuiObjects::goButtonReleaseEvent()
{
  MenuSel++;
  if(MenuSel > menuMax) MenuSel = 0;
//  GUIMenuShow();
}
//------------------------------------------------------------------------------------

void TGuiObjects::menuSetDefaults()
{
  menuItem0Ptr = mitDef0;
  menuItem1Ptr = mitDef1;
  menuItem2Ptr = mitDef2;
  menuItem3Ptr = mitDef3;
  
  menuMax = 3;
}
//------------------------------------------------------------------------------------

void TGuiObjects::menuSetItem(int item,char *str)
{
  switch(item)
  {
    case 0:
      menuItem0Ptr = str;
      break;
    case 1:
      menuItem1Ptr = str;
      break;
    case 2:
      menuItem2Ptr = str;
      break;
    case 3:
      menuItem3Ptr = str;
      break;
  }
}
//------------------------------------------------------------------------------------

int smmin = 0, smmax = 0, smmin1 = 0, smmax1 = 0, smmin2 = 0, smmax2 = 0;

void TGuiObjects::AddECGArrayToScreen(EcgData_t *Data)
{
#ifdef DISABLE_GUI
  return;
#endif
  int sData[12];
  static int prevCh = 0;
  
  if(prevCh != SelectECGChannelToShow)
  {
    smmin = 0;
    smmax = 0;
    smmin1 = 0;
    smmax1 = 0;
    smmin2 = 0;
    smmax2 = 0;
    prevCh = SelectECGChannelToShow;
  }

  if (TAppProcessor::cable_state == ct10Leads) 
  {
    for(uint8_t i = 0; i < 8; i++)
    {
      sData[i] = Data->Channel[i].ChannelData;
    }

    int smpltmp = (sData[0] + sData[1]) / 3;
    sData[11] = sData[7] - smpltmp;
    sData[10] = sData[6] - smpltmp;
    sData[9]  = sData[5] - smpltmp;
    sData[8]  = sData[4] - smpltmp;
    sData[7]  = sData[3] - smpltmp;
    sData[6]  = sData[2] - smpltmp;
    sData[5]  = sData[1] - sData[0] / 2;
    sData[4]  = sData[0] - sData[1] / 2;
    sData[3]  = - (sData[0] + sData[1]) / 2;
    sData[2]  = sData[1] - sData[0];

    smpltmp = sData[3 * SelectECGChannelToShow + 0];
    if(smpltmp > smmax) smmax = smpltmp;
    if(smpltmp < smmin) smmin = smpltmp;

    smpltmp = sData[3 * SelectECGChannelToShow + 1];
    if(smpltmp > smmax1) smmax1 = smpltmp;
    if(smpltmp < smmin1) smmin1 = smpltmp;

    smpltmp = sData[3 * SelectECGChannelToShow + 2];
    if(smpltmp > smmax2) smmax2 = smpltmp;
    if(smpltmp < smmin2) smmin2 = smpltmp;

  }
  else
  {
    for(uint8_t i = 0; i < 3; i++)
    {
      sData[i] = Data->Channel[i].ChannelData;
    }

    int smpltmp = sData[0];
    if(smpltmp > smmax) smmax = smpltmp;
    if(smpltmp < smmin) smmin = smpltmp;

    smpltmp = sData[1];
    if(smpltmp > smmax1) smmax1 = smpltmp;
    if(smpltmp < smmin1) smmin1 = smpltmp;

    smpltmp = sData[2];
    if(smpltmp > smmax2) smmax2 = smpltmp;
    if(smpltmp < smmin2) smmin2 = smpltmp;
  }
}
//------------------------------------------------------------------------------------

int TGuiObjects::GetDataForScreen()
{
  int _SampleToShow;
  
  if(smmax > labs(smmin))  _SampleToShow = smmax;
  else _SampleToShow = smmin;  
    
  smmax = 0;
  smmin = 0;
  
  return _SampleToShow;  
}
//------------------------------------------------------------------------------------

int TGuiObjects::GetDataForScreen(uint8_t n)
{
  int _SampleToShow = 0;
  
  switch(n)
  {
    case 0:
      if(smmax > labs(smmin)) _SampleToShow = smmax;
      else _SampleToShow = smmin;  
      smmax = 0;
      smmin = 0;
      break;
    case 1:
      if(smmax1 > labs(smmin1)) _SampleToShow = smmax1;
      else _SampleToShow = smmin1;  
      smmax1 = 0;
      smmin1 = 0;
      break;
    case 2:
      if(smmax2 > labs(smmin2)) _SampleToShow = smmax2;
      else _SampleToShow = smmin2;  
      smmax2 = 0;
      smmin2 = 0;
      break;
  }
  return _SampleToShow;  
}
//------------------------------------------------------------------------------------

void TGuiObjects::ShowHR(int hr)
{
#ifdef DISABLE_GUI
  return;
#endif
  static char hrstr[5];

  if(GUIMode == guioDesktop)
  {
//    GUI_PutBitmap(128-16,40,&_NIBP_heart_BMP,LCD_PIXEL_ON);
    if((hr < 35) || (hr > 270)) 
      strcpy(hrstr,"---");
    else  
      itos(hr,hrstr,3,DECIMAL);  
//    GUI_DrawText(128-20,30,hrstr,19,algCenter,fntVerdana7,LCD_PIXEL_ON);
  }
}
//------------------------------------------------------------------------------------

/*
void TGuiObjects::HideHeart()
{
  if(GUIMode == guioDesktop)
  {
    TGui::GUI_ClearFillRectangle(128-16,40,128,49,LCD_PIXEL_OFF);
  }
}

void TGuiObjects::HideHeartAndHR()
{
  if(GUIMode == guioDesktop)
  {
    TGui::GUI_ClearFillRectangle(128-20,30,128,49,LCD_PIXEL_OFF);
  }
}
*/

//------------------------------------------------------------------------------------

void TGuiObjects::GO_TASK_ScrOff(void *pvParameters)
{
  static int stage = 5000;
  static bool bton_h = false;
  
//  int Timeout = TFlash::SF_GetData16(LCDOFF_TIMER); //1000
  int StandbyTimeout = 0;
  bool PrevSDState = TSDProcessor::SDPresent;
  bool PrevBTState = TLMX9838::LMX9838_Connected;
  bool PrevStartNowState = TAppProcessor::StartNow; 
  
  vTaskDelay(10000/portTICK_RATE_MS);
  while(1)
  {
    vTaskDelay(10/portTICK_RATE_MS);
    
    if((GUIMode == guioECG) || (GUIMode == guioTestWindow)) Timeout = 100*60*30; //30min
    if(Timeout == 0)
    {
      vTaskSuspend(xGUIObj);
//      TDevice::LcdDisable();
      LcdBL( false );
      TLcd::ColorInversion(true);
      stage = 1;
      BlockKeys = true;
    }
    Timeout--;
    
    if(BlockKeys)
    {
      if((TAppProcessor::RecordInProgress) || (TAppProcessor::Setup.u._bits.UseMonitoring))
        StandbyTimeout = 0;
      else StandbyTimeout++;
#ifdef STENDBUY_NOT_USE
      if(StandbyTimeout > (99*POWERDOWN_TOUT*60)) StandbyTimeout = 10;
#endif  
      if(StandbyTimeout > (100*POWERDOWN_TOUT*60))
      {
#ifdef LED_PROCESSOR
        TDevice::LedOff();
#endif  
  //Turn off bluetooth and ADS1298 
//        TADS1298::StandBy();
        TLMX9838::TurnOffBluetooth();
        vTaskDelay(500/portTICK_RATE_MS);
        
//#warning TURN OFF OTHER PERIPHERALS
        taskENTER_CRITICAL();
        PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
        TAppProcessor::RCC_Configuration_Main();
        taskEXIT_CRITICAL();

        TDevice::DEVICE->Ecg.Enable();
        bton_h = true;
//        while(1) vTaskDelay(1000/portTICK_RATE_MS);
        StandbyTimeout = 0;
//        TDevice::LcdEnable();
        TLcd::ColorInversion(false);
        LcdBL( true );
        while(TLcd::LcdOff); 
        bton_h = false;
        vTaskDelay(100/portTICK_RATE_MS);
        TFrame::TurnOnBluetooth();
#ifdef USE_ADS1292R
//        TADS1292::StartConversation();
#endif // USE_ADS1292R
//        TADS1298::StartConversation();
      }
    }
    else 
    {
      StandbyTimeout = 0;
    }
    
#ifdef LED_PROCESSOR
    if(StandbyLedEnable)
    {  
      switch(stage)
      {
        case 10:
//!!!          TDevice::LedOn();
          break;
        case 20:
          TDevice::LedOff();
          break;
        case 30:
//!!!          TDevice::LedOn();
          break;
        case 40:
          TDevice::LedOff();
          break;
        case 600:
          stage = 0;
          break;
        default:
//          stage = 0;
          break;
      }
      stage++;
    }
#endif
 
    if(((GPIO_ReadInputDataBit(UB1_BUTTON_PORT,UB1_BUTTON_PIN) == 0) 
      || (GPIO_ReadInputDataBit(UB2_BUTTON_PORT,UB2_BUTTON_PIN) == 0)
      || (WaitingECGStartRecord)
      || (TAppProcessor::StartNow != PrevStartNowState)
      || (TSDProcessor::SDPresent != PrevSDState)
      || (TLMX9838::LMX9838_Connected != PrevBTState))
      /*&& (TAppProcessor::SysVoltageValue > 3100)*/)
    {
      StandbyTimeout = 0;
//      Timeout = TFlash::SF_GetData16(LCDOFF_TIMER);
      Timeout = LCDBLOFF_TIMER;
      PrevSDState = TSDProcessor::SDPresent;
      PrevBTState = TLMX9838::LMX9838_Connected;
      if(TLcd::LcdOff) 
      {
        stage = 5000;
        TDevice::LcdEnable();

        while(TLcd::LcdOff); 

//        vTaskResume(TLcd::xLcdInvalidate);
        if(bton_h)
        { 
          bton_h = false;
          vTaskDelay(100/portTICK_RATE_MS);
          TFrame::TurnOnBluetooth();
        }
      }
                        TLcd::ColorInversion(false);
      LcdBL( true );
      vTaskResume(xGUIObj);
      ToolbarsFlagsShow = true;
//      ShowToolbar();
      while((GPIO_ReadInputDataBit(UB1_BUTTON_PORT,UB1_BUTTON_PIN) == 0) 
        || (GPIO_ReadInputDataBit(UB2_BUTTON_PORT,UB2_BUTTON_PIN) == 0)) 
      {
//        Timeout = TFlash::SF_GetData16(LCDOFF_TIMER);
        Timeout = 3000;
        vTaskDelay(100/portTICK_RATE_MS);
      }
      BlockKeys = false;
    }
    PrevSDState = TSDProcessor::SDPresent;
    PrevBTState = TLMX9838::LMX9838_Connected;
    PrevStartNowState = TAppProcessor::StartNow; 
  }
}
//------------------------------------------------------------------------------------

