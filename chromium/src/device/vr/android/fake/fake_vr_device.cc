// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/vr/android/fake/fake_vr_device.h"

#include "device/vr/android/fake/fake_vr_device_provider.h"
#include "base/trace_event/trace_event.h"

namespace device {

FakeVRDevice::FakeVRDevice(FakeVRDeviceProvider* provider)
    : VRDevice(), fake_vr_provider_(provider) {}

FakeVRDevice::~FakeVRDevice() {}

mojom::VRDisplayInfoPtr FakeVRDevice::GetVRDevice() {
  TRACE_EVENT0("input", "FakeVRDevice::GetVRDevice");

  mojom::VRDisplayInfoPtr device = mojom::VRDisplayInfo::New();

  device->index = id();

  device->capabilities = mojom::VRDisplayCapabilities::New();
  device->capabilities->hasOrientation = true;
  device->capabilities->hasPosition = false;
  device->capabilities->hasExternalDisplay = false;
  device->capabilities->canPresent = true;

  device->leftEye = mojom::VREyeParameters::New();
  device->rightEye = mojom::VREyeParameters::New();
  mojom::VREyeParametersPtr& left_eye = device->leftEye;
  mojom::VREyeParametersPtr& right_eye = device->rightEye;

  left_eye->fieldOfView = mojom::VRFieldOfView::New();
  right_eye->fieldOfView = mojom::VRFieldOfView::New();

  left_eye->offset.resize(3);
  right_eye->offset.resize(3);

  // Set the render target size to "invalid" to indicate that
  // we can't render into it yet. Other code uses this to check
  // for valid state.
  left_eye->renderWidth = 0 / 2;
  left_eye->renderHeight = 0;

  right_eye->renderWidth = left_eye->renderWidth;
  right_eye->renderHeight = left_eye->renderHeight;

  // We may not be able to get an instance of GvrApi right away, so
  // stub in some data till we have one.
  device->displayName = "Fake VR Device";

  left_eye->fieldOfView->upDegrees = 45;
  left_eye->fieldOfView->downDegrees = 45;
  left_eye->fieldOfView->leftDegrees = 45;
  left_eye->fieldOfView->rightDegrees = 45;

  right_eye->fieldOfView->upDegrees = 45;
  right_eye->fieldOfView->downDegrees = 45;
  right_eye->fieldOfView->leftDegrees = 45;
  right_eye->fieldOfView->rightDegrees = 45;

  left_eye->offset[0] = -0.0;
  left_eye->offset[1] = -0.0;
  left_eye->offset[2] = -0.03;

  right_eye->offset[0] = 0.0;
  right_eye->offset[1] = 0.0;
  right_eye->offset[2] = 0.03;

  return device;
}

mojom::VRPosePtr FakeVRDevice::GetPose() {

  return nullptr;
}

void FakeVRDevice::ResetPose() {
}

void FakeVRDevice::RequestPresent(const base::Callback<void(bool)>& callback) {
}

void FakeVRDevice::SetSecureOrigin(bool secure_origin) {
}

void FakeVRDevice::ExitPresent() {
}

void FakeVRDevice::SubmitFrame(mojom::VRPosePtr pose) {
}

void FakeVRDevice::UpdateLayerBounds(mojom::VRLayerBoundsPtr left_bounds,
                                  mojom::VRLayerBoundsPtr right_bounds) {
}

}  // namespace device
