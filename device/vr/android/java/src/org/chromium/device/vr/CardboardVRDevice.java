// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.device.vr;

import android.content.Context;

import com.google.vrtoolkit.cardboard.CardboardDeviceParams;
import com.google.vrtoolkit.cardboard.FieldOfView;
import com.google.vrtoolkit.cardboard.HeadMountedDisplay;
import com.google.vrtoolkit.cardboard.HeadMountedDisplayManager;
import com.google.vrtoolkit.cardboard.ScreenParams;
import com.google.vrtoolkit.cardboard.sensors.HeadTracker;

import org.chromium.base.annotations.CalledByNative;
import org.chromium.base.annotations.JNINamespace;

/**
 * This is the implementation of the C++ counterpart CardboardVRDevice.
 */
@JNINamespace("device")
class CardboardVRDevice {
    private static final String TAG = "CardboardVRDevice";
    private final HeadMountedDisplayManager mHMDManager;
    private final HeadTracker mHeadTracker;

    @CalledByNative
    private static CardboardVRDevice create(Context context) {
        return new CardboardVRDevice(context);
    }

    private CardboardVRDevice(Context context) {
        mHMDManager = new HeadMountedDisplayManager(context);

        mHeadTracker = HeadTracker.createFromContext(context);
        mHeadTracker.setNeckModelEnabled(true);
        mHeadTracker.startTracking();
    }

    @CalledByNative
    private void getFieldOfView(float[] outFov) {
        HeadMountedDisplay hmd = mHMDManager.getHeadMountedDisplay();
        CardboardDeviceParams deviceParams = hmd.getCardboardDeviceParams();
        FieldOfView fov = deviceParams.getLeftEyeMaxFov();
        outFov[0] = fov.getTop();
        outFov[1] = fov.getBottom();
        outFov[2] = fov.getLeft();
        outFov[3] = fov.getRight();
    }

    @CalledByNative
    private float getIpd() {
        HeadMountedDisplay hmd = mHMDManager.getHeadMountedDisplay();
        CardboardDeviceParams deviceParams = hmd.getCardboardDeviceParams();
        return deviceParams.getInterLensDistance();
    }

    @CalledByNative
    private String getDeviceName() {
        HeadMountedDisplay hmd = mHMDManager.getHeadMountedDisplay();
        CardboardDeviceParams deviceParams = hmd.getCardboardDeviceParams();
        return deviceParams.getVendor() + " " + deviceParams.getModel();
    }

    @CalledByNative
    private void getScreenSize(int[] outSize) {
        HeadMountedDisplay hmd = mHMDManager.getHeadMountedDisplay();
        ScreenParams screenParams = hmd.getScreenParams();
        outSize[0] = screenParams.getWidth();
        outSize[1] = screenParams.getHeight();
    }

    @CalledByNative
    private void getSensorState(float[] outMatrix) {
        mHeadTracker.getLastHeadView(outMatrix, 0);
    }

    @CalledByNative
    private void stopTracking() {
        mHeadTracker.stopTracking();
    }

    @CalledByNative
    private void resetSensor() {
        mHeadTracker.resetTracker();
    }
}
