// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/vr/VRSeeThroughCamera.h"

namespace blink {

VRSeeThroughCamera::VRSeeThroughCamera(): m_width(0)
	, m_height(0)
	, m_textureWidth(0)
	, m_textureHeight(0)
	, m_focalLengthX(0)
	, m_focalLengthY(0)
	, m_pointX(0)
	, m_pointY(0)
	, m_orientation(0)
{
}

unsigned long VRSeeThroughCamera::width() const
{
	return m_width;
}

unsigned long VRSeeThroughCamera::height() const
{
	return m_height;
}

unsigned long VRSeeThroughCamera::textureWidth() const
{
	return m_textureWidth;
}

unsigned long VRSeeThroughCamera::textureHeight() const
{
	return m_textureHeight;
}

double VRSeeThroughCamera::focalLengthX() const
{
	return m_focalLengthX;
}

double VRSeeThroughCamera::focalLengthY() const
{
	return m_focalLengthY;
}

double VRSeeThroughCamera::pointX() const
{
	return m_pointX;
}

double VRSeeThroughCamera::pointY() const
{
	return m_pointY;
}

long VRSeeThroughCamera::orientation()
{
	return m_orientation;
}

void VRSeeThroughCamera::setSeeThroughCamera(const device::mojom::blink::VRSeeThroughCameraPtr& seeThroughCameraPtr)
{
	m_width = seeThroughCameraPtr->width;
	m_height = seeThroughCameraPtr->height;
	m_textureWidth = seeThroughCameraPtr->textureWidth;
	m_textureHeight = seeThroughCameraPtr->textureHeight;
	m_focalLengthX = seeThroughCameraPtr->focalLengthX;
	m_focalLengthY = seeThroughCameraPtr->focalLengthY;
	m_pointX = seeThroughCameraPtr->pointX;
	m_pointY = seeThroughCameraPtr->pointY;
	m_orientation = seeThroughCameraPtr->orientation;
}

DEFINE_TRACE(VRSeeThroughCamera)
{
}

} // namespace blink
