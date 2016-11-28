// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/vr/VRController.h"

#include "core/frame/LocalFrame.h"
#include "modules/vr/VRGetDevicesCallback.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/mojo/MojoHelper.h"
#include "public/platform/ServiceRegistry.h"

namespace blink {

VRController::~VRController()
{
}

void VRController::provideTo(LocalFrame& frame, ServiceRegistry* registry)
{
    ASSERT(RuntimeEnabledFeatures::webVREnabled());
    Supplement<LocalFrame>::provideTo(frame, supplementName(), registry ? new VRController(frame, registry) : nullptr);
}

VRController* VRController::from(LocalFrame& frame)
{
    return static_cast<VRController*>(Supplement<LocalFrame>::from(frame, supplementName()));
}

VRController::VRController(LocalFrame& frame, ServiceRegistry* registry)
{
    ASSERT(!m_service.is_bound());
    registry->connectToRemoteService(mojo::GetProxy(&m_service));
}

const char* VRController::supplementName()
{
    return "VRController";
}

void VRController::getDisplays(std::unique_ptr<VRGetDevicesCallback> callback)
{
    if (!m_service) {
        callback->onError();
        return;
    }

    m_pendingGetDevicesCallbacks.append(std::move(callback));
    m_service->GetDisplays(convertToBaseCallback(WTF::bind(&VRController::onGetDisplays, wrapPersistent(this))));
}

device::blink::VRPosePtr VRController::getPose(unsigned index)
{
    if (!m_service)
        return nullptr;

    device::blink::VRPosePtr pose;
    m_service->GetPose(index, &pose);
    return pose;
}

unsigned VRController::getMaxPointCloudVertexCount(unsigned index)
{
    if (!m_service)
        return 0;

    unsigned result;
    m_service->GetMaxPointCloudVertexCount(index, &result);
    return result;
}

device::blink::VRPointCloudPtr VRController::getPointCloud(unsigned index, bool justUpdatePointCloud)
{
    if (!m_service)
        return nullptr;

    device::blink::VRPointCloudPtr pointCloud;
    m_service->GetPointCloud(index, justUpdatePointCloud, &pointCloud);
    return pointCloud;
}

device::blink::VRPickingPointAndPlanePtr VRController::getPickingPointAndPlaneInPointCloud(unsigned index, float x, float y)
{
    if (!m_service)
        return nullptr;

    device::blink::VRPickingPointAndPlanePtr pickingPointAndPlane;
    m_service->GetPickingPointAndPlaneInPointCloud(index, x, y, &pickingPointAndPlane);
    return pickingPointAndPlane;
}

device::blink::VRSeeThroughCameraPtr VRController::getSeeThroughCamera(unsigned index)
{
    if (!m_service)
        return nullptr;

    device::blink::VRSeeThroughCameraPtr seeThroughCamera;
    m_service->GetSeeThroughCamera(index, &seeThroughCamera);
    return seeThroughCamera;
}

mojo::WTFArray<float> VRController::getPoseMatrix(unsigned index)
{
    if (!m_service)
        return nullptr;

    mojo::WTFArray<float> poseMatrix;
    m_service->GetPoseMatrix(index, &poseMatrix);
    return poseMatrix;
}

int VRController::getSeeThroughCameraOrientation(unsigned index)
{
    if (!m_service)
        return 0;

    int32_t result;
    m_service->GetSeeThroughCameraOrientation(index, &result);
    return static_cast<int>(result);
}

void VRController::resetPose(unsigned index)
{
    if (!m_service)
        return;
    m_service->ResetPose(index);
}

void VRController::onGetDisplays(mojo::WTFArray<device::blink::VRDisplayPtr> displays)
{
    std::unique_ptr<VRGetDevicesCallback> callback = m_pendingGetDevicesCallbacks.takeFirst();
    if (!callback)
        return;

    callback->onSuccess(std::move(displays));
}

DEFINE_TRACE(VRController)
{
    Supplement<LocalFrame>::trace(visitor);
}

} // namespace blink
