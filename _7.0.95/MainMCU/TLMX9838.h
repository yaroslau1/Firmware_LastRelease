/*
    Title:
    Author:   VN Asliuk(Cardian)
    Date:     24.08.2010 16:37:32
    Purpose:
    Software:
    Hardware:
    Note:
*/
//------------------------------------------------------------------------------------
#ifndef TLMX9838_H
#define TLMX9838_H
//------------------------------------------------------------------------------------
//#include "TDevice.h"
#include "Utilities.h"
//#include "TLcdTrace.h"
#include "main.h"
//------------------------------------------------------------------------------------
#define STX_BT 0x02
#define ETX_BT 0x03
#define CFM_BT 0x03
//------------------------------------------------------------------------------------

class TLMX9838// : public TDevice
{
  public:
  	static void BluetoothInit();
  	static void TurnOnBluetooth();
  	static void TurnOffBluetooth();

  	static void SendByte(uint8_t data, bool pause);
  	static void SendByte(uint8_t data);
	static bool ReceiveByte(uint8_t *bt);

  	static void LMX9838_ReadDeviceName();
  	static void LMX9838_SetDeviceName(const char* DevN);
  	static void LMX9838_ReadDeviceAddress();
  	static void LMX9838_ReportAllEvents();
  	static void LMX9838_ReadEventFilter();
  	static void LMX9838_ReadSecurityMode();
  	static void LMX9838_SetSecurityMode1();
	static void LMX9838_ChangeDeviceAddress(uint8_t *addr);

  	static void LMX9838_Reset();

  	static bool LMX9838_WaitForCFM();

  	static void LMX9838_ParceEvents(uint8_t pckt,uint8_t id,uint8_t *_gdata);

  	static bool LMX9838_Connected;
  	static bool LMX9838_Ready;

  	static inline bool LMX9838_GetOnOffState() {return BT_On;}

  	static void BluetoothPHYInit();
	static void BluetoothPHYDeinit();
//		static void LMX9838EXTIInit();
//		static void LMX9838EXTIDeInit();
  	static bool BT_On;

	static uint8_t BDAddress[6];
	static uint8_t RemoteBTPort;
	
  protected:
  	static bool CFM_wait;
  private:

	static uint8_t EventFilter;
	static uint8_t SecurityMode;
	static uint8_t *DataPtr;
	static uint8_t LMX_BT_Addr[6];

};
//------------------------------------------------------------------------------------
#endif
