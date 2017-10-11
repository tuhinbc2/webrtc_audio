#TODO: neon sign ?? try it.

LOCAL_PATH := $(call my-dir)

ARCHITECTURE := $(TARGET_ARCH_ABI)

$(warning $(ARCHITECTURE))

WEBRTC_PATH := ../../../../../
VAD_PATH := $(WEBRTC_PATH)modules/audio_processing/vad/
COMMON_AUDIO_PATH := $(WEBRTC_PATH)common_audio/
ISAC_PATH := $(WEBRTC_PATH)modules/audio_coding/codecs/isac/main/source/
BASE_PATH := $(WEBRTC_PATH)base/
COMMON_VAD_PATH := $(WEBRTC_PATH)common_audio/vad/
SP_PATH := $(WEBRTC_PATH)common_audio/signal_processing

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(WEBRTC_PATH)
NS_PATH := ../..



LOCAL_MODULE    := webrtc_ns
LOCAL_SRC_FILES :=  \
	$(NS_PATH)/defines.h \
	$(NS_PATH)/noise_suppression.c \
	$(NS_PATH)/noise_suppression.h \
	$(NS_PATH)/noise_suppression_x.c \
	$(NS_PATH)/noise_suppression_x.h \
	$(NS_PATH)/nsx_core.c \
	$(NS_PATH)/nsx_core.h \
	$(NS_PATH)/nsx_core_c.c \
	$(NS_PATH)/nsx_defines.h \
	$(NS_PATH)/ns_core.c \
	$(NS_PATH)/ns_core.h \
	$(NS_PATH)/windows_private.h \
    $(SP_PATH)/auto_correlation.c \
    $(SP_PATH)/auto_corr_to_refl_coef.c \
    $(SP_PATH)/complex_bit_reverse.c \
    $(SP_PATH)/complex_fft.c \
    $(SP_PATH)/copy_set_operations.c \
    $(SP_PATH)/cross_correlation.c \
    $(SP_PATH)/division_operations.c \
    $(SP_PATH)/downsample_fast.c \
    $(SP_PATH)/energy.c \
    $(SP_PATH)/get_hanning_window.c \
    $(SP_PATH)/get_scaling_square.c \
    $(SP_PATH)/ilbc_specific_functions.c \
    $(SP_PATH)/levinson_durbin.c \
    $(SP_PATH)/lpc_to_refl_coef.c \
    $(SP_PATH)/min_max_operations.c \
    $(SP_PATH)/randomization_functions.c \
    $(SP_PATH)/real_fft.c \
    $(SP_PATH)/refl_coef_to_lpc.c \
    $(SP_PATH)/resample.c \
    $(SP_PATH)/resample_48khz.c \
    $(SP_PATH)/resample_by_2.c \
    $(SP_PATH)/resample_by_2_internal.c \
    $(SP_PATH)/resample_fractional.c \
    $(SP_PATH)/splitting_filter.c \
    $(SP_PATH)/spl_init.c \
    $(SP_PATH)/spl_inl.c \
    $(SP_PATH)/spl_sqrt.c \
    $(SP_PATH)/spl_sqrt_floor.c \
    $(SP_PATH)/sqrt_of_one_minus_x_squared.c \
    $(SP_PATH)/vector_scaling_operations.c \
    $(COMMON_AUDIO_PATH)/fft4g.c

#    $(SP_PATH)/dot_product_with_scale.c \

#	$(NS_PATH)/nsx_core_mips.c \
#	$(NS_PATH)/nsx_core_neon.c \
#	$(BASE_PATH)checks.cc \

#ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
#endif # TARGET_ARCH_ABI == armeabi-v7a

LOCAL_CFLAGS := -DWEBRTC_ANDROID -O3 -DWEBRTC_POSIX -D__UCLIBC__
#-mfloat-abi=softfp -DWEBRTC_HAS_NEON -mfpu=neon

#TODO: enable cflags -O3
#TODO: who defined __GLIBCXX__? . for more info remove uclibc from cflag
LOCAL_LDLIBS := -llog
#LOCAL_STATIC_LIBRARIES := cpufeatures
#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)

#$(call import-module,android/cpufeatures)
