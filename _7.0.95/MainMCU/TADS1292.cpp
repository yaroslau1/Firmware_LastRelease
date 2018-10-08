/********************************************************************************/
/* @title		ADS1292 control						*/
/* @author		Aleksis							*/
/* @version		0.1							*/
/* @date		24.04.2014						*/
/********************************************************************************/
/* @brief									*/
/********************************************************************************/
#include "TADS1292.h"
#include "TADS1298.h"
#include "TSDProcessor.h"
#include "main.h"
#include "TRespProcessor.h"
#include "TAppProcessor.h"

#ifdef USE_ADS1292R


/*------------------------------------------------------------------------------*/
/* @brief	заполняет буффер как результат одного преобразования ADS1292	*/
/* @param	None								*/
/* @return	None								*/
/*------------------------------------------------------------------------------*/
void TADS1292::GetConvertResult( void )
{
/*	TRespProcessor::RespData.Status.ChannelBytes[3] = 0;

	TRespProcessor::RespData.Status.ChannelBytes[2] = DataRead();
	TRespProcessor::RespData.Status.ChannelBytes[1] = DataRead();
	TRespProcessor::RespData.Status.ChannelBytes[0] = DataRead();

	TRespProcessor::RespData.Channel_1.ChannelBytes[3] = DataRead();
	TRespProcessor::RespData.Channel_1.ChannelBytes[2] = DataRead();
	TRespProcessor::RespData.Channel_1.ChannelBytes[1] = DataRead();

	TRespProcessor::RespData.Channel_2.ChannelBytes[3] = DataRead();
	TRespProcessor::RespData.Channel_2.ChannelBytes[2] = DataRead();
	TRespProcessor::RespData.Channel_2.ChannelBytes[1] = DataRead();

	TRespProcessor::RespData.Channel_1.ChannelData = TRespProcessor::RespData.Channel_1.ChannelData >> 8;		// сдвигаем на 8 и делаем 24 битные данные 
	TRespProcessor::RespData.Channel_2.ChannelData = TRespProcessor::RespData.Channel_2.ChannelData >> 8;
*/}



/*------------------------------------------------------------------------------*/
/* @brief	Hастраивает ADS1292 на преобразование				*/
/* @param	None								*/
/* @return	None								*/
/*------------------------------------------------------------------------------*/
void TADS1292::StartConversation( void )
{
}



/*------------------------------------------------------------------------------*/
/* @brief	Включает тестовые сигналы в виде меандра. Необходимо запускать	*/
/*		перед началом работы, чтобы прошла внутреняя калибровка АЦП	*/
/* @param	None								*/
/* @return	None								*/
/*------------------------------------------------------------------------------*/
void TADS1292::TestSignals( void )
{
}



/*------------------------------------------------------------------------------*/
/* @brief	Выключает преобразование					*/
/* @param	None								*/
/* @return	None								*/
/*------------------------------------------------------------------------------*/
void TADS1292::StopConversation( void )
{
}



/*------------------------------------------------------------------------------*/
/* @brief	Устанавливает уровень на линии Reset				*/
/* @param	state - требуемый уровень					*/
/* @return	None								*/
/*------------------------------------------------------------------------------*/
void TADS1292::SPI_Reset( bool state )
{
}



/*------------------------------------------------------------------------------*/
/* @brief	Устанавливает уровень на линии CS				*/
/* @param	state - требуемый уровень					*/
/* @return	None								*/
/*------------------------------------------------------------------------------*/
void TADS1292::SPI_SSControl( bool state )
{
/*	if( state == true ) GPIO_SetBits( ADS1292_CS_PORT, ADS1292_CS_PIN );
	else GPIO_ResetBits( ADS1292_CS_PORT, ADS1292_CS_PIN );
*/}



/*------------------------------------------------------------------------------*/
/* @brief	Пересылает по SPI байт						*/
/* @param	byte - пересылаемый байт					*/
/* @return	None								*/
/*------------------------------------------------------------------------------*/
void TADS1292::SPI_SendByte( uint8_t byte )
{
//	SPI_I2S_SendData( ADS1298_SPI, byte );
//	while( SPI_I2S_GetFlagStatus( ADS1298_SPI, SPI_I2S_FLAG_TXE ) == RESET );
//	__delay_cycles(	1 );
TADS1298::SPI_SendByte( byte );
}



/*------------------------------------------------------------------------------*/
/* @brief	Принимает по SPI байт						*/
/* @param	data - пересылаемый байт					*/
/* @return	принятый байт							*/
/*------------------------------------------------------------------------------*/
uint8_t TADS1292::SPI_ReceiveByte( uint8_t data )
{
//	volatile uint8_t Data;
//	SPI_I2S_SendData( ADS1298_SPI, data );
//	while( SPI_I2S_GetFlagStatus( ADS1298_SPI, SPI_I2S_FLAG_RXNE ) == RESET )
//		;
//	__delay_cycles(	1 );
//	Data = SPI_I2S_ReceiveData( ADS1298_SPI );
//	return Data;
//	return TADS1298::SPI_ReceiveByte( data );
	return data;
}



/*------------------------------------------------------------------------------*/
/* @brief	Инициализация ADS1292						*/
/* @param	None								*/
/* @return	None								*/
/*------------------------------------------------------------------------------*/
void TADS1292::ADS1292_Init( void )
{
	__delay_cycles( 3000 );
	SPI_Reset( 0 );
	__delay_cycles( 3000 );
	SPI_Reset( 1 );
	__delay_cycles( 3000 );

//	FFHigh_MA0( 0, NULL, true );		/* filters reset */
//	FFHigh_MA1( 0, NULL, true );
//	FFHigh_MA2( 0, NULL, true );

	FHigh_0_32_Fixed( 0, 8, true, false );
	FHigh_0_32_Fixed( 0, 9, true, false );

//	for( int i=0; i<512; i++ ){					/* очистка буфферов */
//		TSDProcessor::EcgBuffer_1[i] = 0;
//		TSDProcessor::EcgBuffer_2[i] = 0;
//	}
}



/*----------------------------------------------------------------------------------*/
void TADS1292::SetOpcode( uint8_t opcode )
{
	SPI_SSControl( 0 );
	SPI_SendByte( opcode );
	SPI_SSControl( 1 );
}


/*----------------------------------------------------------------------------------*/
uint8_t TADS1292::RegisterRead( uint8_t Register )
{
	volatile uint8_t data;
	SPI_SSControl( 0 );

	SPI_SendByte( ADS1292_OPCODE_SDATAC );

	SPI_SendByte( ADS1292_OPCODE_RREG | Register );

	data = SPI_ReceiveByte( 0x00 );
	data = SPI_ReceiveByte( 0x00 );

	SPI_SSControl( 1 );
	return data;
}



/*----------------------------------------------------------------------------------*/
uint8_t TADS1292::DataRead( void )
{
	volatile uint8_t data;
	SPI_SSControl( 0 );

	data = SPI_ReceiveByte( 0x00 );

	SPI_SSControl( 1 );
	return data;
}



/*----------------------------------------------------------------------------------*/
void TADS1292::RegisterWrite( uint8_t Register, uint8_t Data )
{
	SPI_SSControl( 0 );

	SPI_SendByte( ADS1292_OPCODE_WREG | Register );
	SPI_SendByte( 0x00 );
	SPI_SendByte( Data );

	SPI_SSControl( 1 );
}



/*------------------------------------------------------------------------------*/
/* @brief	Инициализация GPIO						*/
/* @param	None								*/
/* @return	None								*/
/*------------------------------------------------------------------------------*/
void TADS1292::ADS1292PHYInit( void )
{
/*	GPIO_InitTypeDef GPIO_InitStructure;

	ADS1292_GPIO_RCC_ENABLE;

	// RESET pin
	GPIO_SetBits( ADS1292_RESET_PORT, ADS1292_RESET_PIN );
	GPIO_InitStructure.GPIO_Pin = ADS1292_RESET_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;
	GPIO_Init( ADS1292_RESET_PORT, &GPIO_InitStructure );

	// CS 
	GPIO_SetBits( ADS1292_CS_PORT, ADS1292_CS_PIN );
	GPIO_InitStructure.GPIO_Pin = ADS1292_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init( ADS1292_CS_PORT, &GPIO_InitStructure );
*/}



/*------------------------------------------------------------------------------*/
/* @brief	Вход в энергосберегающий режим					*/
/* @param	None								*/
/* @return	None								*/
/*------------------------------------------------------------------------------*/
void TADS1292::StandBy()
{
	SetOpcode( ADS1292_OPCODE_STANDBY );
}



/*------------------------------------------------------------------------------*/
/* @brief	Выход из энергосберегающего режима				*/
/* @param	None								*/
/* @return	None								*/
/*------------------------------------------------------------------------------*/
void TADS1292::WakeUp()
{
	SetOpcode( ADS1292_OPCODE_WAKEUP );
}

# endif