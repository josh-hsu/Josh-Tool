LOCAL_PATH := $(call my-dir)

###############################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := JoshToolApp
LOCAL_SRC_FILES := app-debug.apk
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_TAGS := debug
LOCAL_MODULE_SUFFIX := $(COMMON_ANDROID_PACKAGE_SUFFIX)
LOCAL_CERTIFICATE := platform
LOCAL_DEX_PREOPT := false
include $(BUILD_PREBUILT)
