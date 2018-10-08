/************************************************************************************/
/* @title    ADS1298 control                            */
/* @author    Cyber Genius                            */
/* @version    0.1                                  */
/* @date    13.07.2012                              */
/************************************************************************************/
/* @brief                                      */
/************************************************************************************/
#include "TECG.h"
#include "TADS1292.h"
#include "TADS1298.h"
#include "main.h"
#include "TLed.h"
#include "TInterpreter.h"

//EcgData_t TECG::EcgData;
int16_t TECG::EcgData[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int16_t TECG::DiffFltData[8] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t TECG::ADSData[33];
uint8_t TECG::ChannelCount = 0;
uint32_t TECG::SynthChannel;
bool TECG::R = false;

uint8_t TECG::DMAState = 0;
DMA_InitTypeDef TECG::DMA_InitStructure;

volatile uint8_t Zero = 0;

int convecgdata = 0;

Hartbeat_TypeDef TECG::Hartbeat;

int32_t TECG::rX_i_1 = 0;
int32_t TECG::rY_i = 0;
int32_t TECG::rwData = 0;

int32_t TECG::data;
int32_t TECG::rOnStatus = 0;
int32_t TECG::rOffStatus = 0;
int32_t TECG::rMaxVal = 0;
uint32_t TECG::rOnOffPrd = 0;

bool TECG::StartMeasurment = false;
bool TECG::RespOn = false;
bool TECG::ECGOn = false;

uint32_t TECG::SampleCounter = 0;

/*----------------------------------------------------------------------------------*/
/* @brief  заполняет буффер как результат одного преобразования ADS1298      */
/* @param  None                                  */
/* @return  None                                  */
/*----------------------------------------------------------------------------------*/
void TECG::GetConvertResult( void )
{
  DMAState = 1;
  
  DMA_DeInit(DMA1_Channel4);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADSData[24];
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 9;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_Init(DMA1_Channel4, &DMA_InitStructure);

  DMA_DeInit(DMA1_Channel5);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Zero;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  
  SPI_SSControl( CS_ADS1292 );

  DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);

  EraseFIFO();

  DMA_Cmd(DMA1_Channel4, ENABLE);
  DMA_Cmd(DMA1_Channel5, ENABLE);
 
/*
  GPIO_ResetBits( ADS1298_CS_PORT, ADS1298_CS_PIN );
//  uint8_t tmp = SPI_ReceiveByte( 0x00 );
//  tmp = SPI_ReceiveByte( 0x00 );
//  tmp = SPI_ReceiveByte( 0x00 );

  EcgData.ECGStat[2] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGStat[1] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGStat[0] = SPI_ReceiveByte( 0x00 );

  EcgData.ECGCh1[2] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh1[1] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh1[0] = SPI_ReceiveByte( 0x00 );

  EcgData.ECGCh2[2] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh2[1] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh2[0] = SPI_ReceiveByte( 0x00 );

  EcgData.ECGCh3[2] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh3[1] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh3[0] = SPI_ReceiveByte( 0x00 );

  EcgData.ECGCh4[2] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh4[1] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh4[0] = SPI_ReceiveByte( 0x00 );

  EcgData.ECGCh5[2] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh5[1] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh5[0] = SPI_ReceiveByte( 0x00 );

  EcgData.ECGCh6[2] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh6[1] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh6[0] = SPI_ReceiveByte( 0x00 );

  EcgData.ECGCh7[2] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh7[1] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh7[0] = SPI_ReceiveByte( 0x00 );

  EcgData.ECGCh8[2] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh8[1] = SPI_ReceiveByte( 0x00 );
  EcgData.ECGCh8[0] = SPI_ReceiveByte( 0x00 );

  GPIO_SetBits( ADS1298_CS_PORT, ADS1298_CS_PIN );

  GPIO_ResetBits( ADS1292_CS_PORT, ADS1292_CS_PIN );

  EcgData.RespStat[2] = SPI_ReceiveByte( 0x00 );
  EcgData.RespStat[1] = SPI_ReceiveByte( 0x00 );
  EcgData.RespStat[0] = SPI_ReceiveByte( 0x00 );

  EcgData.RespCh1[2] = SPI_ReceiveByte( 0x00 );
  EcgData.RespCh1[1] = SPI_ReceiveByte( 0x00 );
  EcgData.RespCh1[0] = SPI_ReceiveByte( 0x00 );

  EcgData.RespCh2[2] = SPI_ReceiveByte( 0x00 );
  EcgData.RespCh2[1] = SPI_ReceiveByte( 0x00 );
  EcgData.RespCh2[0] = SPI_ReceiveByte( 0x00 );

  GPIO_SetBits( ADS1292_CS_PORT, ADS1292_CS_PIN );
*/

}

void TECG::DMA_IRQHandler()
{
  DMA_Cmd(DMA1_Channel4, DISABLE);
  DMA_Cmd(DMA1_Channel5, DISABLE);
   SPI_SSControl( CS_NONE );

  if (DMAState == 1)
  {
    DMAState = 2;

    DMA_DeInit(DMA1_Channel4);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADSData[0];
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 27;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    DMA_DeInit(DMA1_Channel5);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&Zero;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  
    SPI_SSControl( CS_ADS1298 );

    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);

    EraseFIFO();

    DMA_Cmd(DMA1_Channel4, ENABLE);
    DMA_Cmd(DMA1_Channel5, ENABLE);
  }
  else if (DMAState == 2)
  {
    DMAState = 0;
#ifdef DEMO_VERSION
    const int8_t DemoECG[] = {0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x1C, 0xE8, 	0x00, 0x19, 0xC8, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0xC8, 	0x00, 0x1B, 0x58, 	0x00, 0x1C, 0x20, 	0x00, 0x1C, 0xE8, 	0x00, 0x1D, 0xB0, 	0x00, 0x1E, 0x78, 	0x00, 0x1E, 0x78, 	0x00, 0x1E, 0x78, 	0x00, 0x1D, 0xB0, 	0x00, 0x1D, 0xB0, 	0x00, 0x1C, 0xE8, 	0x00, 0x1C, 0x20, 	0x00, 0x1B, 0x58, 	0x00, 0x1A, 0x90, 	0x00, 0x19, 0xC8, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x18, 0x38, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x18, 0x38, 	0x00, 0x17, 0x70, 	0x00, 0x18, 0x38, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x18, 0x38, 	0x00, 0x1D, 0xB0, 	0x00, 0x1B, 0x58, 	0x00, 0x2A, 0x30, 	0x00, 0x3F, 0x48, 	0x00, 0x52, 0xD0, 	0x00, 0x68, 0xB0, 	0x00, 0x6F, 0xB8, 	0x00, 0x5E, 0x88, 	0x00, 0x45, 0x88, 	0x00, 0x2E, 0xE0, 	0x00, 0x1A, 0x90, 	0x00, 0x10, 0x68, 	0x00, 0x0A, 0xF0, 	0x00, 0x06, 0x40, 	0x00, 0x02, 0x58, 	0x00, 0x01, 0x90, 	0x00, 0x05, 0x78, 	0x00, 0x0B, 0xB8, 	0x00, 0x0F, 0xA0, 	0x00, 0x11, 0x30, 	0x00, 0x11, 0x30, 	0x00, 0x11, 0x30, 	0x00, 0x11, 0xF8, 	0x00, 0x11, 0xF8, 	0x00, 0x11, 0xF8, 	0x00, 0x11, 0xF8, 	0x00, 0x11, 0xF8, 	0x00, 0x12, 0xC0, 	0x00, 0x12, 0xC0, 	0x00, 0x12, 0xC0, 	0x00, 0x12, 0xC0, 	0x00, 0x12, 0xC0, 	0x00, 0x13, 0x88, 	0x00, 0x12, 0xC0, 	0x00, 0x13, 0x88, 	0x00, 0x13, 0x88, 	0x00, 0x13, 0x88, 	0x00, 0x13, 0x88, 	0x00, 0x14, 0x50, 	0x00, 0x14, 0x50, 	0x00, 0x14, 0x50, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0xE0, 	0x00, 0x15, 0xE0, 	0x00, 0x15, 0xE0, 	0x00, 0x15, 0xE0, 	0x00, 0x16, 0xA8, 	0x00, 0x15, 0xE0, 	0x00, 0x16, 0xA8, 	0x00, 0x16, 0xA8, 	0x00, 0x17, 0x70, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x1A, 0x90, 	0x00, 0x1A, 0x90, 	0x00, 0x1B, 0x58, 	0x00, 0x1B, 0x58, 	0x00, 0x1B, 0x58, 	0x00, 0x1C, 0x20, 	0x00, 0x1C, 0x20, 	0x00, 0x1C, 0xE8, 	0x00, 0x1C, 0xE8, 	0x00, 0x1E, 0x78, 	0x00, 0x1E, 0x78, 	0x00, 0x20, 0x08, 	0x00, 0x20, 0x08, 	0x00, 0x20, 0x08, 	0x00, 0x20, 0xD0, 	0x00, 0x20, 0xD0, 	0x00, 0x20, 0x08, 	0x00, 0x1F, 0x40, 	0x00, 0x1F, 0x40, 	0x00, 0x1E, 0x78, 	0x00, 0x1C, 0xE8, 	0x00, 0x1B, 0x58, 	0x00, 0x1A, 0x90, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x17, 0x70, 	0x00, 0x16, 0xA8, 	0x00, 0x15, 0xE0, 	0x00, 0x15, 0xE0, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0x18, 	0x00, 0x14, 0x50, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0x18, 	0x00, 0x15, 0xE0, 	0x00, 0x15, 0xE0, 	0x00, 0x15, 0xE0, 	0x00, 0x15, 0xE0, 	0x00, 0x16, 0xA8, 	0x00, 0x15, 0xE0, 	0x00, 0x15, 0xE0, 	0x00, 0x16, 0xA8, 	0x00, 0x16, 0xA8, 	0x00, 0x16, 0xA8, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x16, 0xA8, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x17, 0x70, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x18, 0x38, 	0x00, 0x19, 0x00, 	0x00, 0x18, 0x38, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00, 	0x00, 0x19, 0x00};
    static int demo_index = 0;
    ADSData[2] = ADSData[2] & 0xF0;
    ADSData[2] = ADSData[2] | 0x02;
    for (int i = 0; i < 8; i++)
    {
      ADSData[3 + 3 * i + 0] = DemoECG[demo_index + 0];
      ADSData[3 + 3 * i + 1] = DemoECG[demo_index + 1];
      ADSData[3 + 3 * i + 2] = DemoECG[demo_index + 2];
    }
    demo_index += 3;
    if (demo_index > (250 * 3 - 1)) demo_index = 0;
#endif
    uint8_t c_s = ADSData[2] & 0x03;
    if (c_s == 0) ChannelCount = 8;
    else if (c_s == 1) ChannelCount = 3;
    else if (c_s == 2) ChannelCount = 3;
    else ChannelCount = 0;
    for (int i = 0; i < ChannelCount; i++)
    {
      EcgData[i] = (((uint16_t)ADSData[3 * (i + 1) + 1]) |  (((uint16_t)ADSData[3 * (i + 1)]) << 8));
    } 
    DiffFlt();
    GetSynthChannel();
    FindR();

/*    
    ADSData[3 + 3 * 1 + 0] = (uint8_t)((SynthChannel >> 16) & 0x000000FF);
    ADSData[3 + 3 * 1 + 1] = (uint8_t)((SynthChannel >>  8) & 0x000000FF);
    ADSData[3 + 3 * 1 + 2] = (uint8_t)((SynthChannel      ) & 0x000000FF);

    ADSData[3 + 3 * 2 + 0] = (uint8_t)(DiffFltData[1] >> 8);
    ADSData[3 + 3 * 2 + 1] = (uint8_t)(DiffFltData[1] & 0x00FF);
    ADSData[3 + 3 * 2 + 2] = 0;
*/    
  }
  else
  {
    DMAState = 0;
  }
}

/*----------------------------------------------------------------------------*/
/* @brief   Hастраивает ADS1292 и ADS1298 на преобразование                   */
/* @param   None                                                              */
/* @return  None                                                              */
/*----------------------------------------------------------------------------*/
void TECG::StartConversation(void)
{
  // Start Off
  SPI_Start(0);

  // Stop Read Data Continuously mode
  SetOpcode(CS_ADS1298, ADS1298_OPCODE_SDATAC);
  SetOpcode(CS_ADS1292, ADS1292_OPCODE_SDATAC);

  // Low-power mode; Daisy-chain mode; Output data rate 250 SPS
  RegisterWrite(CS_ADS1298, ADS1298_REG_CONFIG1, 0x06);  // до 7.0.86 0x26

  // Test signal Not used
  RegisterWrite(CS_ADS1298, ADS1298_REG_CONFIG2, 0x02);

  // Enable internal reference buffer; VREFP is set to 2.4 V;
  // RLD measurement Open; RLDREF signal (AVDD – AVSS) / 2 generated internally;
  // RLD buffer is enabled; RLD sense is disabled; RLD is connected
  RegisterWrite(CS_ADS1298, ADS1298_REG_CONFIG3, 0xCC);

  // Route channels 1, 2, 3 positive signal into RLD derivation
  RegisterWrite(CS_ADS1298, ADS1298_REG_RLD_SENSP, 0x07);     // до v7.0.86 0x26

  // Route channels 1, 2, 3 negative signal into RLD derivation
  RegisterWrite(CS_ADS1298, ADS1298_REG_RLD_SENSN, 0x07);     // до v7.0.86 0x26

  // All Channels: Normal operation; PGA gain 6; Normal electrode input
  RegisterWrite(CS_ADS1298, ADS1298_REG_CH1SET, 0x00);
  RegisterWrite(CS_ADS1298, ADS1298_REG_CH2SET, 0x00);
  RegisterWrite(CS_ADS1298, ADS1298_REG_CH3SET, 0x00);
  RegisterWrite(CS_ADS1298, ADS1298_REG_CH4SET, 0x00);
  RegisterWrite(CS_ADS1298, ADS1298_REG_CH5SET, 0x00);
  RegisterWrite(CS_ADS1298, ADS1298_REG_CH6SET, 0x00);
  RegisterWrite(CS_ADS1298, ADS1298_REG_CH7SET, 0x00);
  RegisterWrite(CS_ADS1298, ADS1298_REG_CH8SET, 0x00);

  // GPIO 1, 2, 3 and 4: Input
  RegisterWrite(CS_ADS1298, ADS1298_REG_GPIO, 0xFF);

  // Pace even Channel 2; Pace odd Channel 3; Pace detect buffer turned on
  RegisterWrite(CS_ADS1298, ADS1298_REG_PACE, 0x03);

  // RESP demodulation circuitry turned off; 
  // RESP modulation circuitry turned off; No respiration
  RegisterWrite(CS_ADS1298, ADS1298_REG_RESP, 0x00);          // до v7.0.86 0xEE

  // Continuous conversion mode; WCT to RLD connection off; 
  //Lead-off comparators disabled
  RegisterWrite(CS_ADS1298, ADS1298_REG_CONFIG4, 0x00);       // до v7.0.86 0x20

  if(RespOn)
  {
  
    // Clock divider 16
    RegisterWrite(CS_ADS1292, ADS1292_REG_LOFF_STAT, 0x40);

    // Continuous conversion mode; Channel oversampling ratio 250 SPS
    RegisterWrite(CS_ADS1292, ADS1292_REG_CONFIG1, 0x01);

    // Lead-off comparators disabled; Reference buffer is enabled;
    // 2.42V reference; Oscillator clock output disabled; Test signal Off
    RegisterWrite(CS_ADS1292, ADS1292_REG_CONFIG2, 0xA0);

    // External RLD
    RegisterWrite(CS_ADS1292, ADS1292_REG_RLD_SENS, 0x00);

    // All Channels: Normal operation; PGA gain 6; Normal electrode input
    RegisterWrite(CS_ADS1292, ADS1292_REG_CH1SET, 0x06);
    RegisterWrite(CS_ADS1292, ADS1292_REG_CH2SET, 0x06);

    // RESP demodulation circuitry turned on; RESP modulation circuitry turned on;
    // Respiration phase 112.5°;
    // Respiration control Internal respiration with internal clock
    RegisterWrite(CS_ADS1292, ADS1292_REG_RESP1, 0xEA);

    // Calibration Off; Respiration control frequency 32 kHz;
    // RLDREF signal fed externally
    RegisterWrite(CS_ADS1292, ADS1292_REG_RESP2, 0x01);

    // GPIO 1 and 2: Input
    RegisterWrite(CS_ADS1292, ADS1292_REG_GPIO, 0x0F);

  }
  
  
  // Start On
  SPI_Start(1);

  // Enable Read Data Continuous mode
  SetOpcode(CS_ADS1292, ADS1292_OPCODE_RDATAC);
  SetOpcode(CS_ADS1298, ADS1298_OPCODE_RDATAC);

}


/*----------------------------------------------------------------------------------*/
/* @brief  Включает тестовые сигналы в виде меандра. Также необходимо запускать  */
/*      перед началом работы снятия экг, чтобы прошла внутреняя калибровка АЦП  */
/* @param  None                                  */
/* @return  None                                  */
/*----------------------------------------------------------------------------------*/
void TECG::TestSignals( void )
{

  SPI_Start( 0 );

  SetOpcode( CS_ADS1292, ADS1292_OPCODE_SDATAC );

  RegisterWrite( CS_ADS1292, ADS1292_REG_LOFF_STAT, 0x40 );  /* Clock divider is 16 */

  RegisterWrite( CS_ADS1292, ADS1292_REG_CONFIG1, 0x01 );  /* Continuous conversion mode, 250 sps */
  RegisterWrite( CS_ADS1292, ADS1292_REG_CONFIG2, 0xA3 );  /* Internal reference 2.42V, test signal 1Hz */

  RegisterWrite( CS_ADS1292, ADS1292_REG_RLD_SENS, 0x00 );  /* External RLD */

  RegisterWrite( CS_ADS1292, ADS1292_REG_CH1SET, 0x05 );  /* Channel ON, PGA = 6, Test signal */
  RegisterWrite( CS_ADS1292, ADS1292_REG_CH2SET, 0x05 );  /* Channel ON, PGA = 6, Test signal */

  RegisterWrite( CS_ADS1292, ADS1292_REG_RESP1, 0x02 );  /* Resp MOD and DEMOD off, Phase = 0 */
  RegisterWrite( CS_ADS1292, ADS1292_REG_RESP2, 0x01 );  /* Resp freq = 32kHz */

  RegisterWrite( CS_ADS1292, ADS1292_REG_GPIO, 0x0F );  /* all input */

  SetOpcode( CS_ADS1292, ADS1292_OPCODE_RDATAC );

  SetOpcode( CS_ADS1298, ADS1298_OPCODE_SDATAC );

  RegisterWrite( CS_ADS1298, ADS1298_REG_CONFIG1, 0x20 );     // до v7.0.86 0x26

  RegisterWrite( CS_ADS1298, ADS1298_REG_CONFIG3, 0xC0 );
  RegisterWrite( CS_ADS1298, ADS1298_REG_CONFIG2, 0x10 );

  RegisterWrite( CS_ADS1298, ADS1298_REG_CH2SET, 0x05 );
  RegisterWrite( CS_ADS1298, ADS1298_REG_CH3SET, 0x05 );
  RegisterWrite( CS_ADS1298, ADS1298_REG_CH6SET, 0x05 );

  RegisterWrite( CS_ADS1298, ADS1298_REG_CH1SET, 0x05 );    /* channels power down */
  RegisterWrite( CS_ADS1298, ADS1298_REG_CH4SET, 0x05 );
  RegisterWrite( CS_ADS1298, ADS1298_REG_CH5SET, 0x05 );
  RegisterWrite( CS_ADS1298, ADS1298_REG_CH7SET, 0x05 );
  RegisterWrite( CS_ADS1298, ADS1298_REG_CH8SET, 0x05 );

  RegisterWrite( CS_ADS1298, ADS1298_REG_PACE, 0x03 );    /* Enable hardware pace detect */

  SPI_Start( 1 );
  SetOpcode( CS_ADS1298, ADS1298_OPCODE_RDATAC );

}



/*----------------------------------------------------------------------------------*/
/* @brief  None                                  */
/* @param  None                                  */
/* @return  None                                  */
/*----------------------------------------------------------------------------------*/
void TECG::StopConversation( void )
{

  SPI_Start( 0 );

  SetOpcode( CS_ADS1292, ADS1292_OPCODE_SDATAC );

  RegisterWrite( CS_ADS1292, ADS1292_REG_LOFF_STAT, 0x40 );  /* Clock divider is 16 */

  RegisterWrite( CS_ADS1292, ADS1292_REG_CONFIG1, 0x81 );  /* Single-shot mode, 250 sps */
  RegisterWrite( CS_ADS1292, ADS1292_REG_CONFIG2, 0x80 );  /* Reference off, test signal off */

  RegisterWrite( CS_ADS1292, ADS1292_REG_RLD_SENS, 0x00 );  /* External RLD */

  RegisterWrite( CS_ADS1292, ADS1292_REG_CH1SET, 0x81 );  /* Channel off, PGA = 6, Input shorted */
  RegisterWrite( CS_ADS1292, ADS1292_REG_CH2SET, 0x81 );  /* Channel off, PGA = 6, Input shorted */

  RegisterWrite( CS_ADS1292, ADS1292_REG_RESP1, 0x02 );  /* Resp MOD and DEMOD off, Phase = 0 */
  RegisterWrite( CS_ADS1292, ADS1292_REG_RESP2, 0x01 );  /* Resp freq = 32kHz */

  RegisterWrite( CS_ADS1292, ADS1292_REG_GPIO, 0x0F );  /* all input */

  SetOpcode( CS_ADS1292, ADS1292_OPCODE_RDATAC );

  SetOpcode( CS_ADS1298, ADS1298_OPCODE_SDATAC );

  RegisterWrite( CS_ADS1298, ADS1298_REG_CONFIG1, 0x06 );  // до v7.0.86 0x20
  RegisterWrite( CS_ADS1298, ADS1298_REG_CONFIG2, 0x02 );  // до v7.0.86 0x00
  RegisterWrite( CS_ADS1298, ADS1298_REG_CONFIG3, 0x40 );  // до v7.0.86 0x00

  RegisterWrite( CS_ADS1298, ADS1298_REG_RLD_SENSP, 0x00 );
  RegisterWrite( CS_ADS1298, ADS1298_REG_RLD_SENSN, 0x00 );

  RegisterWrite( CS_ADS1298, ADS1298_REG_CH1SET, 0x81 );  // до v7.0.86 0x01
  RegisterWrite( CS_ADS1298, ADS1298_REG_CH2SET, 0x81 );  // до v7.0.86 0x01
  RegisterWrite( CS_ADS1298, ADS1298_REG_CH3SET, 0x81 );  // до v7.0.86 0x01
  RegisterWrite( CS_ADS1298, ADS1298_REG_CH4SET, 0x81 );  // до v7.0.86 0x01
  RegisterWrite( CS_ADS1298, ADS1298_REG_CH5SET, 0x81 );  // до v7.0.86 0x01
  RegisterWrite( CS_ADS1298, ADS1298_REG_CH6SET, 0x81 );  // до v7.0.86 0x01
  RegisterWrite( CS_ADS1298, ADS1298_REG_CH7SET, 0x81 );  // до v7.0.86 0x01
  RegisterWrite( CS_ADS1298, ADS1298_REG_CH8SET, 0x81 );  // до v7.0.86 0x01
}



/*----------------------------------------------------------------------------------*/
void TECG::SPI_Start( bool state )
{
  if( state == true ) GPIO_SetBits( ADS1298_START_PORT, ADS1298_START_PIN );
  else GPIO_ResetBits( ADS1298_START_PORT, ADS1298_START_PIN );
}



/*----------------------------------------------------------------------------------*/
void TECG::SPI_Reset( uint8_t state )
{
  GPIO_SetBits( ADS1298_RESET_PORT, ADS1298_RESET_PIN );
  GPIO_SetBits( ADS1292_RESET_PORT, ADS1292_RESET_PIN );
  switch(state)
  {
    case CS_ADS1298:
      GPIO_ResetBits( ADS1298_RESET_PORT, ADS1298_RESET_PIN ); break;
    case CS_ADS1292:
      GPIO_ResetBits( ADS1292_RESET_PORT, ADS1292_RESET_PIN ); break;
  }
}



/*----------------------------------------------------------------------------------*/
void TECG::SPI_SSControl( uint8_t state )
{
  GPIO_SetBits( ADS1298_CS_PORT, ADS1298_CS_PIN );
  GPIO_SetBits( ADS1292_CS_PORT, ADS1292_CS_PIN );
  GPIO_SetBits( EEPROM_CS_PORT, EEPROM_CS_PIN );
  switch(state)
  {
    case CS_ADS1298:
      GPIO_ResetBits( ADS1298_CS_PORT, ADS1298_CS_PIN ); break;
    case CS_ADS1292:
      GPIO_ResetBits( ADS1292_CS_PORT, ADS1292_CS_PIN ); break;
    case CS_EEPROM:
      GPIO_ResetBits( EEPROM_CS_PORT, EEPROM_CS_PIN ); break;
  }
}



/*----------------------------------------------------------------------------------*/
void TECG::SPI_SendByte( uint8_t byte )
{
  SPI_SendData8( ADS1298_SPI, byte );
  while( SPI_I2S_GetFlagStatus( ADS1298_SPI, SPI_I2S_FLAG_BSY ) == SET );  // while( SPI_I2S_GetFlagStatus( ADS1298_SPI, SPI_I2S_FLAG_TXE ) == RESET )
//  __delay_cycles(  1 );
}



/*----------------------------------------------------------------------------------*/
uint8_t TECG::SPI_ReceiveByte( uint8_t data )
{
  volatile uint8_t Data;
  while( SPI_GetReceptionFIFOStatus( ADS1298_SPI ) != SPI_ReceptionFIFOStatus_Empty)
  {
    Data = SPI_ReceiveData8( ADS1298_SPI );
  }
  while( SPI_GetTransmissionFIFOStatus( ADS1298_SPI ) != SPI_TransmissionFIFOStatus_Empty);
  SPI_SendData8( ADS1298_SPI, data );
  while( SPI_GetReceptionFIFOStatus( ADS1298_SPI ) != SPI_ReceptionFIFOStatus_Empty);
  Data = SPI_ReceiveData8( ADS1298_SPI );
  return Data;
}

/*----------------------------------------------------------------------------------*/
void TECG::EraseFIFO(  )
{
  volatile uint8_t Data;
  while( SPI_GetReceptionFIFOStatus( ADS1298_SPI ) != SPI_ReceptionFIFOStatus_Empty)
  {
    Data = SPI_ReceiveData8( ADS1298_SPI );
  }
}



/*----------------------------------------------------------------------------------*/
void TECG::Init( void )
{
  PHYInit();
  SPI_Start( 0 );

  SPI_Reset( CS_NONE );
  __delay_cycles( 3000 );
  SPI_Reset( CS_ADS1292 );
  __delay_cycles( 3000 );
  SPI_Reset( CS_NONE );
  __delay_cycles( 3000 );
  SPI_Reset( CS_ADS1298 );
  __delay_cycles( 3000 );
  SPI_Reset( CS_NONE );
  __delay_cycles( 1000000 );

}



/*----------------------------------------------------------------------------------*/
void TECG::SetOpcode( uint8_t cs, uint8_t opcode )
{
  SPI_SSControl( cs );
  SPI_SendByte( opcode );
  SPI_SSControl( CS_NONE );
}


/*----------------------------------------------------------------------------------*/
uint8_t TECG::RegisterRead( uint8_t cs, uint8_t Register )
{
  volatile uint8_t data;
  SPI_SSControl( cs );

  switch(cs)
  {
    case CS_ADS1298:
      SPI_SendByte( ADS1298_OPCODE_SDATAC );
      SPI_SendByte( ADS1298_OPCODE_RREG | Register );
      break;
    case CS_ADS1292:
      SPI_SendByte( ADS1292_OPCODE_SDATAC );
      SPI_SendByte( ADS1292_OPCODE_RREG | Register );
      break;
  }

  data = SPI_ReceiveByte( 0x00 );
  data = SPI_ReceiveByte( 0x00 );

  SPI_SSControl( CS_NONE );
  return data;
}



/*----------------------------------------------------------------------------------*/
uint8_t TECG::DataRead( uint8_t cs )
{
  volatile uint8_t data;
  SPI_SSControl( cs );

  data = SPI_ReceiveByte( 0x00 );

  SPI_SSControl( CS_NONE );
  return data;
}

/*----------------------------------------------------------------------------------*/
void TECG::RegisterWrite( uint8_t cs, uint8_t Register, uint8_t Data )
{
  SPI_SSControl( cs );

  switch(cs)
  {
    case CS_ADS1298:
      SPI_SendByte( ADS1298_OPCODE_WREG | Register );
      break;
    case CS_ADS1292:
      SPI_SendByte( ADS1292_OPCODE_WREG | Register );
      break;
  }
  
  SPI_SendByte( 0x00 );
  SPI_SendByte( Data );

  SPI_SSControl( CS_NONE );
}



/*----------------------------------------------------------------------------------*/
void TECG::PHYInit( void )
{
  ADS1298_GPIO_RCC_ENABLE;
  ADS1298_SPI_RCC_ENABLE;

  GPIO_InitTypeDef GPIO_InitStructure;

  // RESET pin
  GPIO_InitStructure.GPIO_Pin = ADS1298_RESET_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init( ADS1298_RESET_PORT, &GPIO_InitStructure );

  // START pin
  GPIO_InitStructure.GPIO_Pin = ADS1298_START_PIN;
  GPIO_Init( ADS1298_START_PORT, &GPIO_InitStructure );

  // SPI pins config
  GPIO_InitStructure.GPIO_Pin = ADS1298_SCK_PIN | ADS1298_MOSI_PIN | ADS1298_MISO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
  GPIO_PinAFConfig(ADS1298_SPI_PORT, ADS1298_SCK_PINSOURCE, ADS1298_SPI_AF);
  GPIO_PinAFConfig(ADS1298_SPI_PORT, ADS1298_MISO_PINSOURCE, ADS1298_SPI_AF);
  GPIO_PinAFConfig(ADS1298_SPI_PORT, ADS1298_MOSI_PINSOURCE, ADS1298_SPI_AF);
  GPIO_Init( ADS1298_SPI_PORT, &GPIO_InitStructure );

  // CS
  GPIO_SetBits( ADS1298_CS_PORT, ADS1298_CS_PIN );
  GPIO_InitStructure.GPIO_Pin = ADS1298_CS_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
  GPIO_Init( ADS1298_CS_PORT, &GPIO_InitStructure );

  ADS1292_GPIO_RCC_ENABLE;

// !!! Инициализация RESET увеличивает потребление на 10мА !!! 
  // RESET pin
  GPIO_SetBits( ADS1292_RESET_PORT, ADS1292_RESET_PIN );
  GPIO_InitStructure.GPIO_Pin = ADS1292_RESET_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
  GPIO_Init( ADS1292_RESET_PORT, &GPIO_InitStructure );

  // CS 
  GPIO_SetBits( ADS1292_CS_PORT, ADS1292_CS_PIN );
  GPIO_InitStructure.GPIO_Pin = ADS1292_CS_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
  GPIO_Init( ADS1292_CS_PORT, &GPIO_InitStructure );

  // SPI Setup
  SPI_InitTypeDef SPI_InitStructure;

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;    // SPI_BaudRatePrescaler_16
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init( ADS1298_SPI, &SPI_InitStructure );

  SPI_RxFIFOThresholdConfig(ADS1298_SPI, SPI_RxFIFOThreshold_QF);

  SPI_Cmd( ADS1298_SPI, ENABLE );

  //---- DRDY CONFIG -----------------------------------------------
  GPIO_InitStructure.GPIO_Pin = ADS1298_DRDY_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;
  GPIO_Init( ADS1298_DRDY_PORT, &GPIO_InitStructure );

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  SYSCFG_EXTILineConfig( ADS_DRDY_EXTI_PORT, ADS_DRDY_EXTI_PIN );

  EXTI_InitTypeDef EXTI_InitStructure;

  EXTI_InitStructure.EXTI_Line = ADS_DRDY_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init( &EXTI_InitStructure );

  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = ADS_DRDY_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init( &NVIC_InitStructure );

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  DMA_DeInit(DMA1_Channel4);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADS1298_SPI->DR;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADSData[0];
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 9;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel4, &DMA_InitStructure);

  DMA_DeInit(DMA1_Channel5);
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( &NVIC_InitStructure );

  // Enable SPI_MASTER DMA Tx request
  SPI_I2S_DMACmd(ADS1298_SPI, SPI_I2S_DMAReq_Tx, ENABLE);
  // Enable SPI_MASTER DMA Rx request
  SPI_I2S_DMACmd(ADS1298_SPI, SPI_I2S_DMAReq_Rx, ENABLE);

  /*
  DMA_Cmd(DMA1_Channel2, ENABLE);
  DMA_Cmd(DMA1_Channel3, ENABLE);

  while(!DMA_GetFlagStatus(DMA1_FLAG_TC2));
  while(!DMA_GetFlagStatus(DMA1_FLAG_TC3));

  DMA_Cmd(DMA1_Channel2, DISABLE);
  DMA_Cmd(DMA1_Channel3, DISABLE);
  */
}

/*----------------------------------------------------------------------------------*/
/* Фильтр ВЧ, нужен чтобы привести кардиограмму к нулю                */
/*----------------------------------------------------------------------------------*/
/* X_i - вх. сигнал, ch - канал, reset - сброс                    */
/*----------------------------------------------------------------------------------*/
int TECG::FHigh_0_32_Fixed( int X_i, int ch, bool Reset, bool ZeroAdduction )
{
  static int X_i_1[8], Y_i_1[8];
  long long Y_i;
  static long int koef;
  static int tempT;    /* переменная нужна для того, чтобы засечь перегрузку, чтобы настроить фильтр так, чтобы он быстрее приводил сигнал к 0 */
  static int State;

  if( Reset ){
    X_i_1[ch] = 0;
    Y_i_1[ch] = 0;
//    koef = 9996L;
    koef = 9991L;
    return 0;
  }

  int _X_i;

  /* Алгоритм быстрого приведения сигнала к нулю */
  if( ZeroAdduction == true ){
    switch( State ){
      case 0:
        if(( Y_i_1[0] > 25000 ) || ( Y_i_1[0] < -25000 ) ||
           ( Y_i_1[1] > 25000 ) || ( Y_i_1[1] < -25000 ) ||
           ( Y_i_1[2] > 25000 ) || ( Y_i_1[2] < -25000 ))
          tempT++;
        else tempT = 0;
        if( tempT == 20 ){
          koef = 9000L;
          State = 1;
        }
        break;

      case 1:
        tempT++;
        if( tempT == 400 ){
          State = 0;
          tempT = 0;
          koef = 9991L;
        }
        break;
    }
  }
  else {
//    koef = 9996L;
    koef = 9991L;
  }
  /* END BLOCK */

  _X_i = X_i;
  _X_i = X_i;

  Y_i = koef * ( Y_i_1[ch] + _X_i - X_i_1[ch] );    /* чем больше koef, тем точнее сигнал, но медленее выплывает и наоборот */
  Y_i_1[ch] = Y_i / 10000L;
  X_i_1[ch] = _X_i;
  return ( Y_i / 10000L );
}

/*----------------------------------------------------------------------------------*/
/* @brief  ЧСС ЭКГ Детектор                            */
/* @param  wData - входные данные                          */
/* @return  отфильтрованные данные                          */
/*----------------------------------------------------------------------------------*/
int TECG::hartbeat_detector( int16_t wData )
{
  rY_i =  (1500 * (rY_i + wData - rX_i_1));
  
  if(rY_i < 0)
  {
    rY_i = labs(rY_i);
    rY_i >>= 11;
    rY_i = -rY_i;
  }
    
  else rY_i >>= 11;

  rX_i_1 = wData;

  /* ограничитель */
  if(rY_i >  32766) rwData = 32766;
  else if(rY_i < -32766) rwData = -32766;
  else rwData = rY_i;
  /* ENDBLOCK */

  //return (rwData*2);

  return rwData;
}

/*----------------------------------------------------------------------------------*/
/* @brief  Функция измерения ЧСС                          */
/* @param  None                                  */
/* @return  None                                  */
/*----------------------------------------------------------------------------------*/
void TECG::HeartbeatCalculation( void )
{
  // 2-ое отведение
  
  convecgdata = (((uint32_t)TECG::ADSData[11])) |  (((uint32_t)TECG::ADSData[10])<<8) | (((uint32_t)TECG::ADSData[9])<<16);
  
  data = hartbeat_detector( (int16_t) convecgdata );

  if( data > rMaxVal ){
    rMaxVal = data;
  }

  int16_t Threshold;

  static uint8_t OffStat = 0;

  Threshold = (int16_t)(((int16_t)rMaxVal * 1) / 2);

  if (data > MINECGVAL )
  {
    if( data > Threshold )                       // засекли высокий уровень (пик)
    {                               
      //OffStat = 0;

      //TLed::LedGreen_On();
      //TLed::LED_IndicateCharge();
    
      //TInterpreter::SendFrameToHost(CMD_BEEP);
    
      /*
      if( StartMeasurment == false ){                      // если таймер не запущен 
        Hartbeat.Value = (int)( TIM_GetCounter( TIM2 ));
    
      TEcgProcessor::Hartbeat.Redraw = true;
      StartMeasurment = true;
      TIM_SetCounter( TIM2, 0 );                        // сброс счётчика 
      */
  
      if (rOnStatus == 0) rOnStatus++;
    }
  
    else
    {                                             // засекли низкий уровень 
      if( OffStat > 10 )
      {
        OffStat = 0;
//      StartMeasurment = false;
      
//      LedOff();
//      TDevice::DEVICE->DebugTools->Led.Off();
      //TLed::Leds_Off();
      
      }
      OffStat++;
      rOffStatus++;
    }
  }
  
  if(rOnStatus != 0)
  {
    rOnStatus++;
    TLed::LED_IndicateCharge();
  }
  else TLed::Leds_Off();
 
  if(rOnStatus > 25)
  {
    rOnStatus = 0;
  }
 
  rOnOffPrd++;
  
  if( rOnOffPrd > 800 )
  {                          // сброс порога, пика давно нет  //800
    if((rOnStatus - rOffStatus) > 780)           // 780
      rMaxVal = 0;
    
    if((rOffStatus - rOnStatus) > 780)
      rMaxVal = 0;
        
    rOnOffPrd = 0;
    rOnStatus = 0;
    rOffStatus = 0;
  }
}

/*----------------------------------------------------------------------------------*/
void TECG::TimerInit( void )
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );

  TIM_TimeBaseStructure.TIM_Prescaler = 1024;
  
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit( TIM3, &TIM_TimeBaseStructure );

  TIM_Cmd( TIM3, ENABLE );    // запускаем таймер для измерения периода между R зубцами 

  DBGMCU_Config( DBGMCU_TIM3_STOP, ENABLE );
}

/*----------------------------------------------------------------------------------*/
void TECG::TimerDeinit( void )
{
  TIM_DeInit( TIM3 );
}

/*----------------------------------------------------------------------------------*/
void TECG::StandBy()
{
  SetOpcode( CS_ADS1298, ADS1298_OPCODE_STANDBY );
  SetOpcode( CS_ADS1292, ADS1292_OPCODE_STANDBY );
}

/*----------------------------------------------------------------------------------*/
void TECG::WakeUp()
{
  SetOpcode( CS_ADS1298, ADS1298_OPCODE_WAKEUP );
  SetOpcode( CS_ADS1292, ADS1292_OPCODE_WAKEUP );
}
/*----------------------------------------------------------------------------------*/
void TECG::SetResp( bool new_state )
{
  RespOn = new_state;
//  if (new_state != RespOn)
//  {
//    RespOn = new_state;
//    if (RespOn) SetOpcode( CS_ADS1292, ADS1292_OPCODE_WAKEUP );
//    else SetOpcode( CS_ADS1292, ADS1292_OPCODE_STANDBY );
//  }
}
/*----------------------------------------------------------------------------------*/
void TECG::SetECGOn( bool new_state )
{
  if (new_state != ECGOn)
  {
    ECGOn = new_state;
    if (ECGOn) StartConversation();
    else StopConversation();
  }
}
/*----------------------------------------------------------------------------------*/
void TECG::GetSynthChannel()
{
  uint32_t res = 0;
  for (int ch = 0; ch < ChannelCount; ch++)
  {
    res = res + (uint32_t)(DiffFltData[ch] * DiffFltData[ch]);
  } 
  SynthChannel = res;
}
/*----------------------------------------------------------------------------------*/
void TECG::DiffFlt()
{
  static int16_t Xci[8][7];
  static bool first = true;
  
  for(uint8_t ch = 0; ch < 8; ch++)
  {
    for(uint8_t i = 6; i > 0; i--)
    {
      if(first)
      {
        Xci[ch][i] = 0;
      }
      else
      {
        Xci[ch][i] = Xci[ch][i - 1];
      }
    }
    Xci[ch][0] = EcgData[ch];
       
    DiffFltData[ch] = Xci[ch][0] + Xci[ch][1] + Xci[ch][2] - Xci[ch][4] - Xci[ch][5] - Xci[ch][6]; 
  }
  first = false;
}
/*----------------------------------------------------------------------------------*/
void TECG::FindR()
{
  static uint32_t RThreshold = 0;

  if(SynthChannel > RThreshold)
  {
    RThreshold = SynthChannel;
    R = true;
  }
  else
  {
    RThreshold = (uint32_t)(((uint64_t)(RThreshold * 1020)) / 1024);
  }
  
}
