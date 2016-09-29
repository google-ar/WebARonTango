// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/vr/android/tango/tango_vr_device.h"

#include "tango_support_api.h"

#include "base/trace_event/trace_event.h"

#include "TangoHandler.h"

#define THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API -1

using base::android::AttachCurrentThread;
using tango_chromium::TangoHandler;

namespace device {

TangoVRDevice::TangoVRDevice(VRDeviceProvider* provider)
    : VRDevice(provider) {
  tangoCoordinateFramePair.base = TANGO_COORDINATE_FRAME_START_OF_SERVICE;
  tangoCoordinateFramePair.target = TANGO_COORDINATE_FRAME_DEVICE;
}

TangoVRDevice::~TangoVRDevice() {
}

VRDisplayPtr TangoVRDevice::GetVRDevice() {
  TRACE_EVENT0("input", "TangoVRDevice::GetVRDevice");
  VRDisplayPtr device = VRDisplay::New();

  device->displayName = "Tango VR Device";

  device->capabilities = VRDisplayCapabilities::New();
  device->capabilities->hasOrientation = true;
  device->capabilities->hasPosition = true;
  device->capabilities->hasExternalDisplay = false;
  device->capabilities->canPresent = false;
  device->capabilities->hasPointCloud = true;
  device->capabilities->hasSeeThroughCamera = true;

  device->leftEye = VREyeParameters::New();
  device->rightEye = VREyeParameters::New();
  VREyeParametersPtr& left_eye = device->leftEye;
  VREyeParametersPtr& right_eye = device->rightEye;

  left_eye->fieldOfView = VRFieldOfView::New();
  left_eye->fieldOfView->upDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[0]*/;
  left_eye->fieldOfView->downDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[1]*/;
  left_eye->fieldOfView->leftDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[2]*/;
  left_eye->fieldOfView->rightDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[3]*/;

  // Cardboard devices always assume a mirrored FOV, so this is just the left
  // eye FOV with the left and right degrees swapped.
  right_eye->fieldOfView = VRFieldOfView::New();
  right_eye->fieldOfView->upDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[0]*/;
  right_eye->fieldOfView->downDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[1]*/;
  right_eye->fieldOfView->leftDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[3]*/;
  right_eye->fieldOfView->rightDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[2]*/;

  left_eye->offset = mojo::Array<float>::New(3);
  left_eye->offset[0] = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*ipd*/ * -0.5f;
  left_eye->offset[1] = 0.0f;
  left_eye->offset[2] = 0.0f;

  right_eye->offset = mojo::Array<float>::New(3);
  right_eye->offset[0] = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*ipd*/ * 0.5f;
  right_eye->offset[1] = 0.0f;
  right_eye->offset[2] = 0.0f;

  left_eye->renderWidth = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*screen_size[0]*/ / 2.0;
  left_eye->renderHeight = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*screen_size[1]*/;

  right_eye->renderWidth = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*screen_size[0]*/ / 2.0;
  right_eye->renderHeight = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*screen_size[1]*/;

  device->stageParameters = VRStageParameters::New();
  device->stageParameters->standingTransform = mojo::Array<float>::New(16);

  return device;
}

VRPosePtr TangoVRDevice::GetPose() {

  TangoPoseData tangoPoseData;

  bool connected = TangoHandler::getInstance()->getPose(&tangoPoseData);

  TRACE_EVENT0("input", "TangoVRDevice::GetPose");
  
  VRPosePtr pose = VRPose::New();

  pose->timestamp = base::Time::Now().ToJsTime();

  if (connected)
  {
    pose->orientation = mojo::Array<float>::New(4);
    pose->orientation[0] = tangoPoseData.orientation[0]/*decomposed_transform.quaternion[0]*/;
    pose->orientation[1] = tangoPoseData.orientation[1]/*decomposed_transform.quaternion[1]*/;
    pose->orientation[2] = tangoPoseData.orientation[2]/*decomposed_transform.quaternion[2]*/;
    pose->orientation[3] = tangoPoseData.orientation[3]/*decomposed_transform.quaternion[3]*/;

    pose->position = mojo::Array<float>::New(3);
    pose->position[0] = tangoPoseData.translation[0]/*decomposed_transform.translate[0]*/;
    pose->position[1] = tangoPoseData.translation[1]/*decomposed_transform.translate[1]*/;
    pose->position[2] = tangoPoseData.translation[2]/*decomposed_transform.translate[2]*/;
  }
  else 
  {
    pose->orientation = nullptr;
    pose->position = nullptr;
  }

  return pose;
}

void TangoVRDevice::ResetPose() {
  // TODO
}

unsigned TangoVRDevice::GetMaxPointCloudVertexCount()
{
  return TangoHandler::getInstance()->getMaxPointCloudVertexCount();
}

VRPointCloudPtr TangoVRDevice::GetPointCloud()
{
  TangoHandler* tangoHandler = TangoHandler::getInstance();
  VRPointCloudPtr pointCloudPtr;
  pointCloudPtr = VRPointCloud::New();
  pointCloudPtr->vertices = mojo::Array<float>::New(tangoHandler->getMaxPointCloudVertexCount() * 3);
  if (!tangoHandler->getPointCloud(&(pointCloudPtr->vertexCount), &(pointCloudPtr->vertices[0])))
  {
    pointCloudPtr->vertices = nullptr;
  }
  return pointCloudPtr;
}

VRSeeThroughCameraPtr TangoVRDevice::GetSeeThroughCamera()
{
  TangoHandler* tangoHandler = TangoHandler::getInstance();
  VRSeeThroughCameraPtr seeThroughCameraPtr;
  seeThroughCameraPtr = VRSeeThroughCamera::New();
  tangoHandler->getCameraImageSize(&(seeThroughCameraPtr->width), &(seeThroughCameraPtr->height));
  tangoHandler->getCameraImageTextureSize(&(seeThroughCameraPtr->textureWidth), &(seeThroughCameraPtr->textureHeight));
  tangoHandler->getCameraFocalLength(&(seeThroughCameraPtr->focalLengthX), &(seeThroughCameraPtr->focalLengthY));
  tangoHandler->getCameraPoint(&(seeThroughCameraPtr->pointX), &(seeThroughCameraPtr->pointY));
  return seeThroughCameraPtr;
}

VRPickingPointAndPlanePtr TangoVRDevice::GetPickingPointAndPlaneInPointCloud(float x, float y)
{
  VRPickingPointAndPlanePtr pickingPointAndPlanePtr;
  pickingPointAndPlanePtr = VRPickingPointAndPlane::New();
  pickingPointAndPlanePtr->point = mojo::Array<double>::New(3);
  pickingPointAndPlanePtr->plane = mojo::Array<double>::New(4);
  if (!TangoHandler::getInstance()->getPickingPointAndPlaneInPointCloud(x, y, &(pickingPointAndPlanePtr->point[0]), &(pickingPointAndPlanePtr->plane[0])))
  {
    return nullptr;
  }
  return pickingPointAndPlanePtr;
}

mojo::Array<float> TangoVRDevice::GetPoseMatrix() 
{
  mojo::Array<float> poseMatrix = mojo::Array<float>::New(16);
  if (!TangoHandler::getInstance()->getPoseMatrix(&(poseMatrix.front())))
  {
//    poseMatrix = nullptr;
  }
  return poseMatrix;
}

}  // namespace device
