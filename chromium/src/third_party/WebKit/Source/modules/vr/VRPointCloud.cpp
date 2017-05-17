// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/vr/VRPointCloud.h"

#include <float.h>

namespace blink {

namespace {

// DOMFloat32Array* mojoArrayToFloat32Array(const mojo::WTFArray<float>& vec, unsigned size)
// {
//     if (vec.is_null())
//         return nullptr;

//     return DOMFloat32Array::create(&(vec.front()), size);
// }

} // namespace

VRPointCloud::VRPointCloud(): m_numberOfPoints(0), m_lastNumberOfPoints(0)
{
}

unsigned int VRPointCloud::numberOfPoints() const
{
    return m_numberOfPoints;
}

DOMFloat32Array* VRPointCloud::points() const
{
    return m_points;
}

void VRPointCloud::setPointCloud(unsigned maxNumberOfPoints, device::mojom::blink::VRPointCloudPtr& pointCloudPtr)
{
	if (!m_points)
	{
		m_points = DOMFloat32Array::create(maxNumberOfPoints * 3);
		std::fill_n(m_points->data(), m_numberOfPoints, std::numeric_limits<float>::max());
	}
	if (pointCloudPtr.is_null())
	{
		m_numberOfPoints = 0;
	}
	else
	{
		m_numberOfPoints = pointCloudPtr->numberOfPoints;
		if (m_numberOfPoints > 0) {
			memcpy(m_points->data(), &(pointCloudPtr->points.front()), m_numberOfPoints * 3 * sizeof(float));
		}
	}
	if (m_numberOfPoints < m_lastNumberOfPoints)
	{
		std::fill_n(m_points->data() + (m_numberOfPoints * 3), (m_lastNumberOfPoints - m_numberOfPoints) * 3, std::numeric_limits<float>::max());
	}
	m_lastNumberOfPoints = m_numberOfPoints;
}

DEFINE_TRACE(VRPointCloud)
{
    visitor->trace(m_points);
}

} // namespace blink
