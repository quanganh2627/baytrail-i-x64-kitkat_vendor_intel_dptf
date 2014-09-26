# Copyright (C) 2009 The Android Open Source Project
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
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

DPTF_SRC = ../../../../Sources
DPTF_INC = $(LOCAL_PATH)/$(DPTF_SRC)

#LOCAL_ALLOW_UNDEFINED_SYMBOLS := true
LOCAL_MODULE    := DptfPolicyCritical

LOCAL_C_INCLUDES := \
        bionic \
        bionic/libstdc++ \
	prebuilts/ndk/current/sources/cxx-stl/gnu-libstdc++/libs/x86/include \
	prebuilts/ndk/current/sources/cxx-stl/gnu-libstdc++/include \
	$(DPTF_INC) \
	$(DPTF_INC)/SharedLib/SharedLib \
	$(DPTF_INC)/PolicyLib/PolicyLib \
	$(DPTF_INC)/Policies/CriticalPolicy/CriticalPolicy \
	$(DPTF_INC)/EsifInclude \
	$(DPTF_INC)/ThirdParty

LOCAL_SHARED_LIBRARIES := \
        libnativehelper \
        libcutils \
        libutils \
        liblog \
        libdl

LOCAL_CFLAGS += -std=c++11 -fexceptions -frtti -Wall -fPIC \
        -DESIF_ATTR_OS_ANDROID -DESIF_ATTR_USER \
	-Wno-error=non-virtual-dtor

LOCAL_STATIC_LIBRARIES := DptfPolicy DptfParticipant DptfShared

FILE_LIST := $(wildcard $(LOCAL_PATH)/$(DPTF_SRC)/Policies/CriticalPolicy/CriticalPolicy/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_SDK_VERSION := current
LOCAL_NDK_STL_VARIANT := gnustl_shared

include $(BUILD_SHARED_LIBRARY)

SYMLINKS := $(TARGET_OUT)/etc/dptf/bin/$(LOCAL_MODULE).so
$(SYMLINKS): POLICY_LIB := /system/lib/$(LOCAL_MODULE).so
$(SYMLINKS): $(LOCAL_INSTALLED_MODULE) $(LOCAL_PATH)/Android.mk
	@echo "Symlink: $@ -> $(POLICY_LIB)"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf $(POLICY_LIB) $@

ALL_DEFAULT_INSTALLED_MODULES += $(SYMLINKS)

