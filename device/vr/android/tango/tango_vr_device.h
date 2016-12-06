// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_VR_TANGO_VR_DEVICE_H
#define DEVICE_VR_TANGO_VR_DEVICE_H

#include <jni.h>

#include "base/android/jni_android.h"
#include "base/macros.h"
#include "device/vr/vr_device.h"

#include "tango_client_api.h"

namespace device {

class TangoVRDevice : public VRDevice {
 public:
  explicit TangoVRDevice(VRDeviceProvider* provider);
  ~TangoVRDevice() override;

  VRDisplayPtr GetVRDevice() override;
  VRPosePtr GetPose() override;
  void ResetPose() override;
  unsigned GetMaxPointCloudVertexCount() override;
  VRPointCloudPtr GetPointCloud(bool justUpdatePointCloud, unsigned pointsToSkip) override;
  VRSeeThroughCameraPtr GetSeeThroughCamera() override;
  VRPickingPointAndPlanePtr GetPickingPointAndPlaneInPointCloud(float x, float y) override;
  mojo::Array<float> GetPoseMatrix() override;
  int GetSeeThroughCameraOrientation() override;

 private:
  DISALLOW_COPY_AND_ASSIGN(TangoVRDevice);
  TangoCoordinateFramePair tangoCoordinateFramePair;  
};

}  // namespace device

#endif  // DEVICE_VR_TANGO_VR_DEVICE_H
