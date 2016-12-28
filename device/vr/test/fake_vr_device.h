// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_VR_TEST_FAKE_VR_DEVICE_H_
#define DEVICE_VR_TEST_FAKE_VR_DEVICE_H_

#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "device/vr/vr_device.h"
#include "device/vr/vr_device_provider.h"
#include "device/vr/vr_service_impl.h"

namespace device {

class FakeVRDevice : public VRDevice {
 public:
  explicit FakeVRDevice();
  ~FakeVRDevice() override;

  void InitBasicDevice();

  void SetVRDevice(const mojom::VRDisplayInfoPtr& device);
  void SetPose(const mojom::VRPosePtr& state);

  mojom::VRDisplayInfoPtr GetVRDevice() override;
  mojom::VRPosePtr GetPose() override;
  void ResetPose() override;

  void RequestPresent(const base::Callback<void(bool)>& callback) override;
  void SetSecureOrigin(bool secure_origin) override;
  void ExitPresent() override;
  void SubmitFrame(mojom::VRPosePtr pose) override;
  void UpdateLayerBounds(mojom::VRLayerBoundsPtr leftBounds,
                         mojom::VRLayerBoundsPtr rightBounds) override;

 private:
  mojom::VREyeParametersPtr InitEye(float fov, float offset, uint32_t size);

  mojom::VRDisplayInfoPtr device_;
  mojom::VRPosePtr pose_;

  DISALLOW_COPY_AND_ASSIGN(FakeVRDevice);
};

}  // namespace device

#endif  // DEVICE_VR_TEST_FAKE_VR_DEVICE_H_
