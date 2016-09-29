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
{
}

unsigned long VRSeeThroughCamera::width()
{
	return m_width;
}

unsigned long VRSeeThroughCamera::height()
{
	return m_height;
}

unsigned long VRSeeThroughCamera::textureWidth()
{
	return m_textureWidth;
}

unsigned long VRSeeThroughCamera::textureHeight()
{
	return m_textureHeight;
}

double VRSeeThroughCamera::focalLengthX()
{
	return m_focalLengthX;
}

double VRSeeThroughCamera::focalLengthY()
{
	return m_focalLengthY;
}

double VRSeeThroughCamera::pointX()
{
	return m_pointX;
}

double VRSeeThroughCamera::pointY()
{
	return m_pointY;
}

void VRSeeThroughCamera::setSeeThroughCamera(const device::blink::VRSeeThroughCameraPtr& seeThroughCameraPtr)
{
	m_width = seeThroughCameraPtr->width;
	m_height = seeThroughCameraPtr->height;
	m_textureWidth = seeThroughCameraPtr->textureWidth;
	m_textureHeight = seeThroughCameraPtr->textureHeight;
	m_focalLengthX = seeThroughCameraPtr->focalLengthX;
	m_focalLengthY = seeThroughCameraPtr->focalLengthY;
	m_pointX = seeThroughCameraPtr->pointX;
	m_pointY = seeThroughCameraPtr->pointY;
}

DEFINE_TRACE(VRSeeThroughCamera)
{
}

} // namespace blink
