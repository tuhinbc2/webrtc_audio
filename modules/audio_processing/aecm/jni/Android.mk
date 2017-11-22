LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := webrtc_aecm
LOCAL_CFLAGS += -DWEBRTC_POSIX -DWEBRTC_ANDROID -D__STDC_LIMIT_MACROS -D__UCLIBC__

WEBRTC_PATH := ../../../../
VAD_PATH := $(WEBRTC_PATH)modules/audio_processing/vad/
COMMON_AUDIO_PATH := $(WEBRTC_PATH)common_audio/
ISAC_PATH := $(WEBRTC_PATH)modules/audio_coding/codecs/isac/main/source/
BASE_PATH := $(WEBRTC_PATH)base/
COMMON_VAD_PATH := $(WEBRTC_PATH)common_audio/vad/
SP_PATH := $(WEBRTC_PATH)common_audio/signal_processing
UTILITY_PATH := $(WEBRTC_PATH)modules/audio_processing/utility

AECM_PATH := ..
LOCAL_CPP_EXTENSION := .cc .cpp .cxx
LOCAL_C_INCLUDES := $(WEBRTC_PATH)
LOCAL_SRC_FILES	:= \
				$(AECM_PATH)/aecm_core.cc \
				$(AECM_PATH)/aecm_core_c.cc \
				$(SP_PATH)/complex_bit_reverse.c \
				$(SP_PATH)/complex_fft.c \
				$(SP_PATH)/cross_correlation.c \
				$(UTILITY_PATH)/delay_estimator.cc \
				$(UTILITY_PATH)/delay_estimator_wrapper.cc \
				$(SP_PATH)/division_operations.c \
				$(SP_PATH)/dot_product_with_scale.cc \
				$(SP_PATH)/downsample_fast.c \
				$(AECM_PATH)/echo_control_mobile.cc \
				$(SP_PATH)/min_max_operations.c \
				$(SP_PATH)/randomization_functions.c \
				$(SP_PATH)/real_fft.c \
				$(COMMON_AUDIO_PATH)/ring_buffer.c \
				$(SP_PATH)/spl_init.c \
				$(SP_PATH)/spl_sqrt_floor.c \
				$(SP_PATH)/vector_scaling_operations.c \
				$(WEBRTC_PATH)rtc_base/checks.cc \
				
LOCAL_LDLIBS := -llog
				
include $(BUILD_STATIC_LIBRARY)

				