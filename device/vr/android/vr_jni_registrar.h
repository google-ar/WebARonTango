// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_VR_ANDROID_VR_JNI_REGISTRAR_H_
#define DEVICE_VR_ANDROID_VR_JNI_REGISTRAR_H_

#include <jni.h>

#include "device/vr/vr_export.h"

namespace device {
namespace android {

// Registers C++ methods in device/vr classes with JNI.
// See https://www.chromium.org/developers/design-documents/android-jni
//
// Must be called before classes in the VR module are used.
DEVICE_VR_EXPORT bool RegisterVRJni(JNIEnv* env);

}  // namespace android
}  // namespace device

#endif  // DEVICE_VR_ANDROID_VR_JNI_REGISTRAR_H_
