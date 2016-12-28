// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/vr/VRPickingPointAndPlane.h"

namespace blink {

VRPickingPointAndPlane::VRPickingPointAndPlane(): m_correctValues(false)
{
    m_point = DOMFloat32Array::create(3);
    m_plane = DOMFloat32Array::create(4);
}

void VRPickingPointAndPlane::setPickingPointAndPlane(const device::mojom::blink::VRPickingPointAndPlanePtr& pickingPointAndPlanePtr)
{
    m_correctValues = false;

    if (pickingPointAndPlanePtr.is_null())
        return;

    for (size_t i = 0; i < 3; i++) {
    	m_point->data()[i] = (float)pickingPointAndPlanePtr->point[i];
    }
    for (size_t i = 0; i < 4; i++) {
    	m_plane->data()[i] = (float)pickingPointAndPlanePtr->plane[i];
    }
    m_correctValues = true;
}

DEFINE_TRACE(VRPickingPointAndPlane)
{
    visitor->trace(m_point);
    visitor->trace(m_plane);
}

} // namespace blink
