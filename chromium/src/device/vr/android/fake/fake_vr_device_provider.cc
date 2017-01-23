// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/vr/android/fake/fake_vr_device_provider.h"

#include <jni.h>

#include "device/vr/android/fake/fake_vr_device.h"
#include "device/vr/vr_device.h"
#include "device/vr/vr_device_manager.h"
#include "device/vr/vr_service.mojom.h"

namespace device {

FakeVRDeviceProvider::FakeVRDeviceProvider() {}

FakeVRDeviceProvider::~FakeVRDeviceProvider() {
}

void FakeVRDeviceProvider::GetDevices(std::vector<VRDevice*>* devices) {
  Initialize();

  if (vr_device_)
    devices->push_back(vr_device_.get());
}

void FakeVRDeviceProvider::Initialize() {
  if (!vr_device_) {
    vr_device_.reset(
        new FakeVRDevice(this));
  }
}

}  // namespace device
