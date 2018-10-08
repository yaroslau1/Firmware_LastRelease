/************************************************************************************/
/* @title		ADS1298 control														*/
/* @author		Cyber Genius														*/
/* @version		0.1																	*/
/* @date		13.07.2012															*/
/************************************************************************************/
/* @brief																			*/
/************************************************************************************/
#include "TADS1298.h"
#include "TSDProcessor.h"
#include "main.h"
#include "TEcgProcessor.h"
#include "TAppProcessor.h"
#include "TDevice.h"
#include "TRespProcessor.h"
#include "TSpiSram.h"


/*----------------------------------------------------------------------------------*/
/* @brief	заполняет буффер как результат одного преобразования ADS1298			*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
bool TADS1298::GetConvertResultTest( void )
{
  bool res = true;
  static BYTE incSeq = 0;
  static BYTE dummyByte[32] ;
  static BYTE dummyByte2[32] ;

/*
	ADS1298PHYReinit();
	SPI_SSControl( 0 );

	SPI_SendByte( 0x01 );
	SPI_SendByte( 0x40 );

	SPI_SSControl( 1 );
	SPI_SSControl( 0 );

	SPI_SendByte( 0x03 );
//	SPI_SendByte( 0x00 );
//	SPI_SendByte( 0x00 );
//	SPI_SendByte( 0x00 );

  uint8_t address[4];
  uint32_t addr = TEcgProcessor::ram_addr;
  for(int i = 0; i < SPISRAM_ADDR_SIZE; i++)
  {
    address[i] = (uint8_t) (addr & 0xFF);
    addr = addr >> 8;
  }
  for(int i = SPISRAM_ADDR_SIZE; i > 0; i--)
  {
    SPI_SendByte(address[i - 1]);
  }

  TEcgProcessor::ram_addr += 32;
  
  SPI_ReceiveByte( 0x00 );


  for(int j = 0;j < 32;j++)
  {
    dummyByte[j] = SPI_ReceiveByte( 0x00 );
    if(dummyByte[j] != incSeq) res = false;
    incSeq++;
  }


	SPI_SSControl( 1 );
	ADS1298PHYDeinit();
*/  

  TSPISRAM::ReadSeq( TEcgProcessor::ram_addr, 32, &dummyByte[0] );
  TSPISRAM::ReadSeq( TEcgProcessor::ram_addr, 32, &dummyByte2[0] );
  TEcgProcessor::ram_addr += 32;
  for(int j = 0;j < 32;j++)
  {
    if(dummyByte[j] != dummyByte2[j]) 
      res = false;
  }
  for(int j = 0;j < 32;j++)
  {
    if(dummyByte[j] != incSeq) 
      res = false;
    incSeq++;
  }

  if (res) 
    return true;
  else
    return false;

//  return res;
}

/*
void TADS1298::GetConvertResult( void )
{


	ADS1298PHYReinit();
	SPI_SSControl( 0 );

	SPI_SendByte( 0x01 );
	SPI_SendByte( 0x40 );

	SPI_SSControl( 1 );
	SPI_SSControl( 0 );

	SPI_SendByte( 0x03 );
//	SPI_SendByte( 0x00 );
//	SPI_SendByte( 0x00 );
//	SPI_SendByte( 0x00 );

  uint8_t address[4];
  uint32_t addr = TEcgProcessor::ram_addr;
  for(int i = 0; i < SPISRAM_ADDR_SIZE; i++)
  {
    address[i] = (uint8_t) (addr & 0xFF);
    addr = addr >> 8;
  }
  for(int i = SPISRAM_ADDR_SIZE; i > 0; i--)
  {
    SPI_SendByte(address[i - 1]);
  }

  TEcgProcessor::ram_addr += 32;

	int16_t tmp = SPI_ReceiveByte( 0x00 ) << 8;
	tmp += SPI_ReceiveByte( 0x00 );

	TEcgProcessor::WrEcgData.Status.ChannelBytes[3] = 0;
	TEcgProcessor::WrEcgData.Status.ChannelBytes[2] = 0;
	TEcgProcessor::WrEcgData.Status.ChannelBytes[1] = 0;
//	TEcgProcessor::WrEcgData.Status.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
//	TEcgProcessor::WrEcgData.Status.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );
	TEcgProcessor::WrEcgData.Status.ChannelBytes[0] = SPI_ReceiveByte( 0x00 );

	if (TAppProcessor::cable_state == ct10Leads)
	{
		TEcgProcessor::WrEcgData.Channel_3.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_3.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_3.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

		TEcgProcessor::WrEcgData.Channel_1.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_1.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_1.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

		TEcgProcessor::WrEcgData.Channel_2.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_2.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_2.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

		TEcgProcessor::WrEcgData.Channel_7.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_7.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_7.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

		TEcgProcessor::WrEcgData.Channel_8.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_8.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_8.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

		TEcgProcessor::WrEcgData.Channel_6.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_6.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_6.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

		TEcgProcessor::WrEcgData.Channel_5.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_5.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_5.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

		TEcgProcessor::WrEcgData.Channel_4.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_4.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_4.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );
	} else {
		TEcgProcessor::WrEcgData.Channel_1.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_1.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_1.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

		TEcgProcessor::WrEcgData.Channel_3.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_3.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_3.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

		TEcgProcessor::WrEcgData.Channel_2.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_2.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_2.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

		TEcgProcessor::WrEcgData.Channel_4.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_4.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_4.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

		TEcgProcessor::WrEcgData.Channel_5.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_5.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_5.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

		TEcgProcessor::WrEcgData.Channel_6.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_6.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_6.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

		TEcgProcessor::WrEcgData.Channel_7.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_7.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_7.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

		TEcgProcessor::WrEcgData.Channel_8.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_8.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
		TEcgProcessor::WrEcgData.Channel_8.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );
	}


	TRespProcessor::WrRespData.Status.ChannelBytes[3] = 0;
	TRespProcessor::WrRespData.Status.ChannelBytes[2] = 0;
	TRespProcessor::WrRespData.Status.ChannelBytes[1] = 0;
	TRespProcessor::WrRespData.Status.ChannelBytes[0] = 0;
//	TRespProcessor::WrRespData.Status.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
//	TRespProcessor::WrRespData.Status.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );
//	TRespProcessor::WrRespData.Status.ChannelBytes[0] = SPI_ReceiveByte( 0x00 );

	TRespProcessor::WrRespData.Channel_1.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
	TRespProcessor::WrRespData.Channel_1.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
	TRespProcessor::WrRespData.Channel_1.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );

//	TRespProcessor::WrRespData.Channel_2.ChannelBytes[3] = SPI_ReceiveByte( 0x00 );
//	TRespProcessor::WrRespData.Channel_2.ChannelBytes[2] = SPI_ReceiveByte( 0x00 );
//	TRespProcessor::WrRespData.Channel_2.ChannelBytes[1] = SPI_ReceiveByte( 0x00 );



	SPI_SSControl( 1 );
	ADS1298PHYDeinit();

	uint8_t ADCi = (TEcgProcessor::WrEcgData.Status.ChannelBytes[0] & 0x30) >> 4;
	if(ADCi == 6)
    TAppProcessor::SysVoltageValue = tmp;
	else if(ADCi == 5)
    TAppProcessor::AnalogVoltageValue = tmp;
	else if(ADCi == 4)
    TAppProcessor::LiVoltageValue = tmp;
	else if(ADCi == 3)
    TAppProcessor::BatVoltageValue = tmp;
	else if(ADCi == 2)
	{
		TAppProcessor::AX_CoordWr[ADCi] = ((tmp - 2048) * 100 / ADXL_WR_DIVIDE);
	}
	else
	{
		TAppProcessor::AX_CoordWr[ADCi] = -((tmp - 2048) * 100 / ADXL_WR_DIVIDE);
	}





	TEcgProcessor::WrEcgData.Channel_1.ChannelData = TEcgProcessor::WrEcgData.Channel_1.ChannelData >> 8;		// сдвигаем на 8 и делаем 24 битные данные 
	TEcgProcessor::WrEcgData.Channel_2.ChannelData = TEcgProcessor::WrEcgData.Channel_2.ChannelData >> 8;
	TEcgProcessor::WrEcgData.Channel_3.ChannelData = TEcgProcessor::WrEcgData.Channel_3.ChannelData >> 8;
	TEcgProcessor::WrEcgData.Channel_4.ChannelData = TEcgProcessor::WrEcgData.Channel_4.ChannelData >> 8;
	TEcgProcessor::WrEcgData.Channel_5.ChannelData = TEcgProcessor::WrEcgData.Channel_5.ChannelData >> 8;
	TEcgProcessor::WrEcgData.Channel_6.ChannelData = TEcgProcessor::WrEcgData.Channel_6.ChannelData >> 8;
	TEcgProcessor::WrEcgData.Channel_7.ChannelData = TEcgProcessor::WrEcgData.Channel_7.ChannelData >> 8;
	TEcgProcessor::WrEcgData.Channel_8.ChannelData = TEcgProcessor::WrEcgData.Channel_8.ChannelData >> 8;
	TRespProcessor::WrRespData.Channel_1.ChannelData = TRespProcessor::WrRespData.Channel_1.ChannelData >> 8;		// сдвигаем на 8 и делаем 24 битные данные 
	TRespProcessor::WrRespData.Channel_2.ChannelData = TRespProcessor::WrRespData.Channel_2.ChannelData >> 8;


#ifdef LED_PACE_HARD
//	TDevice::LedOff();
	if((TEcgProcessor::WrEcgData.Status.ChannelBytes[0] & 0x08) != 0) TDevice::LedOn();
	else TDevice::LedOff();
#endif

}
*/

void TADS1298::GetConvertResult( void )
{

  static BYTE Recived[32] ;
//  static BYTE Recived2[32] ;
//  static bool res;

//  TSPISRAM::ReadSeq( TEcgProcessor::ram_addr, 32, &Recived[0] );
//  TSPISRAM::ReadSeq( TEcgProcessor::ram_addr, 32, &Recived2[0] );

  for (uint32_t j = 0; j < 32; j++)
  {
    Recived[j] = TEcgProcessor::BuffEcgData[TEcgProcessor::ram_addr + j];
  }
  
  TEcgProcessor::ram_addr += 32;
/*
  for(int j = 0;j < 32;j++)
  {
    if(Recived[j] != Recived2[j]) 
      res = false;
  }
*/
	int16_t tmp = Recived[0] << 8;
	tmp += Recived[1];

	TEcgProcessor::WrEcgData.Status.ChannelBytes[3] = 0;
	TEcgProcessor::WrEcgData.Status.ChannelBytes[2] = 0;
	TEcgProcessor::WrEcgData.Status.ChannelBytes[1] = 0;
	TEcgProcessor::WrEcgData.Status.ChannelBytes[0] = Recived[2];

	if (TAppProcessor::cable_state == ct10Leads)
	{
		TEcgProcessor::WrEcgData.Channel[2].ChannelBytes[3] = Recived[3];
		TEcgProcessor::WrEcgData.Channel[2].ChannelBytes[2] = Recived[4];
		TEcgProcessor::WrEcgData.Channel[2].ChannelBytes[1] = Recived[5];

		TEcgProcessor::WrEcgData.Channel[0].ChannelBytes[3] = Recived[6];
		TEcgProcessor::WrEcgData.Channel[0].ChannelBytes[2] = Recived[7];
		TEcgProcessor::WrEcgData.Channel[0].ChannelBytes[1] = Recived[8];

		TEcgProcessor::WrEcgData.Channel[1].ChannelBytes[3] = Recived[9];
		TEcgProcessor::WrEcgData.Channel[1].ChannelBytes[2] = Recived[10];
		TEcgProcessor::WrEcgData.Channel[1].ChannelBytes[1] = Recived[11];

		TEcgProcessor::WrEcgData.Channel[6].ChannelBytes[3] = Recived[12];
		TEcgProcessor::WrEcgData.Channel[6].ChannelBytes[2] = Recived[13];
		TEcgProcessor::WrEcgData.Channel[6].ChannelBytes[1] = Recived[14];

		TEcgProcessor::WrEcgData.Channel[7].ChannelBytes[3] = Recived[15];
		TEcgProcessor::WrEcgData.Channel[7].ChannelBytes[2] = Recived[16];
		TEcgProcessor::WrEcgData.Channel[7].ChannelBytes[1] = Recived[17];

		TEcgProcessor::WrEcgData.Channel[5].ChannelBytes[3] = Recived[18];
		TEcgProcessor::WrEcgData.Channel[5].ChannelBytes[2] = Recived[19];
		TEcgProcessor::WrEcgData.Channel[5].ChannelBytes[1] = Recived[20];

		TEcgProcessor::WrEcgData.Channel[4].ChannelBytes[3] = Recived[21];
		TEcgProcessor::WrEcgData.Channel[4].ChannelBytes[2] = Recived[22];
		TEcgProcessor::WrEcgData.Channel[4].ChannelBytes[1] = Recived[23];

		TEcgProcessor::WrEcgData.Channel[3].ChannelBytes[3] = Recived[24];
		TEcgProcessor::WrEcgData.Channel[3].ChannelBytes[2] = Recived[25];
		TEcgProcessor::WrEcgData.Channel[3].ChannelBytes[1] = Recived[26];
	} else {
		TEcgProcessor::WrEcgData.Channel[0].ChannelBytes[3] = Recived[3];
		TEcgProcessor::WrEcgData.Channel[0].ChannelBytes[2] = Recived[4];
		TEcgProcessor::WrEcgData.Channel[0].ChannelBytes[1] = Recived[5];

		TEcgProcessor::WrEcgData.Channel[2].ChannelBytes[3] = Recived[6];
		TEcgProcessor::WrEcgData.Channel[2].ChannelBytes[2] = Recived[7];
		TEcgProcessor::WrEcgData.Channel[2].ChannelBytes[1] = Recived[8];

		TEcgProcessor::WrEcgData.Channel[1].ChannelBytes[3] = Recived[9];
		TEcgProcessor::WrEcgData.Channel[1].ChannelBytes[2] = Recived[10];
		TEcgProcessor::WrEcgData.Channel[1].ChannelBytes[1] = Recived[11];
	}


	TRespProcessor::WrRespData.Status.ChannelBytes[3] = 0;
	TRespProcessor::WrRespData.Status.ChannelBytes[2] = 0;
	TRespProcessor::WrRespData.Status.ChannelBytes[1] = 0;
	TRespProcessor::WrRespData.Status.ChannelBytes[0] = 0;

	TRespProcessor::WrRespData.Channel_1.ChannelBytes[3] = Recived[27];
	TRespProcessor::WrRespData.Channel_1.ChannelBytes[2] = Recived[28];
	TRespProcessor::WrRespData.Channel_1.ChannelBytes[1] = Recived[29];

	uint8_t ADCi = (TEcgProcessor::WrEcgData.Status.ChannelBytes[0] & 0x70) >> 4;
	if(ADCi == 6)
    TAppProcessor::SysVoltageValue = tmp;
	else if(ADCi == 5)
    TAppProcessor::AnalogVoltageValue = tmp;
	else if(ADCi == 4)
    TAppProcessor::LiVoltageValue = tmp;
	else if(ADCi == 3)
    TAppProcessor::BatVoltageValue = tmp;
	else if(ADCi == 2)
	{
		TAppProcessor::AX_CoordWr[ADCi] = ((tmp - 2048) * 100 / ADXL_WR_DIVIDE);
	}
	else
	{
		TAppProcessor::AX_CoordWr[ADCi] = -((tmp - 2048) * 100 / ADXL_WR_DIVIDE);
	}

  uint8_t ch_cnt = 3;
  if (TAppProcessor::cable_state == ct10Leads)
	{
    ch_cnt = 8;
	}

  for(uint8_t i = 0; i < ch_cnt; i++)
  {
    TEcgProcessor::WrEcgData.Channel[i].ChannelData = TEcgProcessor::WrEcgData.Channel[i].ChannelData >> 8;		/* сдвигаем на 8 и делаем 24 битные данные */
  }
	TRespProcessor::WrRespData.Channel_1.ChannelData = TRespProcessor::WrRespData.Channel_1.ChannelData >> 8;		/* сдвигаем на 8 и делаем 24 битные данные */
//	TRespProcessor::WrRespData.Channel_2.ChannelData = TRespProcessor::WrRespData.Channel_2.ChannelData >> 8;


#ifdef LED_PACE_HARD
//	TDevice::LedOff();
	if((TEcgProcessor::WrEcgData.Status.ChannelBytes[0] & 0x08) != 0) TDevice::LedOn();
	else TDevice::LedOff();
#endif

}

/*----------------------------------------------------------------------------------*/
/* @brief	Hастраивает ADS1298 на преобразование									*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void TADS1298::StartConversation( void )
{
}



/*----------------------------------------------------------------------------------*/
/* @brief	Включает тестовые сигналы в виде меандра. Также необходимо запускать	*/
/*			перед началом работы снятия экг, чтобы прошла внутреняя калибровка АЦП	*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void TADS1298::TestSignals( void )
{
}



/*----------------------------------------------------------------------------------*/
/* @brief	None																	*/
/* @param	None																	*/
/* @return	None																	*/
/*----------------------------------------------------------------------------------*/
void TADS1298::StopConversation( void )
{
}



/*----------------------------------------------------------------------------------*/
void TADS1298::SPI_Start( bool state )
{
}



/*----------------------------------------------------------------------------------*/
void TADS1298::SPI_Reset( bool state )
{
}



/*----------------------------------------------------------------------------------*/
void TADS1298::SPI_SSControl( bool state )
{
	if( state == true ) GPIO_SetBits( ADS1298_CS_PORT, ADS1298_CS_PIN );
	else GPIO_ResetBits( ADS1298_CS_PORT, ADS1298_CS_PIN );
}



/*----------------------------------------------------------------------------------*/
void TADS1298::SPI_SendByte( uint8_t byte )
{
	SPI_I2S_SendData( ADS1298_SPI, byte );
	while( SPI_I2S_GetFlagStatus( ADS1298_SPI, SPI_I2S_FLAG_BSY ) == SET );  // while( SPI_I2S_GetFlagStatus( ADS1298_SPI, SPI_I2S_FLAG_TXE ) == RESET )
	__delay_cycles(	1 );
}



/*----------------------------------------------------------------------------------*/
uint8_t TADS1298::SPI_ReceiveByte( uint8_t data )
{
	volatile uint8_t Data;
	SPI_I2S_SendData( ADS1298_SPI, data );
	while( SPI_I2S_GetFlagStatus( ADS1298_SPI, SPI_I2S_FLAG_BSY ) == SET )  // while( SPI_I2S_GetFlagStatus( ADS1298_SPI, SPI_I2S_FLAG_RXNE ) == RESET )
		;
	__delay_cycles(	1 );
	Data = SPI_I2S_ReceiveData( ADS1298_SPI );
	return Data;
}



/*----------------------------------------------------------------------------------*/
void TADS1298::ADS1298_Init( void )
{
/*	SPI_Start( 0 );
	__delay_cycles( 3000 );
	SPI_Reset( 1 );
	__delay_cycles( 3000 );
	SPI_Reset( 0 );
	__delay_cycles( 3000 );
	SPI_Reset( 1 );
	__delay_cycles( 1000000 );
*/
	FFHigh_MA0( 0, NULL, true );		/* filters reset */
	FFHigh_MA1( 0, NULL, true );
	FFHigh_MA2( 0, NULL, true );

	for(uint8_t i = 0; i < 8; i++)
  {
    FHigh_0_32_Fixed( 0, i, true, false );
  }
	for( int i=0; i<512; i++ ){					/* очистка буфферов */
		TSDProcessor::EcgBuffer_1[i] = 0;
		TSDProcessor::EcgBuffer_2[i] = 0;
	}
}



/*----------------------------------------------------------------------------------*/
void TADS1298::SetOpcode( uint8_t opcode )
{
}


/*----------------------------------------------------------------------------------*/
uint8_t TADS1298::RegisterRead( uint8_t Register )
{
/*	volatile uint8_t data;
	SPI_SSControl( 0 );

	SPI_SendByte( ADS1298_OPCODE_SDATAC );

	SPI_SendByte( ADS1298_OPCODE_RREG | Register );

	data = SPI_ReceiveByte( 0x00 );
	data = SPI_ReceiveByte( 0x00 );

	SPI_SSControl( 1 );
	return data;
*/	return 0;
}



/*----------------------------------------------------------------------------------*/
uint8_t TADS1298::DataRead( void )
{
	volatile uint8_t data;
	SPI_SSControl( 0 );

	data = SPI_ReceiveByte( 0x00 );

	SPI_SSControl( 1 );
	return data;
}



/*----------------------------------------------------------------------------------*/
void TADS1298::RegisterWrite( uint8_t Register, uint8_t Data )
{
	SPI_SSControl( 0 );

	SPI_SendByte( ADS1298_OPCODE_WREG | Register );
	SPI_SendByte( 0x00 );
	SPI_SendByte( Data );

	SPI_SSControl( 1 );
}



/*----------------------------------------------------------------------------------*/
void TADS1298::ADS1298PHYInit( void )
{
	ADS1298_GPIO_RCC_ENABLE;
	ADS1298_SPI_RCC_ENABLE;

	GPIO_InitTypeDef GPIO_InitStructure;

#ifdef BUFFERED_TRANSFER
  TSPISRAM::PHYInit();
#else
	// SPI pins config
	GPIO_InitStructure.GPIO_Pin = ADS1298_SCK_PIN | ADS1298_MOSI_PIN | ADS1298_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;
	GPIO_PinAFConfig(ADS1298_SPI_PORT, ADS1298_SCK_PINSOURCE, ADS1298_SPI_AF);
	GPIO_PinAFConfig(ADS1298_SPI_PORT, ADS1298_MISO_PINSOURCE, ADS1298_SPI_AF);
	GPIO_PinAFConfig(ADS1298_SPI_PORT, ADS1298_MOSI_PINSOURCE, ADS1298_SPI_AF);
	GPIO_Init( ADS1298_SPI_PORT, &GPIO_InitStructure );

	// CS
	GPIO_SetBits( ADS1298_CS_PORT, ADS1298_CS_PIN );
	GPIO_InitStructure.GPIO_Pin = ADS1298_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType =  GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;
	GPIO_Init( ADS1298_CS_PORT, &GPIO_InitStructure );

	// SPI Setup
	SPI_InitTypeDef SPI_InitStructure;

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;    // SPI_BaudRatePrescaler_16
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init( ADS1298_SPI, &SPI_InitStructure );

	SPI_Cmd( ADS1298_SPI, ENABLE );
#endif

	//---- DRDY CONFIG -----------------------------------------------
	GPIO_InitStructure.GPIO_Pin = ADS1298_DRDY_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType =  GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;
	GPIO_Init( ADS1298_DRDY_PORT, &GPIO_InitStructure );

	EXTI_InitTypeDef EXTI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig( ADS_DRDY_EXTI_PORT, ADS_DRDY_EXTI_PIN);

	EXTI_ClearITPendingBit( ADS_DRDY_EXTI_LINE ); 			// Clear the Key Button EXTI line pending bit
	EXTI_InitStructure.EXTI_Line = ADS_DRDY_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init( &EXTI_InitStructure );

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = ADS_DRDY_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13; //104
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init( &NVIC_InitStructure );
}
/*----------------------------------------------------------------------------------*/
void TADS1298::ADS1298PHYReinit( void )
{
#ifdef BUFFERED_TRANSFER
  TSPISRAM::PHYReinit();
#else
	GPIO_InitTypeDef GPIO_InitStructure;

	// SPI pins config
	GPIO_InitStructure.GPIO_Pin = ADS1298_SCK_PIN | ADS1298_MOSI_PIN | ADS1298_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;
	GPIO_PinAFConfig(ADS1298_SPI_PORT, ADS1298_SCK_PINSOURCE, ADS1298_SPI_AF);
	GPIO_PinAFConfig(ADS1298_SPI_PORT, ADS1298_MISO_PINSOURCE, ADS1298_SPI_AF);
	GPIO_PinAFConfig(ADS1298_SPI_PORT, ADS1298_MOSI_PINSOURCE, ADS1298_SPI_AF);
	GPIO_Init( ADS1298_SPI_PORT, &GPIO_InitStructure );
#endif
}
/*----------------------------------------------------------------------------------*/
void TADS1298::ADS1298PHYDeinit( void )
{
#ifdef BUFFERED_TRANSFER
  TSPISRAM::PHYDeinit();
#else
	GPIO_InitTypeDef GPIO_InitStructure;

	// SPI pins config
	GPIO_InitStructure.GPIO_Pin = ADS1298_SCK_PIN | ADS1298_MOSI_PIN | ADS1298_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;
	GPIO_Init( ADS1298_SPI_PORT, &GPIO_InitStructure );
#endif
}
/*----------------------------------------------------------------------------------*/

void TADS1298::StandBy()
{
  SetOpcode( ADS1298_OPCODE_STANDBY );
}
/*----------------------------------------------------------------------------------*/

void TADS1298::WakeUp()
{
  SetOpcode( ADS1298_OPCODE_WAKEUP );
}
/*----------------------------------------------------------------------------------*/
