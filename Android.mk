LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := audio_mixer

LOCAL_CPP_EXTENSION := .cc \
					   .cpp

LOCAL_SRC_FILES := \
	audio/audio_frame.cc \
	audio/audio_frame_operations.cc \
	audio_mixer/audio_frame_manipulator.cc \
	audio_mixer/audio_mixer_impl.cc \
	audio_mixer/default_output_rate_calculator.cc \
	audio_mixer/frame_combiner.cc \
	audio_mixer/gain_change_calculator.cc \
	audio_mixer/sine_wave_generator.cc \
	rtc_base/checks.cc \
	rtc_base/criticalsection.cc \
	rtc_base/platform_thread_types.cc \
	rtc_base/race_checker.cc \
	rtc_base/timeutils.cc
	

LOCAL_LDLIBS := -llog -lz

LOCAL_C_INCLUDES += $(LOCAL_PATH)/

LOCAL_CFLAGS := -DUSE_ARM_CONFIG
LOCAL_CFLAGS += -DWEBRTC_LINUX -DWEBRTC_ANDROID
LOCAL_CPPFLAGS := -pthread -frtti -fexceptions

include $(BUILD_STATIC_LIBRARY)



