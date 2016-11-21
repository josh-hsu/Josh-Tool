#
# Copyright (C) 2016 The Josh Tool Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    joshtool.cpp \
    fgo/FGO.cpp

LOCAL_MODULE := joshtool

LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_CFLAGS += -Wno-unused-parameter

LOCAL_STATIC_LIBRARIES := \
    libc \
    libjoshgame \

include $(BUILD_EXECUTABLE)

include $(LOCAL_PATH)/libjoshgame/Android.mk
