// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VRPointCloud_h
#define VRPointCloud_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/DOMTypedArray.h"
#include "device/vr/vr_service.mojom-blink.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class VRPointCloud final : public GarbageCollected<VRPointCloud>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    VRPointCloud();

    unsigned int numberOfPoints() const;
    DOMFloat32Array* points() const;

    void setPointCloud(device::mojom::blink::VRPointCloudPtr& pointCloudPtr);

    DECLARE_VIRTUAL_TRACE()

private:
    unsigned long m_numberOfPoints;
    unsigned long m_lastNumberOfPoints;
    Member<DOMFloat32Array> m_points;
};

} // namespace blink

#endif // VRPointCloud_h
