// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/vr/android/tango/tango_vr_device_provider.h"

#include "device/vr/android/tango/tango_vr_device.h"

namespace device {

TangoVRDeviceProvider::TangoVRDeviceProvider() : VRDeviceProvider() {}

TangoVRDeviceProvider::~TangoVRDeviceProvider() {}

void TangoVRDeviceProvider::GetDevices(std::vector<VRDevice*>* devices) {
  if (!tango_device_) {
    tango_device_.reset(new TangoVRDevice(this));
  }

  devices->push_back(tango_device_.get());
}

void TangoVRDeviceProvider::Initialize() {
  // No initialization needed for Tango devices.
}

}  // namespace device
