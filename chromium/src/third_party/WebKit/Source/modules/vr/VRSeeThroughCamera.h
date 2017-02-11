// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VRSeeThroughCamera_h
#define VRSeeThroughCamera_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "device/vr/vr_service.mojom-blink.h"

namespace blink {

class VRSeeThroughCamera final : public GarbageCollected<VRSeeThroughCamera>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    VRSeeThroughCamera();

    unsigned long width() const;
    unsigned long height() const;
    unsigned long textureWidth() const;
    unsigned long textureHeight() const;
    double focalLengthX() const;
    double focalLengthY() const;
    double pointX() const;
    double pointY() const;
    long orientation();

    void setSeeThroughCamera(const device::mojom::blink::VRSeeThroughCameraPtr&);

    DECLARE_VIRTUAL_TRACE()
private:
	unsigned long m_width;
	unsigned long m_height;
	unsigned long m_textureWidth;
	unsigned long m_textureHeight;
    double m_focalLengthX;
    double m_focalLengthY;
    double m_pointX;
    double m_pointY;
    long m_orientation;
};

} // namespace blink

#endif // VRSeeThroughCamera_h
