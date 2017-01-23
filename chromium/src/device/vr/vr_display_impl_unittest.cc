// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/vr/vr_display_impl.h"

#include "base/memory/ptr_util.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "device/vr/test/fake_vr_device.h"
#include "device/vr/test/fake_vr_device_provider.h"
#include "device/vr/test/fake_vr_display_impl_client.h"
#include "device/vr/test/fake_vr_service_client.h"
#include "device/vr/vr_device_manager.h"
#include "device/vr/vr_service.mojom.h"
#include "device/vr/vr_service_impl.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace device {

class VRDisplayImplTest : public testing::Test {
 public:
  VRDisplayImplTest() {}
  ~VRDisplayImplTest() override {}
  void onDisplaySynced(unsigned int number_of_devices) {}
  void onPresentComplete(bool success) {
    is_request_presenting_success_ = success;
  }

 protected:
  void SetUp() override {
    provider_ = new FakeVRDeviceProvider();
    device_ = new FakeVRDevice();
    provider_->AddDevice(base::WrapUnique(device_));
    device_manager_.reset(new VRDeviceManager(base::WrapUnique(provider_)));
  }

  std::unique_ptr<VRServiceImpl> BindService() {
    mojom::VRServiceClientPtr proxy;
    clients_.push_back(new FakeVRServiceClient(mojo::MakeRequest(&proxy)));

    auto service = base::WrapUnique(new VRServiceImpl());
    service->SetClient(std::move(proxy),
                       base::Bind(&VRDisplayImplTest::onDisplaySynced,
                                  base::Unretained(this)));
    return service;
  }

  void RequestPresent(VRDisplayImpl* display_impl) {
    display_impl->RequestPresent(
        true, base::Bind(&VRDisplayImplTest::onPresentComplete,
                         base::Unretained(this)));
  }

  void ExitPresent(VRDisplayImpl* display_impl) { display_impl->ExitPresent(); }

  void TearDown() override { base::RunLoop().RunUntilIdle(); }

  VRDevice* device() { return device_; }

  bool presenting() { return !!device_->presenting_display_; }

  base::MessageLoop message_loop_;
  bool is_request_presenting_success_ = false;
  FakeVRDeviceProvider* provider_;
  FakeVRDevice* device_;
  std::vector<FakeVRServiceClient*> clients_;
  std::unique_ptr<VRDeviceManager> device_manager_;

  DISALLOW_COPY_AND_ASSIGN(VRDisplayImplTest);
};

TEST_F(VRDisplayImplTest, DevicePresentationIsolation) {
  auto service_1 = BindService();
  auto service_2 = BindService();

  VRDisplayImpl* display_1 = service_1->GetVRDisplayImpl(device());
  VRDisplayImpl* display_2 = service_2->GetVRDisplayImpl(device());

  // When not presenting either service should be able to access the device.
  EXPECT_TRUE(device()->IsAccessAllowed(display_1));
  EXPECT_TRUE(device()->IsAccessAllowed(display_2));

  // Begin presenting to the fake device with service 1.
  RequestPresent(display_1);
  EXPECT_TRUE(is_request_presenting_success_);
  EXPECT_TRUE(presenting());

  // Service 2 should not be able to present to the device while service 1
  // is still presenting.
  RequestPresent(display_2);
  EXPECT_FALSE(is_request_presenting_success_);
  EXPECT_TRUE(device()->IsAccessAllowed(display_1));
  EXPECT_FALSE(device()->IsAccessAllowed(display_2));

  // Service 2 should not be able to exit presentation to the device.
  ExitPresent(display_2);
  EXPECT_TRUE(presenting());

  // Service 1 should be able to exit the presentation it initiated.
  ExitPresent(display_1);
  EXPECT_FALSE(presenting());

  // Once presentation had ended both services should be able to access the
  // device.
  EXPECT_TRUE(device()->IsAccessAllowed(display_1));
  EXPECT_TRUE(device()->IsAccessAllowed(display_2));
}

// This test case tests VRDevice class default behaviour when it
// dispatch "vrdevicechanged" event. The expected behaviour is all
// of the services related with this device will receive "vrdevicechanged"
// event.
TEST_F(VRDisplayImplTest, DeviceChangedDispatched) {
  auto service_1 = BindService();
  auto service_2 = BindService();

  device()->OnChanged();

  base::RunLoop().RunUntilIdle();

  for (auto client : clients_)
    EXPECT_TRUE(client->CheckDeviceId(device()->id()));
}
}
