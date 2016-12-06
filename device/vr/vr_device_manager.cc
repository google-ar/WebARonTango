// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/vr/vr_device_manager.h"

#include <utility>

#include "base/bind.h"
#include "base/memory/singleton.h"
#include "build/build_config.h"

#if defined(OS_ANDROID)
#include "device/vr/android/cardboard/cardboard_vr_device_provider.h"
#include "device/vr/android/tango/tango_vr_device_provider.h"
#endif

namespace device {

namespace {
VRDeviceManager* g_vr_device_manager = nullptr;
}

VRDeviceManager::VRDeviceManager()
    : vr_initialized_(false), keep_alive_(false) {
  bindings_.set_connection_error_handler(
      base::Bind(&VRDeviceManager::OnConnectionError, base::Unretained(this)));
// Register VRDeviceProviders for the current platform
#if defined(OS_ANDROID)
  std::unique_ptr<VRDeviceProvider> tango_provider(
      new TangoVRDeviceProvider());
  RegisterProvider(std::move(tango_provider));
  std::unique_ptr<VRDeviceProvider> cardboard_provider(
      new CardboardVRDeviceProvider());
  RegisterProvider(std::move(cardboard_provider));
#endif
}

VRDeviceManager::VRDeviceManager(std::unique_ptr<VRDeviceProvider> provider)
    : vr_initialized_(false), keep_alive_(true) {
  thread_checker_.DetachFromThread();
  RegisterProvider(std::move(provider));
  SetInstance(this);
}

VRDeviceManager::~VRDeviceManager() {
  DCHECK(thread_checker_.CalledOnValidThread());
  g_vr_device_manager = nullptr;
}

void VRDeviceManager::BindRequest(mojo::InterfaceRequest<VRService> request) {
  VRDeviceManager* device_manager = GetInstance();
  device_manager->bindings_.AddBinding(device_manager, std::move(request));
}

void VRDeviceManager::OnConnectionError() {
  DCHECK(thread_checker_.CalledOnValidThread());
  if (bindings_.empty() && !keep_alive_) {
    // Delete the device manager when it has no active connections.
    delete g_vr_device_manager;
  }
}

VRDeviceManager* VRDeviceManager::GetInstance() {
  if (!g_vr_device_manager)
    g_vr_device_manager = new VRDeviceManager();
  return g_vr_device_manager;
}

void VRDeviceManager::SetInstance(VRDeviceManager* instance) {
  // Unit tests can create multiple instances but only one should exist at any
  // given time so g_vr_device_manager should only go from nullptr to
  // non-nullptr and vica versa.
  CHECK_NE(!!instance, !!g_vr_device_manager);
  g_vr_device_manager = instance;
}

bool VRDeviceManager::HasInstance() {
  // For testing. Checks to see if a VRDeviceManager instance is active.
  return !!g_vr_device_manager;
}

mojo::Array<VRDisplayPtr> VRDeviceManager::GetVRDevices() {
  DCHECK(thread_checker_.CalledOnValidThread());

  InitializeProviders();

  std::vector<VRDevice*> devices;
  for (const auto& provider : providers_)
    provider->GetDevices(&devices);

  mojo::Array<VRDisplayPtr> out_devices;
  for (const auto& device : devices) {
    if (device->id() == VR_DEVICE_LAST_ID)
      continue;

    if (devices_.find(device->id()) == devices_.end())
      devices_[device->id()] = device;

    VRDisplayPtr vr_device_info = device->GetVRDevice();
    if (vr_device_info.is_null())
      continue;

    vr_device_info->index = device->id();
    out_devices.push_back(std::move(vr_device_info));
  }

  return out_devices;
}

VRDevice* VRDeviceManager::GetDevice(unsigned int index) {
  DCHECK(thread_checker_.CalledOnValidThread());

  if (index == 0) {
    return NULL;
  }

  DeviceMap::iterator iter = devices_.find(index);
  if (iter == devices_.end()) {
    return nullptr;
  }
  return iter->second;
}

void VRDeviceManager::InitializeProviders() {
  if (vr_initialized_) {
    return;
  }

  for (const auto& provider : providers_)
    provider->Initialize();

  vr_initialized_ = true;
}

void VRDeviceManager::RegisterProvider(
    std::unique_ptr<VRDeviceProvider> provider) {
  providers_.push_back(make_linked_ptr(provider.release()));
}

void VRDeviceManager::GetDisplays(const GetDisplaysCallback& callback) {
  callback.Run(GetVRDevices());
}

void VRDeviceManager::GetPose(uint32_t index, const GetPoseCallback& callback) {
  VRDevice* device = GetDevice(index);
  if (device) {
    callback.Run(device->GetPose());
  } else {
    callback.Run(nullptr);
  }
}

void VRDeviceManager::ResetPose(uint32_t index) {
  VRDevice* device = GetDevice(index);
  if (device)
    device->ResetPose();
}

void VRDeviceManager::GetMaxPointCloudVertexCount(uint32_t index, const GetMaxPointCloudVertexCountCallback& callback) 
{
  VRDevice* device = GetDevice(index);
  if (device) {
    callback.Run(device->GetMaxPointCloudVertexCount());
  } else {
    callback.Run(0);
  }
}

void VRDeviceManager::GetPointCloud(uint32_t index, bool justUpdatePointCloud, unsigned pointsToSkip, const GetPointCloudCallback& callback) {
  VRDevice* device = GetDevice(index);
  if (device) {
    callback.Run(device->GetPointCloud(justUpdatePointCloud, pointsToSkip));
  } else {
    callback.Run(nullptr);
  }
}

void VRDeviceManager::GetPickingPointAndPlaneInPointCloud(uint32_t index, float x, float y, const GetPickingPointAndPlaneInPointCloudCallback& callback)
{
  VRDevice* device = GetDevice(index);
  if (device) {
    callback.Run(device->GetPickingPointAndPlaneInPointCloud(x, y));
  } else {
    callback.Run(nullptr);
  }
}

void VRDeviceManager::GetSeeThroughCamera(uint32_t index, const GetSeeThroughCameraCallback& callback) {
  VRDevice* device = GetDevice(index);
  if (device) {
    callback.Run(device->GetSeeThroughCamera());
  } else {
    callback.Run(nullptr);
  }
}

void VRDeviceManager::GetPoseMatrix(uint32_t index, const GetPoseMatrixCallback& callback) {
  VRDevice* device = GetDevice(index);
  if (device) {
    callback.Run(device->GetPoseMatrix());
  } else {
    callback.Run(nullptr);
  }
}

void VRDeviceManager::GetSeeThroughCameraOrientation(uint32_t index, const GetSeeThroughCameraOrientationCallback& callback) {
  VRDevice* device = GetDevice(index);
  if (device) {
    callback.Run(device->GetSeeThroughCameraOrientation());
  } else {
    callback.Run(0);
  }
}

}  // namespace device
