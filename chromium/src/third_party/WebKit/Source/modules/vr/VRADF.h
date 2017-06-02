// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VRADF_h
#define VRADF_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "device/vr/vr_service.mojom-blink.h"
#include "wtf/text/WTFString.h"

namespace blink {

class VRADF final : public GarbageCollected<VRADF>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    VRADF();

    String uuid() const;
    String name() const;
    unsigned long creationTime() const;

    void setADF(const device::mojom::blink::VRADFPtr&);

    DECLARE_VIRTUAL_TRACE()
private:
    String m_uuid;
    String m_name;
	unsigned long m_creationTime;
};

} // namespace blink

#endif // VRADF_h
