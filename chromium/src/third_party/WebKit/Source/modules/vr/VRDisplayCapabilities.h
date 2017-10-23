// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VRDisplayCapabilities_h
#define VRDisplayCapabilities_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class VRDisplayCapabilities final
    : public GarbageCollected<VRDisplayCapabilities>,
      public ScriptWrappable {
  DEFINE_WRAPPERTYPEINFO();

 public:
  VRDisplayCapabilities();

  bool hasOrientation() const { return m_hasOriention; }
  bool hasPosition() const { return m_hasPosition; }
  bool hasExternalDisplay() const { return m_hasExternalDisplay; }
  bool canPresent() const { return m_canPresent; }
  unsigned maxLayers() const { return m_maxLayers; }

  bool hasPointCloud() const { return m_hasPointCloud; }
  bool hasPassThroughCamera() const { return m_hasPassThroughCamera; }
  bool hasADFSupport() const { return m_hasADFSupport; }
  bool hasMarkerSupport() const { return m_hasMarkerSupport; }

  void setHasOrientation(bool value) { m_hasOriention = value; }
  void setHasPosition(bool value) { m_hasPosition = value; }
  void setHasExternalDisplay(bool value) { m_hasExternalDisplay = value; }
  void setCanPresent(bool value) { m_canPresent = value; }
  void setMaxLayers(unsigned value) { m_maxLayers = value; }

  void setHasPointCloud(bool value) { m_hasPointCloud = value; }
  void setHasPassThroughCamera(bool value) { m_hasPassThroughCamera = value; }
  void setHasADFSupport(bool value) { m_hasADFSupport = value; }
  void setHasMarkerSupport(bool value) { m_hasMarkerSupport = value; }

  DECLARE_VIRTUAL_TRACE()

 private:
  bool m_hasOriention;
  bool m_hasPosition;
  bool m_hasExternalDisplay;
  bool m_canPresent;
  unsigned m_maxLayers;

  bool m_hasPointCloud;
  bool m_hasPassThroughCamera;
  bool m_hasADFSupport;
  bool m_hasMarkerSupport;
};

}  // namespace blink

#endif  // VRDisplayCapabilities_h
