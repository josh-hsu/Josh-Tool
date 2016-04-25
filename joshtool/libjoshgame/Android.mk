LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    CaptureService.cpp \
    CoreThread.cpp \
    InputDevice.cpp \
    InputService.cpp \
    SettingService.cpp

LOCAL_MODULE := libjoshgame

LOCAL_CLANG := true

include $(BUILD_STATIC_LIBRARY)
