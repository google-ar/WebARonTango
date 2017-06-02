// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/vr/VRADF.h"

namespace blink {

VRADF::VRADF(): m_creationTime(0)
{
}

String VRADF::uuid() const
{
	return m_uuid;
}

String VRADF::name() const
{
	return m_name;
}

unsigned long VRADF::creationTime() const
{
	return m_creationTime;
}

void VRADF::setADF(const device::mojom::blink::VRADFPtr& adfPtr)
{
	m_name = adfPtr->name;
	m_uuid = adfPtr->uuid;
	m_creationTime = adfPtr->creationTime;
}

DEFINE_TRACE(VRADF)
{
}

} // namespace blink
