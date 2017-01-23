// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_VR_ANDROID_FAKE_VR_DEVICE_H
#define DEVICE_VR_ANDROID_FAKE_VR_DEVICE_H

#include "base/macros.h"
#include "device/vr/vr_device.h"

namespace device {

class FakeVRDeviceProvider;

class FakeVRDevice : public VRDevice {
 public:
  FakeVRDevice(FakeVRDeviceProvider* provider);
  ~FakeVRDevice() override;

  // VRDevice
  mojom::VRDisplayInfoPtr GetVRDevice() override;
  mojom::VRPosePtr GetPose() override;
  void ResetPose() override;

  void RequestPresent(const base::Callback<void(bool)>& callback) override;
  void SetSecureOrigin(bool secure_origin) override;
  void ExitPresent() override;

  void SubmitFrame(mojom::VRPosePtr pose) override;
  void UpdateLayerBounds(mojom::VRLayerBoundsPtr left_bounds,
                         mojom::VRLayerBoundsPtr right_bounds) override;

 private:
  FakeVRDeviceProvider* fake_vr_provider_;

  DISALLOW_COPY_AND_ASSIGN(FakeVRDevice);
};

}  // namespace device

#endif  // DEVICE_VR_ANDROID_GVR_DEVICE_H
