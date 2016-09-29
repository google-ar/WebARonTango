// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/vr/test/fake_vr_device_provider.h"

namespace device {

FakeVRDeviceProvider::FakeVRDeviceProvider() : VRDeviceProvider() {
  initialized_ = false;
}

FakeVRDeviceProvider::~FakeVRDeviceProvider() {}

void FakeVRDeviceProvider::AddDevice(VRDevice* device) {
  devices_.push_back(device);
}

void FakeVRDeviceProvider::RemoveDevice(VRDevice* device) {
  std::vector<VRDevice*>::iterator iter = devices_.begin();
  while (iter != devices_.end()) {
    if (device == *iter) {
      iter = devices_.erase(iter);
    } else {
      ++iter;
    }
  }
}

void FakeVRDeviceProvider::GetDevices(std::vector<VRDevice*>* devices) {
  std::vector<VRDevice*>::iterator iter;

  for (auto device : devices_) {
    devices->push_back(device);
  }
}

void FakeVRDeviceProvider::Initialize() {
  initialized_ = true;
}

}  // namespace device
