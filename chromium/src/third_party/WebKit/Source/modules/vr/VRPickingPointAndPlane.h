// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VRPickingPointAndPlane_h
#define VRPickingPointAndPlane_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/DOMTypedArray.h"
#include "device/vr/vr_service.mojom-blink.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class VRPickingPointAndPlane final : public GarbageCollected<VRPickingPointAndPlane>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    VRPickingPointAndPlane();
    
    DOMFloat32Array* point() const { return m_point; }
    DOMFloat32Array* plane() const { return m_plane; }

    void setPickingPointAndPlane(const device::mojom::blink::VRPickingPointAndPlanePtr&);

    DECLARE_VIRTUAL_TRACE();

private:
    Member<DOMFloat32Array> m_point;
    Member<DOMFloat32Array> m_plane;
};

} // namespace blink

#endif // VRPickingPointAndPlane_h
