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

LOCAL_MODULE    := agc
LOCAL_SRC_FILES :=  \
	$(AGC_PATH)/agc.cc \
	$(AGC_PATH)/agc_manager_direct.cc \
	$(AGC_PATH)/loudness_histogram.cc \
	$(AGC_PATH)/utility.cc \
	$(VAD_PATH)voice_activity_detector.cc \
	$(VAD_PATH)vad_audio_proc.cc \
	$(VAD_PATH)pole_zero_filter.cc \
	$(VAD_PATH)pitch_internal.cc \
	$(VAD_PATH)standalone_vad.cc \
	$(VAD_PATH)pitch_based_vad.cc \
	$(VAD_PATH)gmm.cc \
	$(VAD_PATH)vad_circular_buffer.cc \
	$(COMMON_AUDIO_PATH)fft4g.c \
	$(ISAC_PATH)intialize.c \
	$(ISAC_PATH)lpc_analysis.c \
	$(ISAC_PATH)filter_functions.c \
	$(ISAC_PATH)filterbanks.c \
	$(ISAC_PATH)pitch_estimator.c \
	$(ISAC_PATH)filterbank_tables.c \
	$(ISAC_PATH)pitch_filter.c \
	$(BASE_PATH)logging.cc \
	$(BASE_PATH)timeutils.cc \
	$(BASE_PATH)platform_thread.cc \
	$(BASE_PATH)checks.cc \
	$(BASE_PATH)event_tracer.cc \
	$(BASE_PATH)criticalsection.cc \
	$(BASE_PATH)event.cc \
	$(BASE_PATH)stringencode.cc \
	$(WEBRTC_PATH)system_wrappers/source/metrics_default.cc \
	$(COMMON_VAD_PATH)webrtc_vad.c \
	$(COMMON_VAD_PATH)vad_core.c \
	$(COMMON_VAD_PATH)vad_sp.c \
	$(COMMON_VAD_PATH)vad_filterbank.c \
	$(COMMON_VAD_PATH)vad_gmm.c \
	$(SP_PATH)spl_init.c \
	$(SP_PATH)min_max_operations.c \
	$(SP_PATH)cross_correlation.c \
	$(SP_PATH)downsample_fast.c \
	$(SP_PATH)vector_scaling_operations.c \
	$(SP_PATH)resample_48khz.c \
	$(SP_PATH)resample_by_2_internal.c \
	$(SP_PATH)resample_fractional.c \
	$(SP_PATH)energy.c \
	$(SP_PATH)get_scaling_square.c \
	$(SP_PATH)division_operations.c \
	$(SP_PATH)resample.c \
	$(SP_PATH)resample_by_2.c \
	$(WEBRTC_PATH)common_audio/resampler/resampler.cc \

LOCAL_CFLAGS := -DWEBRTC_ANDROID -O3 -DWEBRTC_POSIX -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -D__UCLIBC__
#TODO: enable cflags -O3
#TODO: who defined __GLIBCXX__? . for more info remove uclibc from cflag
LOCAL_LDLIBS := -llog
include $(BUILD_SHARED_LIBRARY)
