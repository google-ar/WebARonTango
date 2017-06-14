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
using tango_chromium::ADF;

namespace device {

TangoVRDevice::TangoVRDevice(TangoVRDeviceProvider* provider)
    : tangoVRDeviceProvider(provider) {
  tangoCoordinateFramePair.base = TANGO_COORDINATE_FRAME_START_OF_SERVICE;
  tangoCoordinateFramePair.target = TANGO_COORDINATE_FRAME_DEVICE;
}

TangoVRDevice::~TangoVRDevice() {
}

mojom::VRDisplayInfoPtr TangoVRDevice::GetVRDevice() {
  TRACE_EVENT0("input", "TangoVRDevice::GetVRDevice");
  mojom::VRDisplayInfoPtr device = mojom::VRDisplayInfo::New();

  device->displayName = "Tango VR Device";

  device->capabilities = mojom::VRDisplayCapabilities::New();
  device->capabilities->hasOrientation = true;
  device->capabilities->hasPosition = true;
  device->capabilities->hasExternalDisplay = false;
  device->capabilities->canPresent = false;
  device->capabilities->hasPointCloud = true;
  device->capabilities->hasSeeThroughCamera = true;
  device->capabilities->hasADFSupport = true;

  device->leftEye = mojom::VREyeParameters::New();
  device->rightEye = mojom::VREyeParameters::New();
  mojom::VREyeParametersPtr& left_eye = device->leftEye;
  mojom::VREyeParametersPtr& right_eye = device->rightEye;

  left_eye->fieldOfView = mojom::VRFieldOfView::New();
  left_eye->fieldOfView->upDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[0]*/;
  left_eye->fieldOfView->downDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[1]*/;
  left_eye->fieldOfView->leftDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[2]*/;
  left_eye->fieldOfView->rightDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[3]*/;

  // Cardboard devices always assume a mirrored FOV, so this is just the left
  // eye FOV with the left and right degrees swapped.
  right_eye->fieldOfView = mojom::VRFieldOfView::New();
  right_eye->fieldOfView->upDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[0]*/;
  right_eye->fieldOfView->downDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[1]*/;
  right_eye->fieldOfView->leftDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[3]*/;
  right_eye->fieldOfView->rightDegrees = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*fov[2]*/;

  left_eye->offset.resize(3);
  left_eye->offset[0] = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*ipd*/ * -0.5f;
  left_eye->offset[1] = 0.0f;
  left_eye->offset[2] = 0.0f;

  right_eye->offset.resize(3);
  right_eye->offset[0] = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*ipd*/ * 0.5f;
  right_eye->offset[1] = 0.0f;
  right_eye->offset[2] = 0.0f;

  left_eye->renderWidth = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*screen_size[0]*/ / 2.0;
  left_eye->renderHeight = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*screen_size[1]*/;

  right_eye->renderWidth = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*screen_size[0]*/ / 2.0;
  right_eye->renderHeight = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*screen_size[1]*/;

  // device->stageParameters = mojom::VRStageParameters::New();
  // device->stageParameters->standingTransform = mojo::Array<float>::New(16);

  return device;
}

mojom::VRPosePtr TangoVRDevice::GetPose() {

  TangoPoseData tangoPoseData;

  mojom::VRPosePtr pose = nullptr;
  bool isLocalized = false;

  if (TangoHandler::getInstance()->isConnected() && TangoHandler::getInstance()->getPose(&tangoPoseData, &isLocalized))
  {
    pose = mojom::VRPose::New();

    pose->timestamp = base::Time::Now().ToJsTime();

    pose->orientation.emplace(4);
    pose->position.emplace(3);
    pose->localized = isLocalized;

    pose->orientation.value()[0] = tangoPoseData.orientation[0]/*decomposed_transform.quaternion[0]*/;
    pose->orientation.value()[1] = tangoPoseData.orientation[1]/*decomposed_transform.quaternion[1]*/;
    pose->orientation.value()[2] = tangoPoseData.orientation[2]/*decomposed_transform.quaternion[2]*/;
    pose->orientation.value()[3] = tangoPoseData.orientation[3]/*decomposed_transform.quaternion[3]*/;

    pose->position.value()[0] = tangoPoseData.translation[0]/*decomposed_transform.translate[0]*/;
    pose->position.value()[1] = tangoPoseData.translation[1]/*decomposed_transform.translate[1]*/;
    pose->position.value()[2] = tangoPoseData.translation[2]/*decomposed_transform.translate[2]*/;
  }

  return pose;
}

void TangoVRDevice::ResetPose() {
  // TODO
}

unsigned TangoVRDevice::GetMaxNumberOfPointsInPointCloud()
{
  return TangoHandler::getInstance()->getMaxNumberOfPointsInPointCloud();
}

mojom::VRPointCloudPtr TangoVRDevice::GetPointCloud(bool justUpdatePointCloud, unsigned pointsToSkip, bool transformPoints)
{
  TangoHandler* tangoHandler = TangoHandler::getInstance();
  mojom::VRPointCloudPtr pointCloudPtr = nullptr;
  if (tangoHandler->isConnected())
  {
    if (!justUpdatePointCloud)
    {
      pointCloudPtr = mojom::VRPointCloud::New();
      pointCloudPtr->points.resize(tangoHandler->getMaxNumberOfPointsInPointCloud() * 3);
      pointCloudPtr->pointsTransformMatrix.resize(16);
      if (!tangoHandler->getPointCloud(&(pointCloudPtr->numberOfPoints), &(pointCloudPtr->points[0]), justUpdatePointCloud, pointsToSkip, transformPoints, &(pointCloudPtr->pointsTransformMatrix[0])))
      {
        pointCloudPtr = nullptr;
      }
      pointCloudPtr->pointsAlreadyTransformed = transformPoints;
    }
    else 
    {
      // If the point cloud should only be updated, why create a whole array?
      uint32_t numberOfPoints;
      tangoHandler->getPointCloud(&numberOfPoints, 0, justUpdatePointCloud, pointsToSkip, transformPoints, 0);
    }
  }
  return pointCloudPtr;
}

mojom::VRSeeThroughCameraPtr TangoVRDevice::GetSeeThroughCamera()
{
  TangoHandler* tangoHandler = TangoHandler::getInstance();
  mojom::VRSeeThroughCameraPtr seeThroughCameraPtr = nullptr;
  if (tangoHandler->isConnected())
  {
    seeThroughCameraPtr = mojom::VRSeeThroughCamera::New();
    tangoHandler->getCameraImageSize(&(seeThroughCameraPtr->width), &(seeThroughCameraPtr->height));
    tangoHandler->getCameraImageTextureSize(&(seeThroughCameraPtr->textureWidth), &(seeThroughCameraPtr->textureHeight));
    tangoHandler->getCameraFocalLength(&(seeThroughCameraPtr->focalLengthX), &(seeThroughCameraPtr->focalLengthY));
    tangoHandler->getCameraPoint(&(seeThroughCameraPtr->pointX), &(seeThroughCameraPtr->pointY));
    seeThroughCameraPtr->orientation = tangoHandler->getSensorOrientation();
  }
  return seeThroughCameraPtr;
}

mojom::VRPickingPointAndPlanePtr TangoVRDevice::GetPickingPointAndPlaneInPointCloud(float x, float y)
{
  mojom::VRPickingPointAndPlanePtr pickingPointAndPlanePtr = nullptr;
  if (TangoHandler::getInstance()->isConnected())
  {
    pickingPointAndPlanePtr = mojom::VRPickingPointAndPlane::New();
    pickingPointAndPlanePtr->point = std::vector<double>(3);
    pickingPointAndPlanePtr->plane = std::vector<double>(4);
    if (!TangoHandler::getInstance()->getPickingPointAndPlaneInPointCloud(x, y, &(pickingPointAndPlanePtr->point[0]), &(pickingPointAndPlanePtr->plane[0])))
    {
      pickingPointAndPlanePtr = nullptr;
    }
  }
  return pickingPointAndPlanePtr;
}

std::vector<mojom::VRADFPtr> TangoVRDevice::GetADFs()
{
  std::vector<mojom::VRADFPtr> mojomADFs;
  if (TangoHandler::getInstance()->isConnected())
  {
    std::vector<ADF> adfs;
    if (TangoHandler::getInstance()->getADFs(adfs))
    {
      std::vector<ADF>::size_type size = adfs.size();
      mojomADFs.resize(size);
      for (std::vector<ADF>::size_type i = 0; i < size; i++)
      {
        mojomADFs[i] = mojom::VRADF::New();
        mojomADFs[i]->uuid = adfs[i].getUUID();
        mojomADFs[i]->name = adfs[i].getName();
        mojomADFs[i]->creationTime = adfs[i].getCreationTime();
      }
    }
  }
  return mojomADFs;
}

void TangoVRDevice::EnableADF(const std::string& uuid)
{
  TangoHandler::getInstance()->enableADF(uuid);
}

void TangoVRDevice::DisableADF()
{
  TangoHandler::getInstance()->disableADF();
}

void TangoVRDevice::RequestPresent(const base::Callback<void(bool)>& callback) {
  // gvr_provider_->RequestPresent(callback);
}

void TangoVRDevice::SetSecureOrigin(bool secure_origin) {
  // secure_origin_ = secure_origin;
  // if (delegate_)
  //   delegate_->SetWebVRSecureOrigin(secure_origin_);
}

void TangoVRDevice::ExitPresent() {
  // gvr_provider_->ExitPresent();
  // OnExitPresent();
}

void TangoVRDevice::SubmitFrame(mojom::VRPosePtr pose) {
  // if (delegate_)
  //   delegate_->SubmitWebVRFrame();
}

void TangoVRDevice::UpdateLayerBounds(mojom::VRLayerBoundsPtr left_bounds,
                                  mojom::VRLayerBoundsPtr right_bounds) {
  // if (!delegate_)
  //   return;

  // gvr::Rectf left_gvr_bounds;
  // left_gvr_bounds.left = left_bounds->left;
  // left_gvr_bounds.top = 1.0f - left_bounds->top;
  // left_gvr_bounds.right = left_bounds->left + left_bounds->width;
  // left_gvr_bounds.bottom = 1.0f - (left_bounds->top + left_bounds->height);

  // gvr::Rectf right_gvr_bounds;
  // right_gvr_bounds.left = right_bounds->left;
  // right_gvr_bounds.top = 1.0f - right_bounds->top;
  // right_gvr_bounds.right = right_bounds->left + right_bounds->width;
  // right_gvr_bounds.bottom = 1.0f - (right_bounds->top + right_bounds->height);

  // delegate_->UpdateWebVRTextureBounds(left_gvr_bounds, right_gvr_bounds);
}

}  // namespace device
