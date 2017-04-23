#include <iostream>
#include <cstdlib>

#include "gain_control.h"

#define AGNMODE_ADAPTIVE_DIGITAL 2

#define LOGT(...) printf(__VA_ARGS__); putchar('\n');

void* AGC_instance;

int main()
{
	AGC_instance = WebRtcAgc_Create();
	int agcret;
	if ((agcret = WebRtcAgc_Init(AGC_instance, 1, 255, AGNMODE_ADAPTIVE_DIGITAL, 8000)))
	{
		LOGT("###GN## WebRtcAgc_Init failed");
	}
	else
	{
		LOGT("###GN## WebRtcAgc_Init successful");
	}



	WebRtcAgcConfig gain_config;

	gain_config.targetLevelDbfs = 3;
	gain_config.compressionGaindB = 10;
	gain_config.limiterEnable = true;
	if ((agcret = WebRtcAgc_set_config(AGC_instance, gain_config)))
	{
		LOGT("###GN## WebRtcAgc_set_config failed with error code");
	}
	else
	{
		LOGT("###GN## WebRtcAgc_Create successful");
	}


	FILE* in = fopen("data/near.pcm", "rb");
	FILE* out = fopen("near.pcm", "wb");

	if(in == NULL || out == NULL){
		LOGT("file not found");
		exit(1);
	}

	short in_buffer[81], out_buffer[81];

	in_buffer[0] = 50;
	//                                         int16_t *const *inMic,
	int16_t* p = in_buffer;
	int16_t* q = out_buffer;

	LOGT("data : %d",p[0]);
	int inMicLevel, outMicLevel = -1;
	uint8_t saturationWarning = 1;

	p++;
	while(true){
		if(fread(in_buffer + 1, 2, 80, in) < 1){
			break;
		}
		//LOGT("##")
		inMicLevel = 0;
		if (0 != WebRtcAgc_VirtualMic(AGC_instance, (int16_t *const *)&p, 1, 80, inMicLevel, &outMicLevel))
		{
			LOGT("###GN## WebRtcAgc_AddMic failed");
			break;
		}
		LOGT("outMicLevel %d", outMicLevel);
		/*int WebRtcAgc_Process(void* agcInst,
                      const int16_t* const* inNear,
                      size_t num_bands,
                      size_t samples,
                      int16_t* const* out,
                      int32_t inMicLevel,
                      int32_t* outMicLevel,
                      int16_t echo,
                      uint8_t* saturationWarning);*/

		if (0 != WebRtcAgc_Process(AGC_instance, (const int16_t* const*)&p, 1, 80, (int16_t *const *)&q, outMicLevel, &inMicLevel, 0, &saturationWarning))
		{
			LOGT("###GN## WebRtcAgc_Process failed");
			break;
		}


		fwrite(out_buffer, 2, 80, out);


	}




	WebRtcAgc_Free(AGC_instance);

	return 0;
}