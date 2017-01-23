// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/vr/vr_service_impl.h"

#include <utility>

#include "base/bind.h"
#include "base/memory/ptr_util.h"
#include "device/vr/vr_device.h"
#include "device/vr/vr_device_manager.h"
#include "mojo/public/cpp/bindings/strong_binding.h"

namespace device {

VRServiceImpl::VRServiceImpl() : listening_for_activate_(false) {}

VRServiceImpl::~VRServiceImpl() {
  // Destroy VRDisplay before calling RemoveService below. RemoveService might
  // implicitly trigger destory VRDevice which VRDisplay needs to access in its
  // dtor.
  displays_.clear();
  VRDeviceManager::GetInstance()->RemoveService(this);
}

void VRServiceImpl::Create(mojo::InterfaceRequest<mojom::VRService> request) {
  mojo::MakeStrongBinding(base::MakeUnique<VRServiceImpl>(),
                          std::move(request));
}

// Gets a VRDisplayPtr unique to this service so that the associated page can
// communicate with the VRDevice.
VRDisplayImpl* VRServiceImpl::GetVRDisplayImpl(VRDevice* device) {
  auto it = displays_.find(device);
  if (it != displays_.end())
    return it->second.get();

  VRDisplayImpl* display_impl = new VRDisplayImpl(device, this);
  displays_[device] = base::WrapUnique(display_impl);
  return display_impl;
}

void VRServiceImpl::RemoveDevice(VRDevice* device) {
  displays_.erase(device);
}

void VRServiceImpl::SetClient(mojom::VRServiceClientPtr service_client,
                              const SetClientCallback& callback) {
  DCHECK(!client_.get());
  client_ = std::move(service_client);
  VRDeviceManager* device_manager = VRDeviceManager::GetInstance();
  // Once a client has been connected AddService will force any VRDisplays to
  // send OnConnected to it so that it's populated with the currently active
  // displays. Thereafer it will stay up to date by virtue of listening for new
  // connected events.
  device_manager->AddService(this);
  callback.Run(device_manager->GetNumberOfConnectedDevices());
}

void VRServiceImpl::SetListeningForActivate(bool listening) {
  listening_for_activate_ = listening;
  VRDeviceManager* device_manager = VRDeviceManager::GetInstance();
  device_manager->ListeningForActivateChanged(listening);
}

}  // namespace device
