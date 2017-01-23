// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/vr/vr_service_impl.h"

#include "base/memory/ptr_util.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "device/vr/test/fake_vr_device.h"
#include "device/vr/test/fake_vr_device_provider.h"
#include "device/vr/test/fake_vr_service_client.h"
#include "device/vr/vr_device_manager.h"
#include "device/vr/vr_service.mojom.h"
#include "device/vr/vr_service_impl.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace device {

class VRServiceImplTest : public testing::Test {
 public:
  VRServiceImplTest() {}
  ~VRServiceImplTest() override {}
  void onDisplaySynced(unsigned int number_of_devices) {}

 protected:
  void SetUp() override {
    provider_ = new FakeVRDeviceProvider();
    device_manager_.reset(new VRDeviceManager(base::WrapUnique(provider_)));
  }

  std::unique_ptr<VRServiceImpl> BindService() {
    mojom::VRServiceClientPtr proxy;
    FakeVRServiceClient client(mojo::MakeRequest(&proxy));
    auto service = base::WrapUnique(new VRServiceImpl());
    service->SetClient(std::move(proxy),
                       base::Bind(&VRServiceImplTest::onDisplaySynced,
                                  base::Unretained(this)));
    return service;
  }

  size_t ServiceCount() { return device_manager_->services_.size(); }

 private:
  base::MessageLoop message_loop_;
  FakeVRDeviceProvider* provider_;
  std::unique_ptr<VRDeviceManager> device_manager_;

  DISALLOW_COPY_AND_ASSIGN(VRServiceImplTest);
};

// Ensure that services are registered with the device manager as they are
// created and removed from the device manager as their connections are closed.
TEST_F(VRServiceImplTest, DeviceManagerRegistration) {
  EXPECT_EQ(0u, ServiceCount());

  auto service_1 = BindService();

  EXPECT_EQ(1u, ServiceCount());

  auto service_2 = BindService();

  EXPECT_EQ(2u, ServiceCount());

  service_1.reset();

  EXPECT_EQ(1u, ServiceCount());

  service_2.reset();

  EXPECT_EQ(0u, ServiceCount());
}

}
