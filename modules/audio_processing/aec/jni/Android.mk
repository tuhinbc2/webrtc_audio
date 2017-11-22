#TODO: neon sign ?? try it.

LOCAL_PATH := $(call my-dir)

ARCHITECTURE := $(TARGET_ARCH_ABI)

$(warning $(ARCHITECTURE))

WEBRTC_PATH := ../../../../
VAD_PATH := $(WEBRTC_PATH)modules/audio_processing/vad/
COMMON_AUDIO_PATH := $(WEBRTC_PATH)common_audio/
ISAC_PATH := $(WEBRTC_PATH)modules/audio_coding/codecs/isac/main/source/
BASE_PATH := $(WEBRTC_PATH)base/
COMMON_VAD_PATH := $(WEBRTC_PATH)common_audio/vad/
SP_PATH := $(WEBRTC_PATH)common_audio/signal_processing
UTILITY_PATH := $(WEBRTC_PATH)modules/audio_processing/utility

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(WEBRTC_PATH)
AEC_PATH := ..

#	$(AEC_PATH)/aec_core_mips.cc \
#	$(AEC_PATH)/aec_core_neon.cc \
#   $(AEC_PATH)/aec_core_sse2.cc \

LOCAL_MODULE    := webrtc_aec
LOCAL_SRC_FILES :=  \
	$(AEC_PATH)/aec_core.cc \
	$(AEC_PATH)/aec_resampler.cc \
	$(AEC_PATH)/echo_cancellation.cc \
	$(UTILITY_PATH)/ooura_fft.cc \
	$(UTILITY_PATH)/delay_estimator_wrapper.cc \
	$(UTILITY_PATH)/block_mean_calculator.cc \
	$(UTILITY_PATH)/delay_estimator.cc \
	$(SP_PATH)/randomization_functions.c \
	$(SP_PATH)/spl_init.c \
	$(SP_PATH)/cross_correlation.c \
	$(SP_PATH)/downsample_fast.c \
	$(SP_PATH)/vector_scaling_operations.c \
	$(SP_PATH)/min_max_operations.c \
	$(SP_PATH)/auto_correlation.c \
	$(COMMON_AUDIO_PATH)/ring_buffer.c \
	$(WEBRTC_PATH)/system_wrappers\source\metrics_default.cc \
	$(WEBRTC_PATH)/rtc_base\checks.cc \
	$(WEBRTC_PATH)/rtc_base\criticalsection.cc \
	$(WEBRTC_PATH)/modules\audio_processing\logging\apm_data_dumper.cc \

	
	
#common_audio\signal_processing\min_max_operations.c has neon version
	
	
	


#ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
#endif # TARGET_ARCH_ABI == armeabi-v7a

LOCAL_CFLAGS := -DWEBRTC_ANDROID -O3 -DWEBRTC_POSIX -D__UCLIBC__ -DWEBRTC_APM_DEBUG_DUMP=0 -D__STDC_LIMIT_MACROS
#-mfloat-abi=softfp -DWEBRTC_HAS_NEON -mfpu=neon

#TODO: enable cflags -O3
#TODO: who defined __GLIBCXX__? . for more info remove uclibc from cflag
LOCAL_LDLIBS := -llog
#LOCAL_STATIC_LIBRARIES := cpufeatures
include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_STATIC_LIBRARY)

#$(call import-module,android/cpufeatures)
