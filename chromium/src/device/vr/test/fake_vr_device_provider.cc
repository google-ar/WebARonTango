// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/vr/test/fake_vr_device_provider.h"

namespace device {

FakeVRDeviceProvider::FakeVRDeviceProvider() : VRDeviceProvider() {
  initialized_ = false;
}

FakeVRDeviceProvider::~FakeVRDeviceProvider() {}

void FakeVRDeviceProvider::AddDevice(std::unique_ptr<VRDevice> device) {
  devices_.push_back(std::move(device));
}

void FakeVRDeviceProvider::RemoveDevice(std::unique_ptr<VRDevice> device) {
  std::vector<std::unique_ptr<VRDevice>>::iterator iter = devices_.begin();
  while (iter != devices_.end()) {
    if (device == *iter) {
      iter = devices_.erase(iter);
    } else {
      ++iter;
    }
  }
}

void FakeVRDeviceProvider::GetDevices(std::vector<VRDevice*>* devices) {
  for (const auto& device : devices_) {
    devices->push_back(device.get());
  }
}

void FakeVRDeviceProvider::Initialize() {
  initialized_ = true;
}

}  // namespace device
