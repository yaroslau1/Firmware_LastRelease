#ifndef TAUDIO_H
#define TAUDIO_H

#include "main.h"
//#include "speex_header.h"
//#include <speex/speex.h>
#include "TSDProcessor.h"

#define ADC1_BUFFER_SIZE		1024 //2048 //1024 V104
#define AUDIO_BUFFER_SIZE		1024

#define AUDIO_ENCODED_SIZE	160
#define ENCODED_FRAME_SIZE  20

#define AUDIO_SAMPLE_RATE   8000




/*----------------------------------------------------------------------------------*/
class TAudio
{
	public:
		static void AUDIO_PHYInit( void );
		static void TASK_WriteOnSD( void *pvParameters );
		static void EncodeData( void );
//		static void Speex_WriteInit( void );
//		static void Speex_WriteHeader( FIL *encodedfile, SpeexHeader *SpHeader );

		static int16_t ADC1_raw_data[ADC1_BUFFER_SIZE];
		static int16_t Audio_raw_data[AUDIO_BUFFER_SIZE];
		static uint8_t AudioCodedData[AUDIO_ENCODED_SIZE];		/* сюда сохраняются закодированные данные */
		static uint8_t *pRecData;
		static xTaskHandle xWriteOnSD;
		static bool VoiceRec;

//		static SpeexBits bits;
		static void *enc_state;

	private:

	protected:

};
#endif