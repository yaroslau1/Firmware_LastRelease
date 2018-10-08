/******************************************************************************/
/* @title       SPI SRAM control                                              */
/* @author      Alexey Konev                                                  */
/* @version     0.5                                                           */
/* @date        06.11.2015                                                    */
/******************************************************************************/
/* @brief                                                                     */
/******************************************************************************/

#include "TSpiSram.h"
#include "main.h"


DMA_InitTypeDef TSPISRAM::DMA_InitStructure;


extern uint16_t RegularConvData_Tab[7];

static uint8_t temp;
/*----------------------------------------------------------------------------*/
/* @brief       Peripheral initialization                                     */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
void TSPISRAM::PHYInit( void )
{
  // RCC config
  SPISRAM_GPIO_RCC_ENABLE;
  SPISRAM_SPI_RCC_ENABLE;

  GPIO_InitTypeDef GPIO_InitStructure;

  // SPI pins config
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  // GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = SPISRAM_PIN_PULL;

  GPIO_InitStructure.GPIO_Pin = SPISRAM_SCK_PIN;
  GPIO_PinAFConfig(SPISRAM_SCK_PORT, SPISRAM_SCK_PINSOURCE, SPISRAM_SPI_AF);
  GPIO_Init( SPISRAM_SCK_PORT, &GPIO_InitStructure );

  GPIO_InitStructure.GPIO_Pin = SPISRAM_MISO_PIN;
  GPIO_PinAFConfig(SPISRAM_MISO_PORT, SPISRAM_MISO_PINSOURCE, SPISRAM_SPI_AF);
  GPIO_Init( SPISRAM_MISO_PORT, &GPIO_InitStructure );

  GPIO_InitStructure.GPIO_Pin = SPISRAM_MOSI_PIN;
  GPIO_PinAFConfig(SPISRAM_MOSI_PORT, SPISRAM_MOSI_PINSOURCE, SPISRAM_SPI_AF);
  GPIO_Init( SPISRAM_MOSI_PORT, &GPIO_InitStructure );

  // CS pin config
  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );
  GPIO_InitStructure.GPIO_Pin = SPISRAM_CS_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = SPISRAM_PIN_PULL;
  GPIO_Init( SPISRAM_CS_PORT, &GPIO_InitStructure );

  // IRQ pin config
  GPIO_SetBits( SPISRAM_IRQ_PORT, SPISRAM_IRQ_PIN );
  GPIO_InitStructure.GPIO_Pin = SPISRAM_IRQ_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = SPISRAM_PIN_PULL;
  GPIO_Init( SPISRAM_CS_PORT, &GPIO_InitStructure );

  // SPI config
  SPI_InitTypeDef SPI_InitStructure;

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init( SPISRAM_SPI, &SPI_InitStructure );

  SPI_RxFIFOThresholdConfig(SPISRAM_SPI, SPI_RxFIFOThreshold_QF);

  // SPI enable
  SPI_Cmd( SPISRAM_SPI, ENABLE );







  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);


	// Enable SPI_MASTER DMA Tx request
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);








}


/*----------------------------------------------------------------------------*/
/* @brief       Peripheral deinitialization                                   */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
void TSPISRAM::PHYDeinit( void )
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // SPI pins config
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd =  SPISRAM_PIN_PULL;

  GPIO_InitStructure.GPIO_Pin = SPISRAM_SCK_PIN;
  GPIO_Init( SPISRAM_SCK_PORT, &GPIO_InitStructure );

  GPIO_InitStructure.GPIO_Pin = SPISRAM_MISO_PIN;
  GPIO_Init( SPISRAM_MISO_PORT, &GPIO_InitStructure );

  GPIO_InitStructure.GPIO_Pin = SPISRAM_MOSI_PIN;
  GPIO_Init( SPISRAM_MOSI_PORT, &GPIO_InitStructure );
}


/*----------------------------------------------------------------------------*/
/* @brief       Peripheral reinitialization                                   */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
void TSPISRAM::PHYReinit( void )
{
  GPIO_InitTypeDef GPIO_InitStructure;

  // SPI pins config
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = SPISRAM_PIN_PULL;

  GPIO_InitStructure.GPIO_Pin = SPISRAM_SCK_PIN;
  GPIO_PinAFConfig(SPISRAM_SCK_PORT, SPISRAM_SCK_PINSOURCE, SPISRAM_SPI_AF);
  GPIO_Init( SPISRAM_SCK_PORT, &GPIO_InitStructure );

  GPIO_InitStructure.GPIO_Pin = SPISRAM_MISO_PIN;
  GPIO_PinAFConfig(SPISRAM_MISO_PORT, SPISRAM_MISO_PINSOURCE, SPISRAM_SPI_AF);
  GPIO_Init( SPISRAM_MISO_PORT, &GPIO_InitStructure );

  GPIO_InitStructure.GPIO_Pin = SPISRAM_MOSI_PIN;
  GPIO_PinAFConfig(SPISRAM_MOSI_PORT, SPISRAM_MOSI_PINSOURCE, SPISRAM_SPI_AF);
  GPIO_Init( SPISRAM_MOSI_PORT, &GPIO_InitStructure );
}

/*----------------------------------------------------------------------------*/
/* @brief       Byte mode write in SPI SRAM                                   */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
void TSPISRAM::WriteByte( uint32_t addr, uint8_t data )
{
  PHYReinit();

  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_WRMR);
  SendByte(SPISRAM_BYTEMode);

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );
  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_WRITE);

  SendAddr(addr);

  SendByte(data);

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  PHYDeinit();
}


/*----------------------------------------------------------------------------*/
/* @brief       Byte mode write in SPI SRAM                                   */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
void TSPISRAM::WriteByte( uint32_t addr, uint8_t *data )
{
  PHYReinit();

  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_WRMR);
  SendByte(SPISRAM_BYTEMode);

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );
  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_WRITE);

  SendAddr(addr);

  SendByte(data[0]);

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  PHYDeinit();
}


/*----------------------------------------------------------------------------*/
/* @brief       Page mode write in SPI SRAM                                   */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
void TSPISRAM::WritePage( uint32_t addr, uint8_t *data )
{
  PHYReinit();

  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_WRMR);
  SendByte(SPISRAM_PAGEMode);

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );
  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_WRITE);

  SendAddr(addr);

  for(int i = 0; i < SPISRAM_PAGE_SIZE; i++) {
    SendByte(data[i]);
  }

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  PHYDeinit();
}


/*----------------------------------------------------------------------------*/
/* @brief       Sequential mode write in SPI SRAM                             */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
void TSPISRAM::WriteSeq( uint32_t addr, uint32_t size, uint8_t *data )
{
  PHYReinit();

  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_WRMR);
  SendByte(SPISRAM_SEQMode);

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );
  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_WRITE);

  SendAddr(addr);


  for(int i = 0; i < size; i++) {
    SendByte(data[i]);
  }

/*
  
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&data[0];
	DMA_InitStructure.DMA_BufferSize = size;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
  DMA_ClearFlag(DMA1_FLAG_TC3);
	DMA_Cmd(DMA1_Channel3, ENABLE);
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC3));
	DMA_Cmd(DMA1_Channel3, DISABLE);
*/
  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  PHYDeinit();

  
}


/*----------------------------------------------------------------------------*/
/* @brief       Byte mode read from SPI SRAM                                  */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
uint8_t TSPISRAM::ReadByte( uint32_t addr )
{
  PHYReinit();

  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_WRMR);
  SendByte(SPISRAM_BYTEMode);

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );
  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_READ);

  SendAddr(addr);

  uint8_t i = ReceiveByte(0);

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  PHYDeinit();
  
  return i;
}


/*----------------------------------------------------------------------------*/
/* @brief       Byte mode read from SPI SRAM                                  */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
void TSPISRAM::ReadByte( uint32_t addr, uint8_t *data )
{
  PHYReinit();

  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_WRMR);
  SendByte(SPISRAM_BYTEMode);

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );
  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_READ);

  SendAddr(addr);

  data[0] = ReceiveByte(0);

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  PHYDeinit();
}


/*----------------------------------------------------------------------------*/
/* @brief       Page mode read from SPI SRAM                                  */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
void TSPISRAM::ReadPage( uint32_t addr, uint8_t *data )
{
  PHYReinit();

  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_WRMR);
  SendByte(SPISRAM_PAGEMode);

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );
  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_READ);

  SendAddr(addr);

  for(int i = 0; i < SPISRAM_PAGE_SIZE; i++) {
    data[i] = ReceiveByte(0);
  }

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  PHYDeinit();
}


/*----------------------------------------------------------------------------*/
/* @brief       Sequential mode read from SPI SRAM                            */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
void TSPISRAM::ReadSeq( uint32_t addr, uint32_t size, uint8_t *data )
{
  PHYReinit();

  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_WRMR);
  SendByte(SPISRAM_SEQMode);

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );
  GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  SendByte(SPISRAM_READ);

  SendAddr(addr);

  for(int i = 0; i < size; i++) {
    temp = ReceiveByte((uint8_t)0);
    data[i] = temp;
  }

  GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );

  PHYDeinit();
}


/*----------------------------------------------------------------------------*/
/* @brief       Control CS line                                               */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
void TSPISRAM::SSControl( bool state )
{
	if( state == true ) GPIO_SetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );
	else GPIO_ResetBits( SPISRAM_CS_PORT, SPISRAM_CS_PIN );
}


/*----------------------------------------------------------------------------*/
/* @brief       Control IRQ line                                               */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
void TSPISRAM::IRQControl( bool state )
{
  GPIO_InitTypeDef GPIO_InitStructure;
	if( state == true ) 
  {
    GPIO_SetBits( SPISRAM_IRQ_PORT, SPISRAM_IRQ_PIN );
    GPIO_InitStructure.GPIO_Pin = SPISRAM_IRQ_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = SPISRAM_PIN_PULL;
    GPIO_Init( SPISRAM_IRQ_PORT, &GPIO_InitStructure );
  }
	else 
  {
    GPIO_ResetBits( SPISRAM_IRQ_PORT, SPISRAM_IRQ_PIN );
    GPIO_InitStructure.GPIO_Pin = SPISRAM_IRQ_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = SPISRAM_PIN_PULL;
    GPIO_Init( SPISRAM_IRQ_PORT, &GPIO_InitStructure );
  }
}


/*----------------------------------------------------------------------------*/
/* @brief       Sending byte by SPI                                           */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
void TSPISRAM::SendByte( uint8_t byte )
{
	SPI_SendData8( SPISRAM_SPI, byte );
	while( SPI_I2S_GetFlagStatus( SPISRAM_SPI, SPI_I2S_FLAG_BSY ) == SET );
	__delay_cycles(	1 );
}


/*----------------------------------------------------------------------------*/
/* @brief       Sending address by SPI                                           */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
void TSPISRAM::SendAddr( uint32_t addr )
{
  uint8_t address[4]; 

  for(int i = 0; i < SPISRAM_ADDR_SIZE; i++) 
  {
    address[i] = (uint8_t) (addr & 0xFF);
    addr = addr >> 8;
  }
  for(int i = SPISRAM_ADDR_SIZE; i > 0; i--) 
  {
    SendByte(address[i - 1]);
  }
}


/*----------------------------------------------------------------------------*/
/* @brief       Receiving byte by SPI                                         */
/* @param       None                                                          */
/* @return      None                                                          */
/*----------------------------------------------------------------------------*/
uint8_t TSPISRAM::ReceiveByte( uint8_t data )
{
  volatile uint8_t Data;

  while( SPI_GetReceptionFIFOStatus( SPISRAM_SPI ) != SPI_ReceptionFIFOStatus_Empty)
  {
    Data = SPI_ReceiveData8( SPISRAM_SPI );
  }

  while( SPI_GetTransmissionFIFOStatus( SPISRAM_SPI ) != SPI_TransmissionFIFOStatus_Empty);

  SPI_SendData8( SPISRAM_SPI, data );

  while( SPI_GetReceptionFIFOStatus( SPISRAM_SPI ) != SPI_ReceptionFIFOStatus_Empty);

  Data = SPI_ReceiveData8( SPISRAM_SPI );

  return Data;
}

