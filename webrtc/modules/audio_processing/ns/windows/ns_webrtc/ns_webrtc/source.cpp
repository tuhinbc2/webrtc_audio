#ifdef _DEBUG


#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unordered_map>
#include <vector>
//X version

//#include "webrtc/modules/audio_processing/ns/nsx_core.h"
#include "webrtc/modules/audio_processing/ns/noise_suppression_x.h"
#include "gain_control.h"

#define LOGT(...) printf(__VA_ARGS__);putchar('\n');

using namespace std;
int current_mode = -1;
void change_mode(NsxHandle* handle, int mode)
{
	if (current_mode != mode){
		int re = WebRtcNsx_set_policy(handle, mode);
		if (re != 0){
			printf("set_policy returned %d", re);
			exit(-1);
		}
		current_mode = mode;
		printf("changing mode to %d\n", mode);
	}
}
int sample_size = 80;

class volgain
{
	void* AGC_instance = nullptr;
	short *m_sTempBuf = nullptr;
public:
	volgain(int samplerate)
	{
		AGC_instance = WebRtcAgc_Create();
		int agcret;
		if ((agcret = WebRtcAgc_Init(AGC_instance, 1, 255, 2, 8000)))
		{
			LOGT("###GN## WebRtcAgc_Init failed");
		}
		else
		{
			//LOGT("###GN## WebRtcAgc_Init successful");
		}

		WebRtcAgcConfig gain_config;

		gain_config.targetLevelDbfs = 3;      /* m_iVolume's range is 1-10 */ /* so effective dbfs range is 12-3 */    /* possible range: 0 - 31 */
		gain_config.compressionGaindB = 30;   /* possible range: 0 - 90 */
		gain_config.limiterEnable = true;

		if (WebRtcAgc_set_config(AGC_instance, gain_config))
		{
			LOGT("###GN## WebRtcAgc_set_config failed  ");
		}
		else
		{
			//LOGT("###GN## WebRtcAgc_set_config successful");
		}
		m_sTempBuf = new short[500];
	}

	~volgain()
	{
		if (m_sTempBuf)
			delete[] m_sTempBuf;
		WebRtcAgc_Free(AGC_instance);
	}

	void addgain(short* data)
	{
		uint8_t saturationWarning;
		int32_t inMicLevel = 0;
		int32_t outMicLevel = 0;

		if (0 != WebRtcAgc_VirtualMic(AGC_instance, (int16_t* const*)&data, 1, sample_size, inMicLevel, &outMicLevel))
		{
			LOGT("###GN## WebRtcAgc_VirtualMic failed");
		}

		if (0 != WebRtcAgc_Process(AGC_instance, (const int16_t* const*)&data, 1, sample_size,
			(int16_t* const*)&data, outMicLevel, &inMicLevel, 0, &saturationWarning))
		{
			LOGT("###GN## WebRtcAgc_Process failed");
			return;
		}
		/*if (memcmp(data, m_sTempBuf, sample_size * 2) == 0)
		{
			LOGT("### same data");
		}*/
		//memcpy(data, m_sTempBuf, sample_size * 2);
	}
};

class noiseremover
{
	NsxHandle *ns_handle = nullptr;
	short* m_sTempBuf = nullptr;
	int16_t* nsOut;
public:
	noiseremover(int mode)
	{
		ns_handle = WebRtcNsx_Create();
		int ret_value = WebRtcNsx_Init(ns_handle, 8000);
		if (ret_value != 0){
			printf("init returned %d", ret_value);
			exit(-1);
		}

		ret_value = WebRtcNsx_set_policy(ns_handle, mode);
		if (ret_value != 0){
			printf("set_policy returned %d", ret_value);
			exit(-1);
		}
		m_sTempBuf = new short[100];
		nsOut = m_sTempBuf;
	}

	~noiseremover()
	{
		if (ns_handle)
			WebRtcNsx_Free(ns_handle);
		if (m_sTempBuf)
			delete[] m_sTempBuf;
	}

	void removenoise(int16_t* data)
	{
		WebRtcNsx_Process(ns_handle, (const short *const *)&data, 1, (short *const *)&nsOut);
		memcpy(data, nsOut, 160);
	}
};

int do_ns(std::string input_type = "t1", int mode = 1 /*0: Mild, 1: Medium , 2: Aggressive*/)
{
	int ret_value = -1;
	//const int sample_rate = 8000;

	static std::unordered_map<int, std::string> mode_str = { { 0, "l1" }, { 1, "l2" }, { 2, "l3" }, { 3, "l4" } };
	FILE* f_input = NULL;
	FILE* f_output = NULL;
	std::string file_name_input = "data/input_";
	std::string file_name_output = "data/output/output_";
	file_name_input += input_type;
	file_name_input += std::string(".raw");
	file_name_output += input_type + "_" + mode_str[mode];
	//file_name_output += std::string(".raw");
	printf("Output: %s\n", file_name_output.c_str());
	f_input = fopen(file_name_input.c_str(), "rb");
	f_output = fopen(file_name_output.c_str(), "wb");
	if (f_input == NULL){
		printf("%s not found\n", file_name_input.c_str());
		exit(-1);
	}

	const int number = 1;
	noiseremover* ns[number];
	volgain* gain[number];

	for (int i = 0; i < number; i++){
		ns[i] = new noiseremover(mode);
		gain[i] = new volgain(8000);
	}
	

	int16_t buffer[2048];
	//int16_t output[80];

	//const int16_t d[2][200] = { 0 };
	

	/*//(const short *const *) &nsIn
	//(short *const *) &nsOut

	const short * buf_ptr;
	short * out_buf_ptr;

	const short* const* speechFrame = (const short *const *)&nsIn;

	short* frame = (short*)speechFrame[0];
	printf("hello val: %d\n", frame[99]);*/
//	int counter = 0;
//	const uint32_t *noise_estimate;
//	int noise_f;
	//int noise_bin = WebRtcNsx_num_freq();
	//int sum = 0;
//	FILE* io = fopen("hi", "w");
	//int binlen = WebRtcNsx_num_freq();
//	const uint32_t *bins;
//	int totalnoise = 0;

	while (true){

		ret_value = fread(buffer, 2, sample_size, f_input);
		if (ret_value < 1){
			break;
		}

		for (int i = 0; i < 1; i++){
			ns[i]->removenoise(buffer);
		}

		for (int i = 0; i < 1; i++){
			gain[i]->addgain(buffer);
		}

		fwrite(buffer, 2, sample_size, f_output);

		
// 		//memcpy(d[0], input, 160);
// 
// 		/*const short* const* speechFrame = (const short* const*) input;
// 		short* const* outFrame = (short* const*) output;
// 		for (int i = 0; i < 50; i++){
// 		printf("%d == %d\n ", *(speechFrame[i]), input[i]);
// 		}*/
// 		/*if (counter++ > 250){
// 		ret_value = WebRtcNsx_set_policy(ns_handle, 3);
// 		if (ret_value != 0){
// 		printf("set_policy returned %d", ret_value);
// 		exit(-1);
// 		}
// 		}*/
// 		
// 		//bins = WebRtcNsx_noise_estimate(ns_handle, &binlen);
// 		
// 
// 
// 		/*totalnoise = 0;
// 
// 		for (int i = 0; i < binlen; i++){
// 			totalnoise += bins[i];
// 		}
// 
// 		printf("TotalNoise: %d\n", totalnoise);*/
// 
// 		//WebRtcNsx_Process(ns_handle, (const short *const *)&nsIn, 1, (short *const *)&nsOut);
// 		//printf("number of frequency bin %d\n", WebRtcNsx_num_freq());
// 		/*noise_estimate = WebRtcNsx_noise_estimate(ns_handle, &noise_f);
// 		sum = 0;
// 		for (int i = 0; i < noise_bin; i++){
// 		sum += noise_estimate[i];
// 		//fprintf(io, "%d\n", noise_estimate[i]);
// 		}
// 
// 		if (sum < 1000000000){
// 		change_mode(ns_handle, 0);
// 		}
// 		else{
// 		change_mode(ns_handle, 3);
// 		}
// 
// 		printf("| noisebincount %d | qfactor %d | noise sum %d |\n", noise_bin, noise_f, sum);
// 
// 		for (int i = 0; i < 80; i++){
// 		if (std::abs(output[i]) < 50){
// 		output[i] = 0;
// 		}
// 		fprintf(io, "%d\n", output[i]);
// 		}
// 		//WebRtcNsx_Process(f_a->nois, (const short int * const*)d, 2, (short int * const*)d);*/
	}
	//printf("total iteration took: %d\n", counter);

	return 0;
}

int main()
{
	//do_ns();

	//return 1;


	std::vector<std::string> test_file = {"t1"};
	//std::vector<std::string> test_file = { "nv1", "note3", "bn", "mn", "wn", "t1", "babble", "babble_2", "car", "exhibition", "man", "noise_1", "noise_2" };

	for (auto file : test_file){
		for (int mode = 0; mode < 4; mode++){
			do_ns(file, mode);
		}
	}
}
#endif