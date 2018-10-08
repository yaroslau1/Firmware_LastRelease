#ifndef TECG_H
#define TECG_H
/*----------------------------------------------------------------------------------*/
#include "main.h"
#include "THardware.h"
/*----------------------------------------------------------------------------------*/
#define MINECGVAL  100

enum ECG_CS {CS_NONE,
					   CS_ADS1298,     
					   CS_ADS1292,       
					   CS_EEPROM
};
typedef struct
{
	uint8_t ECGStat[3];		/* первые 4 байта */

	uint8_t ECGCh1[3];		/* 4 байта на канал */
	uint8_t ECGCh2[3];
	uint8_t ECGCh3[3];
	uint8_t ECGCh4[3];
	uint8_t ECGCh5[3];
	uint8_t ECGCh6[3];
	uint8_t ECGCh7[3];
	uint8_t ECGCh8[3];		/* последние 4 байта */

	uint8_t RespStat[3];		/* первые 4 байта */

	uint8_t RespCh1[3];		/* последние 4 байта */
	uint8_t RespCh2[3];		/* последние 4 байта */
} EcgData_t;

typedef struct
{
	void ( *Enable )( void );
	void ( *Disable )( void );
	int  Value;
	bool Redraw;
	bool HartIconReset;
	bool Status;
} Hartbeat_TypeDef;

/*----------------------------------------------------------------------------------*/
class TECG //: public TDSP
{
	public:
		static void Init( void );
    
    static void SetECGOn (bool state);

		static void TestSignals( void );
    static void StandBy();
    static void WakeUp();

		static void GetConvertResult( void );

//		static EcgData_t EcgData;
		static int16_t EcgData[8];
		static int16_t DiffFltData[8];
		static uint8_t ADSData[33];
		static uint8_t DMAState;

		static void SPI_Start( bool state );
		static void SPI_Reset( uint8_t state );
		static void SPI_SendByte( uint8_t byte );
		static uint8_t SPI_ReceiveByte( uint8_t data );
		static DMA_InitTypeDef DMA_InitStructure;
    static void DMA_IRQHandler();
    
    static void HeartbeatCalculation( void );    
 	  
    static int FHigh_0_32_Fixed( int X_i, int ch, bool Reset, bool ZeroAdduction );
    
    static void TimerInit( void );
    static void TimerDeinit( void );
    static uint32_t SampleCounter;
    
    static void SetResp( bool new_state );
    static bool R;
    
  private:

		static void StartConversation( void );
		static void StopConversation( void );

		static void PHYInit( void );
		static void SPI_SSControl( uint8_t state );
    
//	static void SPI_SendByte( uint8_t byte );
//	static uint8_t SPI_ReceiveByte( uint8_t data );

    static int hartbeat_detector( int16_t wData );
		static void SetOpcode( uint8_t cs, uint8_t opcode );
		static uint8_t RegisterRead( uint8_t cs, uint8_t Register );
		static uint8_t DataRead( uint8_t cs );
		static void RegisterWrite( uint8_t cs, uint8_t Register, uint8_t Data );
		static void EraseFIFO( void );
    
    static Hartbeat_TypeDef Hartbeat;
    
    static int32_t rX_i_1;
	  static int32_t rY_i;
	  static int32_t rwData;

    static int32_t data;
	  static int32_t rOnStatus;
	  static int32_t rOffStatus;
	  static int32_t rMaxVal;
	  static uint32_t rOnOffPrd;
    static bool StartMeasurment;
    static bool RespOn;
    static bool ECGOn;
    static void GetSynthChannel(void);
    static uint32_t SynthChannel;
    static void DiffFlt(void);
    static uint8_t ChannelCount;
    static void FindR(void);

    
  protected:
};

#endif