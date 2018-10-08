/*
    Title:    
    Author:   VN Asliuk(Cardian)
    Date:     09.11.2010 13:21:36
    Purpose:  
    Software: 
    Hardware: 
    Note:    
*/
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#ifndef TGUIOBJECTS_H
#define TGUIOBJECTS_H
//------------------------------------------------------------------------------------
#include "TLcd.h"
#include "TFont.h"
#include "TEcgProcessor.h"
//------------------------------------------------------------------------------------
enum _GuiModes { guioNone = 0,
                 guioWait,
                 guioError,
                 guioCharge,
                 guioFirmwareUpdate,
                 guioStartUpProgressBar,
		             guioDesktop,
                 guioECG,
                 guioRecMode,
                 guioSoundRec,
                 guioServiceMenu,
                 guioTestWindow};


//------------------------------------------------------------------------------------
#define TBFM_OFFSET 1
//------------------------------------------------------------------------------------
#define TBFM_BLUETOOTH 			0x00000001
#define TBFM_SLEEP     			0x00000002
#define TBFM_PATIENT   			0x00000004
#define TBFM_NOSOUND   			0x00000008
#define TBFM_BATTERYFAIL	   	0x00000010
#define TBFM_MANUALSTART  		0x00000020
#define TBFM_BLUETOOTHONAIR		0x00000040
#define TBFM_SYNC			0x00000080
#define TBFM_STORE			0x00000100
#define TBFM_DEFAULTPATIENT		0x00000200
#define TBFM_PLANNEDSTART		0x00000400
#define TBFM_REPEATEDSTART		0x00000800
#define TBFM_ECG  			0x00001000
#define TBFM_PACE  			0x00002000
#define TBFM_ECGBP 			0x00004000
#define TBFM_STORED			0x00008000
#define TBFM_MEMCARD			0x00010000
#define TBFM_ERROR  			0x00020000
#define TBFM_NOLEAD  			0x00040000
#define TBFM_LEAD5   			0x00080000
#define TBFM_LEAD7  			0x00100000
#define TBFM_LEAD10  			0x00200000
//------------------------------------------------------------------------------------
#define LED_PROCESSOR
//#define DISABLE_GUI
#define POWERDOWN_TOUT 10 //min

#define LCDBLOFF_TIMER 30*100 //sec
//------------------------------------------------------------------------------------
typedef struct
  {
   	WORD width;
   	WORD height;
   	WORD length;
   	const BYTE *bmpArr;
  } SBmp; 

class TGui : public TFont
{
	public:
		static void GUI_Init();

		static void GUI_PutBitmap( int x, int y, const SBmp *_bmp, int _InvertedShow);

		static void GUI_SelectFont(int FontType);
		static bool GUI_DrawText(int x, int y, const char *str,int lenpixels,int align,int font,int _inv);
		static void GUI_ClearFillRectangle(int x,int y,int x2,int y2,int action);
		static void GUI_InvertPixels(int pixelState);

	protected:

	private:
		static int  CurrentFont;
		static int  CurrentFontHeight;
		static int  InvertedShow;

		static const SBmp * (*getbmpfun) (char _ltr);

		static int GUI_DrawFont(int x, int y, char ch,int _InvertedShow);
		static int GUI_Draw1pxlNullLine(int x, int y, int height,int _InvertedShow);
		static int GUI_GetStrPixelsLen(const char *str,int strlen_ch);
};

class TGuiObjects : public TGui
{
  public:
  	static void GUIObj_Init();
  	
  	static void GUIObj_Invalidate();
  	
  	static void GUIDesktopShowNone();
  	static void GUIDesktopShowWait();
  	static void GUIDesktopShowError();
  	static void GUIDesktopShowCharge();
  	static void GUIDesktopShowFirmwareUpdate();
  	static void GUIDesktopShowProgressBar();
  	static void GUIDesktopShowTime();
  	static void GUIMessageShow(char *msg,int move_after);
		static void GUITestWindowShow();
		static void GUIShowECG();
		static void GUIHideECG();


  	static void GUIShowonTop(int mode);
  	static void GUIShowBatteryFail();
		static void GUIHideBatteryFail();
  	
  	static inline int  GUIObj_GetMode() {return GUIMode;}
  	
  	static volatile int  GUIMode;

  	static void ToolbarSet(int flags,bool Set);
  	static void GUIUpdateTempNBatt();
//		static int MessageBox(uint16_t error,const char *message, bool showselect, bool showchange, const char *select, const char *change, uint32_t timeout);
		static int MessageBox(const char *message, bool showmenu, const char *menutext1, const char *menutext2, uint32_t timeout);
		static int MenuBox(const char *message, const char *menutext, uint32_t timeout);
		static char *messagePtr;
		static void goButtonReleaseEvent();
		static void goButtonLongPushEvent();
		
		static void menuSetDefaults();
		static void menuSetItem(int item,char *str);
  	static int MenuSel;
  	static int menuMax;
  	static int remainTime;
  	static bool decDigits;
  	
  	static int TestWInt0;
  	static int TestWInt1;
  	static int TestWInt2;
  	static int TestWInt3;
  	
  	static int UploadPercent;
  	
  	static bool MeasureNewStyle;
  	static int  MNSFixTime;
  	static bool MeasureHeartBeatEvent;
  	static bool HoldDig;
  	static bool HoldOneUpdate;
  	
  	static bool ECG_Enstead_Time;
//  	static int  SampleToShow;
  	static int  SelectECGChannelToShow;
  	static void AddECGArrayToScreen(EcgData_t *Data);
  	static int GetDataForScreen();
  	static int GetDataForScreen(uint8_t n);
  	
  	static void ShowHR(int hr);
  	static void HideHeart();
  	static void HideHeartAndHR();
    
    static volatile bool EcgOnScreen;
    static void ShowToolbar();
    static void DrawTime(uint16_t x, uint16_t y, uint16_t lenpixels, uint8_t align);
    static void DrawDate(uint16_t x, uint16_t y, uint16_t lenpixels, uint8_t align);
    
    static bool StandbyLedEnable;

    
    static bool ButtonModeStart;
    
    static bool ScreenOrientation;
    static int ToolbarOffset;
	  static xTaskHandle xGUIObj;
	  static xTaskHandle xGUIScrOff;

		static int  Timeout;
    
  protected:
  private:
//	  static xTaskHandle xGUIObj;
    static void GO_TASK_GUIObj(void *pvParameters);	
    
    static void GO_TASK_ScrOff(void *pvParameters);	

    static void ShowBattery(void);

    
    static float t_k;
    static float b_k;
  	static int  ToolbarsFlags;
  	static bool  ToolbarsFlagsShow;
  	static int CountDown;
  	static int HiddenWindow;
  	
  	static int MenuUnderWindow;
  	static const char *menuItem0Ptr;
  	static const char *menuItem1Ptr;
  	static const char *menuItem2Ptr;
  	static const char *menuItem3Ptr;
    
  	static bool WaitingECGStartRecord;
  	
};
//------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------
