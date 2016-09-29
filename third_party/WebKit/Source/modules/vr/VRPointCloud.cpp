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

VRPointCloud::VRPointCloud(): m_vertexCount(0), m_lastVertexCount(0)
{
}

unsigned int VRPointCloud::vertexCount() const
{
    return m_vertexCount;
}

DOMFloat32Array* VRPointCloud::vertices() const
{
    return m_vertices;
}

void VRPointCloud::setPointCloud(device::blink::VRPointCloudPtr& pointCloudPtr)
{
	if (!pointCloudPtr->vertices && pointCloudPtr->vertexCount == 0)
	{
		if (m_vertices)
		{
			std::fill_n(m_vertices->data(), m_lastVertexCount * 3, std::numeric_limits<float>::max());
			// memset(m_vertices->data(), FLT_MAX, m_lastVertexCount * 3 * sizeof(float));
		}
		m_vertexCount = m_lastVertexCount = 0;
	}
	else 
	{
		if (!m_vertices)
		{
			m_vertices = DOMFloat32Array::create(pointCloudPtr->vertices.size());
			std::fill_n(m_vertices->data(), pointCloudPtr->vertices.size(), std::numeric_limits<float>::max());
			// memset(m_vertices->data(), FLT_MAX, pointCloudPtr->vertices.size() * sizeof(float));
		}

		m_vertexCount = pointCloudPtr->vertexCount;
		memcpy(m_vertices->data(), &(pointCloudPtr->vertices.front()), m_vertexCount * 3 * sizeof(float));
		if (m_vertexCount < m_lastVertexCount)
		{
			std::fill_n(m_vertices->data() + (m_vertexCount * 3), (m_lastVertexCount - m_vertexCount) * 3, std::numeric_limits<float>::max());
			// memset(m_vertices->data() + (m_vertexCount * 3), FLT_MAX, (m_lastVertexCount - m_vertexCount) * 3 * sizeof(float));
		}
		m_lastVertexCount = m_vertexCount;
	}
}


DEFINE_TRACE(VRPointCloud)
{
    visitor->trace(m_vertices);
}

} // namespace blink
