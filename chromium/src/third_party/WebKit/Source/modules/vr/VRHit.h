// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VRHit_h
#define VRHit_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/DOMTypedArray.h"
#include "device/vr/vr_service.mojom-blink.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class VRHit final : public GarbageCollected<VRHit>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    VRHit();
    
    DOMFloat32Array* modelMatrix() const { return m_modelMatrix; }

    void setHit(const device::mojom::blink::VRHitPtr&);

    DECLARE_VIRTUAL_TRACE();

private:
    Member<DOMFloat32Array> m_modelMatrix;
};

} // namespace blink

#endif // VRHit_h
