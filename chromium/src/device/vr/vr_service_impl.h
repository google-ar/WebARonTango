// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVICE_VR_VR_SERVICE_IMPL_H
#define DEVICE_VR_VR_SERVICE_IMPL_H

#include <memory>

#include "base/macros.h"

#include "device/vr/vr_device.h"
#include "device/vr/vr_display_impl.h"
#include "device/vr/vr_export.h"
#include "device/vr/vr_service.mojom.h"
#include "mojo/public/cpp/bindings/binding.h"

namespace device {

class VRServiceImpl : public mojom::VRService {
 public:
  DEVICE_VR_EXPORT VRServiceImpl();
  DEVICE_VR_EXPORT ~VRServiceImpl() override;

  DEVICE_VR_EXPORT static void Create(
      mojo::InterfaceRequest<mojom::VRService> request);

  mojom::VRServiceClient* client() { return client_.get(); }

  DEVICE_VR_EXPORT VRDisplayImpl* GetVRDisplayImpl(VRDevice* device);

  // mojom::VRService implementation
  void SetClient(mojom::VRServiceClientPtr service_client,
                 const SetClientCallback& callback) override;

  bool listening_for_activate() { return listening_for_activate_; }

 private:
  friend class FakeVRServiceClient;
  friend class VRDeviceManagerTest;
  friend class VRDisplayImpl;
  friend class VRDisplayImplTest;
  friend class VRServiceImplTest;

  void RemoveDevice(VRDevice* device);

  void SetListeningForActivate(bool listening) override;

  std::map<VRDevice*, std::unique_ptr<VRDisplayImpl>> displays_;

  mojom::VRServiceClientPtr client_;

  bool listening_for_activate_;

  DISALLOW_COPY_AND_ASSIGN(VRServiceImpl);
};

}  // namespace device

#endif  // DEVICE_VR_VR_SERVICE_IMPL_H
