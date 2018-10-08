/*
    Title:
    Author:   VN Asliuk(Cardian)
    Date:     24.08.2010 16:37:01
    Purpose:
    Software:
    Hardware:
    Note:
*/
//------------------------------------------------------------------------------------
#include "TLMX9838.h"
#include "TAppProcessor.h"
//#include "Pinout.h"
#include "THardware.h"
#include "TLcdTrace.h"
#include "TGuiObjectsV2.h"
#include "TFlash.h"

//#ifdef EV_BOARD
#define TW01_NAME "Cardian KR01-SNXXXX-XXXX"    /* имя прибора по блютузу */
//#else
//  #define TW01_NAME "KP01_TST_OLED"    /* имя прибора по блютузу */
//#endif
//------------------------------------------------------------------------------------
bool TLMX9838::CFM_wait = false;
uint8_t TLMX9838::EventFilter = 0xFF;
uint8_t TLMX9838::SecurityMode = 0xFF;
bool TLMX9838::LMX9838_Connected = false;
bool TLMX9838::LMX9838_Ready = false;
uint8_t *TLMX9838::DataPtr = NULL;
bool TLMX9838::BT_On = false;
uint8_t TLMX9838::LMX_BT_Addr[ 6 ];

uint8_t TLMX9838::BDAddress[6] = {0x00,0x0b,0x0d,0x0c,0x63,0xaa};
uint8_t TLMX9838::RemoteBTPort = 0x01;
//------------------------------------------------------------------------------------

void TLMX9838::BluetoothPHYInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  LMX_GPIO_RCC_ENABLE;
  LMX_USART_RCC_ENABLE;

  /* TX pin */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = LMX_TXD_GPIO_PIN;
  GPIO_PinAFConfig(LMX_TXD_GPIO_PORT, LMX_TXD_GPIO_PINSOURCE, LMX_USART_AF);
  GPIO_Init( LMX_TXD_GPIO_PORT, &GPIO_InitStructure );

  /* RX pin */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin = LMX_RXD_GPIO_PIN;
        GPIO_PinAFConfig(LMX_RXD_GPIO_PORT, LMX_RXD_GPIO_PINSOURCE, LMX_USART_AF);
  GPIO_Init( LMX_RXD_GPIO_PORT, &GPIO_InitStructure );

  /* EN pin */
  GPIO_SetBits( LMX_EN_GPIO_PORT, LMX_EN_GPIO_PIN );
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Pin = LMX_EN_GPIO_PIN;
  GPIO_Init( LMX_EN_GPIO_PORT, &GPIO_InitStructure );

  /* RST pin */
  GPIO_SetBits( LMX_RST_GPIO_PORT, LMX_RST_GPIO_PIN );
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Pin = LMX_RST_GPIO_PIN;
  GPIO_Init( LMX_RST_GPIO_PORT, &GPIO_InitStructure );

  /* LSTAT pin */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Pin = LMX_LSTAT_GPIO_PIN;
  GPIO_Init( LMX_LSTAT_GPIO_PORT, &GPIO_InitStructure );

#ifndef BOOTLOADER
  EXTI_InitTypeDef EXTI_InitStructure;
  /* configuring LSTAT ext interrupt */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_EXTILineConfig( LMX_LSTAT_EXTI_PORT, LMX_LSTAT_EXTI_PIN);

  /* Configure LSTAT EXTI line */
  EXTI_ClearITPendingBit( LMX_LSTAT_EXTI_LINE );       /* Clear the Key Button EXTI line pending bit */
  EXTI_InitStructure.EXTI_Line = LMX_LSTAT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init( &EXTI_InitStructure );

  /* Enable and set Button EXTI Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = LMX_LSTAT_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 12;                    //14
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( &NVIC_InitStructure );
#endif /* BOOTLOADER */

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

  /* Enable the USART2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = LMX_USART_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( &NVIC_InitStructure );

  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = 921600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init( LMX_USART, &USART_InitStructure );

  USART_ITConfig( LMX_USART, USART_IT_RXNE, ENABLE );

  /* Enable USART */
  USART_Cmd( LMX_USART, ENABLE );

}

void TLMX9838::BluetoothPHYDeinit()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* TX pin */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Pin = LMX_TXD_GPIO_PIN;
  GPIO_Init( LMX_TXD_GPIO_PORT, &GPIO_InitStructure );

  /* RX pin */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Pin = LMX_RXD_GPIO_PIN;
  GPIO_Init( LMX_RXD_GPIO_PORT, &GPIO_InitStructure );

//  /* EN pin */
//  GPIO_SetBits( LMX_EN_GPIO_PORT, LMX_EN_GPIO_PIN );
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_InitStructure.GPIO_Pin = LMX_EN_GPIO_PIN;
//  GPIO_Init( LMX_EN_GPIO_PORT, &GPIO_InitStructure );

  /* RST pin */
  GPIO_SetBits( LMX_RST_GPIO_PORT, LMX_RST_GPIO_PIN );
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Pin = LMX_RST_GPIO_PIN;
  GPIO_Init( LMX_RST_GPIO_PORT, &GPIO_InitStructure );

  /* LSTAT pin */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Pin = LMX_LSTAT_GPIO_PIN;
  GPIO_Init( LMX_LSTAT_GPIO_PORT, &GPIO_InitStructure );

  EXTI_InitStructure.EXTI_Line = LMX_LSTAT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init( &EXTI_InitStructure );

  NVIC_InitStructure.NVIC_IRQChannel = LMX_USART_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init( &NVIC_InitStructure );

  USART_Cmd( LMX_USART, DISABLE );

  USART_ITConfig( LMX_USART, USART_IT_RXNE, DISABLE );

  LMX_USART_RCC_DISABLE;

}

//------------------------------------------------------------------------------------

//void TLMX9838::LMX9838EXTIInit()
//{
//  EXTI_InitTypeDef EXTI_InitStructure;
//#ifdef EV_BOARD
//  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init( &EXTI_InitStructure );
//#else
//  // Configure Button EXTI line
//  EXTI_InitStructure.EXTI_Line = EXTI_Line3;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//  EXTI_Init( &EXTI_InitStructure );
//#endif
//}
//------------------------------------------------------------------------------------
//
//void TLMX9838::LMX9838EXTIDeInit()
//{
//  EXTI_InitTypeDef EXTI_InitStructure;
//  #ifdef EV_BOARD
//  EXTI_InitStructure.EXTI_Line = EXTI_Line6;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
//  EXTI_Init(&EXTI_InitStructure);
//  #else
//  // Configure Button EXTI line
//  EXTI_InitStructure.EXTI_Line = EXTI_Line3;
//  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
//  EXTI_Init( &EXTI_InitStructure );
//  #endif
//}
//------------------------------------------------------------------------------------

void TLMX9838::BluetoothInit()
{
//  BluetoothPHYInit();
  TurnOnBluetooth();

  TLcdTrace::AddLine(1,"Bluetooth Setup.");

  LMX9838_ReadEventFilter();
  if(!LMX9838_WaitForCFM())
    {
//      TLcdTrace::AddLine("Set ReportAllEvents");
      LMX9838_ReportAllEvents();
    }
  if(EventFilter != 0)
    {
      TLcdTrace::AddLine("Set ReportAllEvents");
      LMX9838_ReportAllEvents();
    }

  LMX9838_ReadDeviceName();
  LMX9838_WaitForCFM();
   vTaskDelay(50/portTICK_RATE_MS);
  stufSerialNumber *tw01_serialname;

   tw01_serialname = (stufSerialNumber*)TFlash::SF_GetDataPtr(SERIAL_NUM);

  if(tw01_serialname->DeviceName[0] == 'C')
  {
    if(strcmp((char const*)tw01_serialname->DeviceName,(char const*)DataPtr) != 0)
      {
          LMX9838_SetDeviceName((char const*)tw01_serialname->DeviceName);
      }
  }

  LMX9838_ReadSecurityMode();
  LMX9838_WaitForCFM();
   vTaskDelay(50/portTICK_RATE_MS);
  if(SecurityMode != 0x1)
    {
//      #ifdef LCD_TRACE
      TLcdTrace::AddLine("Set Security mode 1");
//      #endif
      LMX9838_SetSecurityMode1();
    }

  TFrame::StartUpBluetooth = true;
}
//------------------------------------------------------------------------------------

void TLMX9838::TurnOnBluetooth()  //Calling from Tasks only
{
  if(BT_On) return;

  BT_On = true;

  GPIO_ResetBits( LMX_EN_GPIO_PORT, LMX_EN_GPIO_PIN );    /* активировать блютуз */

#ifndef BOOTLOADER
  vTaskDelay(1000/portTICK_RATE_MS);
#else
  __delay_cycles( 2500000 );
#endif
//  #ifdef LCD_TRACE
//    TLcdTrace::AddLine("Turn on LMX9838..Ok");
//  #endif
  //Reset module
   LMX9838_Reset();

//  #ifdef LCD_TRACE
//    TLcdTrace::AddLine("Reset LMX9838");
//    TLcdTrace::AddLine("Wait for ready...");
//  #endif

    LMX9838_Ready = true;

//  #ifdef LCD_TRACE
//    if(LMX9838_Ready)
//      {
//        TLcdTrace::AddLine("READY");
//         #ifndef IAP_CFG
//          vTaskResume(TFrame::xhCmdProcessor);
//        #endif
//      }
//    else
//      {
//        TLcdTrace::AddLine("FAIL");
//        BT_On = false;
//      }
//  #endif

  #ifndef IAP_CFG
    TGuiObjects::ToolbarSet(TBFM_BLUETOOTH,BT_On);
  #endif

}
//------------------------------------------------------------------------------------

void TLMX9838::TurnOffBluetooth()
{
  if(!BT_On) return;

   BT_On = false;

   #ifndef IAP_CFG
    vTaskDelay(50/portTICK_RATE_MS);
  #endif

  BluetoothPHYDeinit();
  GPIO_SetBits( LMX_EN_GPIO_PORT, LMX_EN_GPIO_PIN );    /* деактивировать блютуз */

//#ifdef IAP_CFG
//  IAP_Delay();
//#else
  vTaskDelay(100/portTICK_RATE_MS);
//  #endif

//  #ifdef LCD_TRACE
//    TLcdTrace::AddLine("BT Turn Off");
//  #endif
   #ifndef IAP_CFG
    TGuiObjects::ToolbarSet(TBFM_BLUETOOTH|TBFM_BLUETOOTHONAIR,0);
  #endif
}
//------------------------------------------------------------------------------------

void TLMX9838::LMX9838_ReportAllEvents()
{
  SendByte(STX_BT);

  SendByte(0x52);
  SendByte(0x4E);

  SendByte(0x1);
  SendByte(0);

  SendByte(0xA1);
  SendByte(0x00);

  SendByte(ETX_BT);
}
//------------------------------------------------------------------------------------

void TLMX9838::LMX9838_ReadDeviceName()
{
  SendByte(STX_BT);

  SendByte(0x52);
  SendByte(0x03);

  SendByte(0);
  SendByte(0);

  SendByte(0x55);

  SendByte(ETX_BT);
}
//------------------------------------------------------------------------------------

void TLMX9838::LMX9838_SetDeviceName(const char* DevN)
{
  int DevNLen = strlen(DevN);

  if(DevNLen > 38) return;

  SendByte(STX_BT);

  SendByte(0x52);
  SendByte(0x04);

  SendByte((uint8_t)(DevNLen + 2));
  SendByte(0);

  CharInt crcBCC;
  crcBCC.Int = 0x52 + 0x04 + (DevNLen + 2);
  SendByte(crcBCC.Char[0]);

  SendByte((uint8_t)(DevNLen + 1));
  for(int u = 0;u <= DevNLen;u++)
  {
    SendByte((uint8_t)DevN[u]);
  }

  SendByte(ETX_BT);
}
//------------------------------------------------------------------------------------

void TLMX9838::LMX9838_ReadDeviceAddress()
{
  SendByte(STX_BT);

  SendByte(0x52);
  SendByte(0x05);

  SendByte(0);
  SendByte(0);

  SendByte(0x57);

  SendByte(ETX_BT);
}
//------------------------------------------------------------------------------------

void TLMX9838::LMX9838_ChangeDeviceAddress(uint8_t *addr)
{
  SendByte(STX_BT);

  SendByte(0x52);
  SendByte(0x27);

  SendByte(6);
  SendByte(0);

  for(int u = 0;u < 6;u++)
  {
    SendByte(addr[u]);
  }

  SendByte(ETX_BT);
}
//------------------------------------------------------------------------------------

void TLMX9838::LMX9838_ReadEventFilter()
{
  SendByte(STX_BT);

  SendByte(0x52);
  SendByte(0x4F);

  SendByte(0);
  SendByte(0);

  SendByte(0xA1);

  SendByte(ETX_BT);
}
//------------------------------------------------------------------------------------

void TLMX9838::LMX9838_ReadSecurityMode()
{
  SendByte(STX_BT);

  SendByte(0x52);
  SendByte(0x18);

  SendByte(0);
  SendByte(0);

  SendByte(0x6A);

  SendByte(ETX_BT);
}
//------------------------------------------------------------------------------------

void TLMX9838::LMX9838_SetSecurityMode1()
{
  SendByte(STX_BT);

  SendByte(0x52);
  SendByte(0x19);

  SendByte(0x01);
  SendByte(0);

  SendByte(0x6C);

  SendByte(0x01);

  SendByte(ETX_BT);
}
//------------------------------------------------------------------------------------

bool TLMX9838::LMX9838_WaitForCFM()
{
  CFM_wait = false;
  for(int i = 0;i < 100;i++)
  {
    if(CFM_wait == true)
      {
        CFM_wait = false;
        return true;
      }
     vTaskDelay(10/portTICK_RATE_MS);
  }
//  #ifdef LCD_TRACE
//    TLcdTrace::AddLine("CFM Failed!");
//  #endif
  return false;
}
//------------------------------------------------------------------------------------

void TLMX9838::LMX9838_ParceEvents(uint8_t pckt,uint8_t id,uint8_t *_gdata)
{
  #ifdef LCD_TRACE
//    char str[24];
//    char str2[8];
//    char numAddr[8];
//    char btAddr[32];
  #endif
  if(pckt == 0x69)
    {
      switch(id)
      {
        case 0x50:
//          #ifdef LCD_TRACE
//            TLcdTrace::AddLine("CMD Link Established");
//            {
//              itos(_gdata[8],numAddr,2,HEXADECIMAL);
//              strcpy(btAddr,numAddr);
//              itos(_gdata[7],numAddr,2,HEXADECIMAL);
//              strcat(btAddr,numAddr);
//              itos(_gdata[6],numAddr,2,HEXADECIMAL);
//              strcat(btAddr,numAddr);
//              itos(_gdata[5],numAddr,2,HEXADECIMAL);
//              strcat(btAddr,numAddr);
//              itos(_gdata[4],numAddr,2,HEXADECIMAL);
//              strcat(btAddr,numAddr);
//              itos(_gdata[3],numAddr,2,HEXADECIMAL);
//              strcat(btAddr,numAddr);
//              TLcdTrace::AddLine(btAddr);
//            }
//          #endif
          break;
        case 0x0E:
//          #ifdef LCD_TRACE
//            TLcdTrace::AddLine("CMD Link Released");
//          #endif
          break;
      }
    }
  if(pckt == 0x43)
    {
      CFM_wait = true;
      switch(id)
      {
        case 0x03: //read local name
          DataPtr = &_gdata[5];
          #ifdef LCD_TRACE
            TLcdTrace::AddLine((char const*)&_gdata[5]);
          #endif
          break;
        case 0x05: //read local address
          DataPtr = &_gdata[4];
          for(int s = 0;s < 6;s++)
          {
            LMX_BT_Addr[s] = DataPtr[4+s];
          }
//          #ifdef LCD_TRACE
//            itos(_gdata[9],numAddr,2,HEXADECIMAL);
//            strcpy(btAddr,numAddr);
//            itos(_gdata[8],numAddr,2,HEXADECIMAL);
//            strcat(btAddr,numAddr);
//            itos(_gdata[7],numAddr,2,HEXADECIMAL);
//            strcat(btAddr,numAddr);
//            itos(_gdata[6],numAddr,2,HEXADECIMAL);
//            strcat(btAddr,numAddr);
//            itos(_gdata[5],numAddr,2,HEXADECIMAL);
//            strcat(btAddr,numAddr);
//            itos(_gdata[4],numAddr,2,HEXADECIMAL);
//            strcat(btAddr,numAddr);
//            TLcdTrace::AddLine(btAddr);
//          #endif
          break;
        case 0x4F: //read event filter
          DataPtr = &_gdata[3];
//          #ifdef LCD_TRACE
//            {
//              strcpy(str,"CMD Event filter:");
//              EventFilter = _gdata[3];
//              itos(_gdata[3],str2,2,HEXADECIMAL);
//              strcat(str,str2);
//
//              TLcdTrace::AddLine(str);
//            }
//          #endif
          break;
        case 0x18: //read event filter
          DataPtr = &_gdata[4];
//          #ifdef LCD_TRACE
//            {
//              strcpy(str,"CMD Security mode:");
//              SecurityMode = _gdata[4];
//              itos(_gdata[4],str2,2,HEXADECIMAL);
//              strcat(str,str2);
//
//              TLcdTrace::AddLine(str);
//            }
//          #endif
          break;
      }
  }
}
//------------------------------------------------------------------------------------

void TLMX9838::LMX9838_Reset()
{
  GPIO_ResetBits( LMX_RST_GPIO_PORT, LMX_RST_GPIO_PIN );
#ifndef BOOTLOADER
  vTaskDelay(100/portTICK_RATE_MS);
  GPIO_SetBits( LMX_RST_GPIO_PORT, LMX_RST_GPIO_PIN );
  vTaskDelay(300/portTICK_RATE_MS);
#else
  __delay_cycles( 2500000 );
  GPIO_SetBits( LMX_RST_GPIO_PORT, LMX_RST_GPIO_PIN );
  __delay_cycles( 7500000 );
#endif
}
//------------------------------------------------------------------------------------

void TLMX9838::SendByte( uint8_t data )
{
  if( !BT_On ) return;

  LMX_USART->DR = ((uint16_t) data);

  //  USART_SendData( LMX_USART, data) ;
  while(USART_GetFlagStatus( LMX_USART, USART_FLAG_TXE ) == RESET ) ;//taskYIELD(); //1093
//  while(USART_GetFlagStatus( LMX_USART, USART_FLAG_TC ) == RESET ) ;//taskYIELD(); //1093

  __delay_cycles( 250 );

}

void TLMX9838::SendByte( uint8_t data, bool pause )
{
  if( !BT_On ) return;

  LMX_USART->DR = ((uint16_t) data);

  //  USART_SendData( LMX_USART, data) ;
  while(USART_GetFlagStatus( LMX_USART, USART_FLAG_TXE ) == RESET ) ;//taskYIELD(); //1093
//  while(USART_GetFlagStatus( LMX_USART, USART_FLAG_TC ) == RESET ) ;//taskYIELD(); //1093

  if(pause) __delay_cycles( 250 );
  //else __delay_cycles( 250 );

}
//------------------------------------------------------------------------------------

bool TLMX9838::ReceiveByte( uint8_t *bt )
{
  if( USART_GetFlagStatus( LMX_USART, USART_FLAG_RXNE ) == RESET )
    *bt = USART_ReceiveData( LMX_USART );
  else return false;

  return true;
}
//------------------------------------------------------------------------------------
