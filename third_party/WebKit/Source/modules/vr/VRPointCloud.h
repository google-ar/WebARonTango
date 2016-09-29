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

    unsigned int vertexCount() const;
    DOMFloat32Array* vertices() const;

    void setPointCloud(device::blink::VRPointCloudPtr& pointCloudPtr);

    DECLARE_VIRTUAL_TRACE()

private:
    unsigned long m_vertexCount;
    unsigned long m_lastVertexCount;
    Member<DOMFloat32Array> m_vertices;
};

} // namespace blink

#endif // VRPointCloud_h
