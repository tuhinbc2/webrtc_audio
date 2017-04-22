LOCAL_PATH := $(call my-dir)

ARCHITECTURE := $(TARGET_ARCH_ABI)

$(warning $(ARCHITECTURE))

WEBRTC_PATH := ../../../../../
VAD_PATH := $(WEBRTC_PATH)modules/audio_processing/vad/
COMMON_AUDIO_PATH := $(WEBRTC_PATH)common_audio/
ISAC_PATH := $(WEBRTC_PATH)modules/audio_coding/codecs/isac/main/source/
BASE_PATH := $(WEBRTC_PATH)base/
COMMON_VAD_PATH := $(WEBRTC_PATH)common_audio/vad/
SP_PATH := $(WEBRTC_PATH)common_audio/signal_processing/

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(WEBRTC_PATH)
AGC_PATH := ../../
L_AGC_PATH := $(AGC_PATH)/legacy/

LOCAL_MODULE    := agc
LOCAL_SRC_FILES :=  \
	$(L_AGC_PATH)/analog_agc.c \
	$(L_AGC_PATH)/digital_agc.c \
	$(BASE_PATH)checks.cc \
	$(SP_PATH)spl_sqrt.c \
	$(SP_PATH)division_operations.c \
	$(SP_PATH)resample_by_2.c \
	$(SP_PATH)dot_product_with_scale.cc \
	$(SP_PATH)copy_set_operations.c \




#	$(SP_PATH)spl_init.c \


	







	#$(SP_PATH)resample_48khz.c \
	




	#$(SP_PATH)energy.c \
	





	#$(SP_PATH)resample.c \
	

	#$(COMMON_AUDIO_PATH)fft4g.c \
	

	#$(WEBRTC_PATH)common_audio/resampler/resampler.cc \
	

	#$(WEBRTC_PATH)system_wrappers/source/metrics_default.cc \


LOCAL_CFLAGS := -DWEBRTC_ANDROID -O3 -DWEBRTC_POSIX -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -D__UCLIBC__
#TODO: enable cflags -O3
#TODO: who defined __GLIBCXX__? . for more info remove uclibc from cflag
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)
