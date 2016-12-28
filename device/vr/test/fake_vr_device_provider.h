// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_VR_TEST_FAKE_VR_DEVICE_PROVIDER_H_
#define DEVICE_VR_TEST_FAKE_VR_DEVICE_PROVIDER_H_

#include <vector>
#include "device/vr/vr_device.h"
#include "device/vr/vr_device_provider.h"

namespace device {

class FakeVRDeviceProvider : public VRDeviceProvider {
 public:
  FakeVRDeviceProvider();
  ~FakeVRDeviceProvider() override;

  // Adds devices to the provider with the given device, which will be
  // returned when GetDevices is queried.
  void AddDevice(std::unique_ptr<VRDevice> device);
  void RemoveDevice(std::unique_ptr<VRDevice> device);
  bool IsInitialized() { return initialized_; }

  void GetDevices(std::vector<VRDevice*>* devices) override;
  void Initialize() override;

 private:
  std::vector<std::unique_ptr<VRDevice>> devices_;
  bool initialized_;
};

}  // namespace device

#endif  // DEVICE_VR_TEST_FAKE_VR_DEVICE_PROVIDER_H_
