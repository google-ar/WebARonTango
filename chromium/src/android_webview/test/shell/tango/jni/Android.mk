#
# Copyright 2014 Google Inc. All Rights Reserved.
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

# ==========================================================
# tango_client_api
# ==========================================================
LOCAL_PATH := ../../../../../third_party/tango/libtango_client_api/arm64-v8a
include $(CLEAR_VARS)
LOCAL_MODULE := tango_client_api
LOCAL_SRC_FILES := \
	libtango_client_api.so
include $(PREBUILT_SHARED_LIBRARY)

# ==========================================================
# tango_support_api
# ==========================================================
LOCAL_PATH := ../../../../../third_party/tango/libtango_support_api/arm64-v8a
include $(CLEAR_VARS)
LOCAL_MODULE := tango_support_api
LOCAL_SRC_FILES := \
	libtango_support_api.so
include $(PREBUILT_SHARED_LIBRARY)

# ==========================================================
# tango_chromium
# ==========================================================
LOCAL_PATH := .
include $(CLEAR_VARS)
LOCAL_MODULE := libtango_chromium
LOCAL_C_INCLUDES := \
	. \
	../../../../../third_party/tango/libtango_client_api \
	../../../../../third_party/tango/libtango_support_api
LOCAL_SRC_FILES := TangoHandler.cpp \
                   TangoHandlerJNIInterface.cpp
LOCAL_CFLAGS := -std=gnu++11 -Werror -fexceptions
LOCAL_SHARED_LIBRARIES := tango_client_api tango_support_api
LOCAL_LDLIBS := -llog -landroid -lGLESv2 -lEGL
include $(BUILD_SHARED_LIBRARY)