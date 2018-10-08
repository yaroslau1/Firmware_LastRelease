/************************************************************************************/
/* @title		Audio processor														*/
/* @author		Cyber Genius														*/
/* @version		1.0																	*/
/* @date		17.08.2012															*/
/************************************************************************************/
/* @brief																			*/
/************************************************************************************/

#include "TAudio.h"
#include "THardware.h"
/************************************************************************************/

/************************************************************************************/
#define ADC1_DR_ADRESS ((uint32_t)0x4001204C)
/************************************************************************************/
int16_t TAudio::ADC1_raw_data[ADC1_BUFFER_SIZE];
int16_t TAudio::Audio_raw_data[AUDIO_BUFFER_SIZE];
bool TAudio::VoiceRec = false;
/************************************************************************************/

//SpeexBits TAudio::bits;								/* Holds bits so they can be read and written by the Speex routines */
void *TAudio::enc_state;							/* Holds the states of the encoder & the decoder */
uint8_t TAudio::AudioCodedData[AUDIO_ENCODED_SIZE]; /* сюда сохраняются закодированные данные */
uint8_t *TAudio::pRecData;
int quality = 4, complexity=1, vbr=0, enh=1;		/* SPEEX PARAMETERS, MUST REMAINED UNCHANGED */
//SpeexHeader header;

xTaskHandle TAudio::xWriteOnSD;

volatile int zond ;
FIL CodedFile;



/*----------------------------------------------------------------------------------*/
void TAudio::AUDIO_PHYInit( void )
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	// NVIC
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	// GPIO
	RCC_AHB1PeriphClockCmd( AudioIn_GPIO_CLK, ENABLE );
	GPIO_InitStructure.GPIO_Pin = AudioIn_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( AudioIn_GPIO_PORT, &GPIO_InitStructure );

	RCC_AHB1PeriphClockCmd( AudioPwr_GPIO_CLK, ENABLE );
  GPIO_SetBits(AudioPwr_GPIO_PORT,AudioPwr_GPIO_PIN);
	GPIO_InitStructure.GPIO_Pin = AudioPwr_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( AudioPwr_GPIO_PORT, &GPIO_InitStructure );
  
	// TIM
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM4, ENABLE );
	TIM_TimeBaseStructure.TIM_Period = (SystemCoreClock / AUDIO_SAMPLE_RATE) - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM4, &TIM_TimeBaseStructure );

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = (SystemCoreClock / AUDIO_SAMPLE_RATE) / 2 - 1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC4Init( TIM4, &TIM_OCInitStructure );

	TIM_OC4PreloadConfig( TIM4, TIM_OCPreload_Enable );

	TIM_SelectMasterSlaveMode( TIM4, TIM_MasterSlaveMode_Enable );

	TIM_SelectOutputTrigger( TIM4, TIM_TRGOSource_OC4Ref );

	// DMA
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_DMA2, ENABLE );
	DMA_DeInit(DMA2_Stream0);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&ADC1->DR);					/* адрес регистра переферии */
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&TAudio::ADC1_raw_data;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;							/* из переферии в память */
	DMA_InitStructure.DMA_BufferSize = ADC1_BUFFER_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init( DMA2_Stream0, &DMA_InitStructure );

	DMA_ITConfig( DMA2_Stream0, DMA_IT_TC | DMA_IT_HT, ENABLE );

	DMA_Cmd( DMA2_Stream0, ENABLE );









	// ADC
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1, ENABLE );

	ADC_InitStructure.ADC_ScanConvMode = DISABLE; //ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T4_CC4;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
	ADC_InitStructure.ADC_NbrOfConversion = 0; //2;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_Init( ADC1, &ADC_InitStructure );

	ADC_RegularChannelConfig( ADC1, AudioIn_ADC_Channel, 1, ADC_SampleTime_56Cycles );
//	ADC_RegularChannelConfig( ADC1, AudioIn_ADC_Channel, 2, ADC_SampleTime_56Cycles );

	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div8;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);



        
	ADC_DMARequestAfterLastTransferCmd( ADC1, ENABLE );

	ADC_DMACmd( ADC1, ENABLE );

	ADC_Cmd( ADC1, ENABLE );

	TIM_Cmd( TIM4, ENABLE );
}



///*----------------------------------------------------------------------------------*/
//void TAudio::TASK_WriteOnSD( void *pvParameters )
//{
//	vTaskSuspend( xWriteOnSD );
//
//	while( 1 ){
//		uint32_t s;
//
//		EncodeData();
//
////		FIL SoundFile;
////
////		if( SD_Init() == SD_OK ){
////			f_mount( 0, &TSDProcessor::FATFS_Obj );
////
////			f_open( &SoundFile, "sound.sss", FA_CREATE_ALWAYS | FA_WRITE );
////
////			uint8_t *uiTemp;
////			uiTemp = (uint8_t*)&ADC1_raw_data[0];
////
////			for( int i=0; i<((ADC1_BUFFER_SIZE*2)/512); i++ )
////				f_write( &SoundFile, &uiTemp[i*512], 512, &s );
////
////			f_close( &SoundFile );
////		}
//
//		if( disk_initialize(0) == RES_OK ){
//			f_mount( 0, &TSDProcessor::FATFS_Obj );
//			zond = 1000;
//			f_open( &CodedFile, "coded.spx", FA_CREATE_ALWAYS | FA_WRITE );
//
//			Speex_WriteHeader( &CodedFile, &header );
//
//			f_write( &CodedFile, AudioCodedData, 1000, &s );
//
//			f_close( &CodedFile );
//			zond = 0;
//		}
//
//		vTaskSuspend( NULL );
//	}
//}
//
//
//
///*----------------------------------------------------------------------------------*/
///* @brief	кодирование данных														*/
///* @param	None																	*/
///* @return	None																	*/
///*----------------------------------------------------------------------------------*/
//void TAudio::EncodeData( void )
//{
//	pRecData = AudioCodedData;
//	for( int t=0; t<50; t++ ){
//		speex_bits_reset( &bits );		/* обязательно - очистка буферов speex */
//
//		speex_encode_int( enc_state, (spx_int16_t*)ADC1_raw_data, &bits );
//
//		speex_bits_write( &bits, (char *)pRecData, ENCODED_FRAME_SIZE );	/* записваем ENCODED_FRAME_SIZE байт */
//		pRecData += ENCODED_FRAME_SIZE;
//	}
//}
//
//
//
///*----------------------------------------------------------------------------------*/
//void TAudio::Speex_WriteInit( void )
//{
//	const SpeexMode *mode=NULL;
//
//	mode = speex_lib_get_mode( SPEEX_MODEID_NB );
//	speex_init_header( &header, 8000, 1, mode );
//
//	enc_state = speex_encoder_init( &speex_nb_mode );
//
//	speex_encoder_ctl( enc_state, SPEEX_SET_QUALITY, &quality );
//	speex_encoder_ctl( enc_state, SPEEX_SET_COMPLEXITY, &complexity );
//
//	speex_bits_init( &bits );		/* Speex encoding initializations */
//}
//
//
//
///*----------------------------------------------------------------------------------*/
//void TAudio::Speex_WriteHeader( FIL *encodedfile, SpeexHeader *SpHeader )
//{
//	static uint8_t HeaderBuffer[80];
//	int k = 0;
//	uint32_t s;
//
//	for( int i=0; i<SPEEX_HEADER_STRING_LENGTH; i++ )
//		HeaderBuffer[k++] = (uint8_t)(SpHeader->speex_string[i]);
//
//	for( int i=0; i<SPEEX_HEADER_VERSION_LENGTH; i++ )
//		HeaderBuffer[k++] = (uint8_t)(SpHeader->speex_version[i]);
//
//	HeaderBuffer[k++] = (uint8_t)SpHeader->speex_version_id;
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->speex_version_id >> 8);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->speex_version_id >> 16);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->speex_version_id >> 24);
//
//	HeaderBuffer[k++] = (uint8_t)SpHeader->header_size;
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->header_size >> 8);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->header_size >> 16);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->header_size >> 24);
//
//	HeaderBuffer[k++] = (uint8_t)SpHeader->rate;
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->rate >> 8);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->rate >> 16);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->rate >> 24);
//
//	HeaderBuffer[k++] = (uint8_t)SpHeader->mode;
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->mode >> 8);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->mode >> 16);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->mode >> 24);
//
//	HeaderBuffer[k++] = (uint8_t)SpHeader->mode_bitstream_version;
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->mode_bitstream_version >> 8);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->mode_bitstream_version >> 16);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->mode_bitstream_version >> 24);
//
//	HeaderBuffer[k++] = (uint8_t)SpHeader->nb_channels;
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->nb_channels >> 8);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->nb_channels >> 16);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->nb_channels >> 24);
//
//	HeaderBuffer[k++] = (uint8_t)SpHeader->bitrate;
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->bitrate >> 8);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->bitrate >> 16);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->bitrate >> 24);
//
//	HeaderBuffer[k++] = (uint8_t)SpHeader->frame_size;
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->frame_size >> 8);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->frame_size >> 16);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->frame_size >> 24);
//
//	HeaderBuffer[k++] = (uint8_t)SpHeader->vbr;
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->vbr >> 8);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->vbr >> 16);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->vbr >> 24);
//
//	HeaderBuffer[k++] = (uint8_t)SpHeader->frames_per_packet;
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->frames_per_packet >> 8);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->frames_per_packet >> 16);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->frames_per_packet >> 24);
//
//	HeaderBuffer[k++] = (uint8_t)SpHeader->extra_headers;
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->extra_headers >> 8);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->extra_headers >> 16);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->extra_headers >> 24);
//
//	HeaderBuffer[k++] = (uint8_t)SpHeader->reserved1;
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->reserved1 >> 8);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->reserved1 >> 16);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->reserved1 >> 24);
//
//	HeaderBuffer[k++] = (uint8_t)SpHeader->reserved2;
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->reserved2 >> 8);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->reserved2 >> 16);
//	HeaderBuffer[k++] = (uint8_t)(SpHeader->reserved2 >> 24);
//
//	f_write( &CodedFile, HeaderBuffer, 80, &s );
//
//	f_sync( &CodedFile );
//}