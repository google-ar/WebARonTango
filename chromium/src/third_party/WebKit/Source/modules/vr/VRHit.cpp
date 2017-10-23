// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/vr/VRHit.h"

namespace blink {

VRHit::VRHit()
{
    m_modelMatrix = DOMFloat32Array::create(16);
}

void VRHit::setHit(const device::mojom::blink::VRHitPtr& hitPtr)
{
    if (hitPtr.is_null())
        return;

    for (size_t i = 0; i < 16; i++) {
    	m_modelMatrix->data()[i] = (float)hitPtr->modelMatrix[i];
    }
}

DEFINE_TRACE(VRHit)
{
    visitor->trace(m_modelMatrix);
}

} // namespace blink
