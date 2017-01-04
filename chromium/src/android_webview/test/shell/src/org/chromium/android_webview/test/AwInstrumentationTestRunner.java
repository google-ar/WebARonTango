// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.android_webview.test;

import org.chromium.base.test.BaseChromiumInstrumentationTestRunner;
import org.chromium.base.test.BaseTestResult;
import org.chromium.policy.test.annotations.Policies;

/**
 * Instrumentation test runner that allows integrating features defined above base layer
 * for webview testing.
 */
public class AwInstrumentationTestRunner extends BaseChromiumInstrumentationTestRunner {
    @Override
    protected void addTestHooks(BaseTestResult result) {
        super.addTestHooks(result);

        result.addPreTestHook(Policies.getRegistrationHook());
    }
}
