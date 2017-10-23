// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/vr/android/tango/tango_vr_device.h"

#include "tango_support_api.h"

#include "base/trace_event/trace_event.h"

#include "TangoHandler.h"

#define THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API -1

const float RAD_2_DEG = 180.0 / M_PI;

using base::android::AttachCurrentThread;
using tango_chromium::TangoHandler;
using tango_chromium::ADF;
using tango_chromium::Marker;
using tango_chromium::Hit;

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
  device->capabilities->hasPassThroughCamera = true;
  device->capabilities->hasADFSupport = true;
  device->capabilities->hasMarkerSupport = true;

  device->leftEye = mojom::VREyeParameters::New();
  device->rightEye = mojom::VREyeParameters::New();
  mojom::VREyeParametersPtr& left_eye = device->leftEye;
  mojom::VREyeParametersPtr& right_eye = device->rightEye;
  left_eye->fieldOfView = mojom::VRFieldOfView::New();
  right_eye->fieldOfView = mojom::VRFieldOfView::New();

  left_eye->offset.resize(3);
  right_eye->offset.resize(3);

  TangoHandler* tangoHandler = TangoHandler::getInstance();
  if (!tangoHandler->isConnected()) {
    // We may not be able to get an instance of TangoHandler right away, so
    // stub in some data till we have one.
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
    left_eye->offset[2] = -0.0;

    right_eye->offset[0] = 0.0;
    right_eye->offset[1] = 0.0;
    right_eye->offset[2] = 0.0;

    left_eye->renderWidth = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*screen_size[0]*/ / 2.0;
    left_eye->renderHeight = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*screen_size[1]*/;

    right_eye->renderWidth = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*screen_size[0]*/ / 2.0;
    right_eye->renderHeight = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API/*screen_size[1]*/;

    return device;
  }

  uint32_t iw = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API;
  uint32_t ih = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API;
  double fx = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API;
  double fy = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API;
  double cx = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API;
  double cy = THIS_VALUE_NEEDS_TO_BE_OBTAINED_FROM_THE_TANGO_API;

  tangoHandler->getCameraImageSize(&iw, &ih);
  tangoHandler->getCameraFocalLength(&fx, &fy);
  tangoHandler->getCameraPoint(&cx, &cy);

  float vDegrees = atan(ih / (2.0 * fy)) * RAD_2_DEG;
  float hDegrees = atan(iw / (2.0 * fx)) * RAD_2_DEG;

  left_eye->fieldOfView->upDegrees = vDegrees;
  left_eye->fieldOfView->downDegrees = vDegrees;
  left_eye->fieldOfView->leftDegrees = hDegrees;
  left_eye->fieldOfView->rightDegrees = hDegrees;

  right_eye->fieldOfView->upDegrees = vDegrees;
  right_eye->fieldOfView->downDegrees = vDegrees;
  right_eye->fieldOfView->leftDegrees = hDegrees;
  right_eye->fieldOfView->rightDegrees = hDegrees;

  left_eye->offset[0] = 0.0f;
  left_eye->offset[1] = 0.0f;
  left_eye->offset[2] = 0.0f;

  right_eye->offset[0] = 0.0f;
  right_eye->offset[1] = 0.0f;
  right_eye->offset[2] = 0.0f;

  left_eye->renderWidth = iw;
  left_eye->renderHeight = ih;

  right_eye->renderWidth = iw;
  right_eye->renderHeight = ih;

  // Store the orientation values so we can check in future GetPose()
  // calls if we need to update camera intrinsics and regenerate the
  // VRDeviceInfoPtr
  lastSensorOrientation = tangoHandler->getSensorOrientation();
  lastActivityOrientation = tangoHandler->getActivityOrientation();

  return device;
}

mojom::VRPosePtr TangoVRDevice::GetPose() {

  // Check to see if orientation has changed, and if so, fire
  // an OnChanged() so that the VRFieldOfView can be updated,
  // with the up-to-date VRDeviceInfoPtr sent to WebKit for correct
  // projection matrix calculations.
  TangoHandler* tangoHandler = TangoHandler::getInstance();
  if (tangoHandler->isConnected() &&
      (lastSensorOrientation  != tangoHandler->getSensorOrientation() ||
      lastActivityOrientation != tangoHandler->getActivityOrientation())) {
    VRDevice::OnChanged();
  }

  TangoPoseData tangoPoseData;

  mojom::VRPosePtr pose = nullptr;
  bool isLocalized = false;

  if (TangoHandler::getInstance()->isConnected() && TangoHandler::getInstance()->getPose(&tangoPoseData, &isLocalized))
  {
    pose = mojom::VRPose::New();

    pose->timestamp = base::Time::Now().ToJsTime();
    pose->localized = isLocalized;

    pose->orientation.emplace(4);
    pose->position.emplace(3);

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
  TangoHandler::getInstance()->resetPose();
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

mojom::VRPassThroughCameraPtr TangoVRDevice::GetPassThroughCamera()
{
  TangoHandler* tangoHandler = TangoHandler::getInstance();
  mojom::VRPassThroughCameraPtr passThroughCameraPtr = nullptr;
  if (tangoHandler->isConnected())
  {
    passThroughCameraPtr = mojom::VRPassThroughCamera::New();
    tangoHandler->getCameraImageSize(&(passThroughCameraPtr->width), &(passThroughCameraPtr->height));
    tangoHandler->getCameraImageTextureSize(&(passThroughCameraPtr->textureWidth), &(passThroughCameraPtr->textureHeight));
    tangoHandler->getCameraFocalLength(&(passThroughCameraPtr->focalLengthX), &(passThroughCameraPtr->focalLengthY));
    tangoHandler->getCameraPoint(&(passThroughCameraPtr->pointX), &(passThroughCameraPtr->pointY));
    passThroughCameraPtr->orientation = tangoHandler->getSensorOrientation();
  }
  return passThroughCameraPtr;
}

std::vector<mojom::VRHitPtr> TangoVRDevice::HitTest(float x, float y)
{
  std::vector<mojom::VRHitPtr> mojomHits;
  if (TangoHandler::getInstance()->isConnected())
  {
    std::vector<Hit> hits;
    if (TangoHandler::getInstance()->hitTest(x, y, hits) && hits.size() > 0)
    {
      std::vector<Hit>::size_type size = hits.size();
      mojomHits.resize(size);
      for (std::vector<Hit>::size_type i = 0; i < size; i++)
      {
        mojomHits[i] = mojom::VRHit::New();
        mojomHits[i]->modelMatrix.resize(16);
        for (int j = 0; j < 16; j++)
        {
          mojomHits[i]->modelMatrix[j] = hits[i].modelMatrix[j];
        }
      }
    }
  }
  return mojomHits;
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

std::vector<mojom::VRMarkerPtr> TangoVRDevice::GetMarkers(unsigned markerType, float markerSize)
{
  std::vector<mojom::VRMarkerPtr> mojomMarkers;
  if (TangoHandler::getInstance()->isConnected())
  {
    TangoSupportMarkerType mt;
    switch(markerType)
    {
      case 0x1:
        mt = TANGO_MARKER_ARTAG;
        break;
      case 0x2:
        mt = TANGO_MARKER_QRCODE;
        break;
      default:
        VLOG(0) << "ERROR: Incorrect marker type value. Currently supported values are VRDipslay.MARKER_TYPE_AR and VRDisplay.MARKER_TYPE_QRCODE.";
        return mojomMarkers;
    }
    std::vector<Marker> markers;
    if (TangoHandler::getInstance()->getMarkers(mt, markerSize, markers))
    {
      std::vector<Marker>::size_type size = markers.size();
      mojomMarkers.resize(size);
      for (std::vector<Marker>::size_type i = 0; i < size; i++)
      {
        mojomMarkers[i] = mojom::VRMarker::New();
        mojomMarkers[i]->type = markers[i].getType();
        mojomMarkers[i]->id = markers[i].getId();
        mojomMarkers[i]->content = markers[i].getContent();
        mojomMarkers[i]->position.resize(3);
        const double* markerPosition = markers[i].getPosition(); 
        mojomMarkers[i]->position[0] = markerPosition[0];
        mojomMarkers[i]->position[1] = markerPosition[1];
        mojomMarkers[i]->position[2] = markerPosition[2];
        mojomMarkers[i]->orientation.resize(4);
        const double* markerOrientation = markers[i].getOrientation(); 
        mojomMarkers[i]->orientation[0] = markerOrientation[0];
        mojomMarkers[i]->orientation[1] = markerOrientation[1];
        mojomMarkers[i]->orientation[2] = markerOrientation[2];
        mojomMarkers[i]->orientation[3] = markerOrientation[3];
      }
    }
  }
  return mojomMarkers;
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
