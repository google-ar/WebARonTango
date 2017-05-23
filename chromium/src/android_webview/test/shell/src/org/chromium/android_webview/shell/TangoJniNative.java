/*
 * Copyright 2016 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *            http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.chromium.android_webview.shell;

import android.app.Activity;
import android.os.IBinder;
import android.util.Log;

/**
 * Interfaces between C and Java.
 *
 * Note that these are the functions that call into native code, native code is
 * responsible for the communication between the application and Tango Service.
 */
public class TangoJniNative {

    public static boolean initialize()
    {
        // This project depends on tango_client_api, so we need to make sure we load
        // the correct library first.
        if (TangoInitializationHelper.loadTangoSharedLibrary() ==
                TangoInitializationHelper.ARCH_ERROR) {
            Log.e("TangoJNINative", "ERROR! Unable to load libtango_client_api.so!");
            return false;
        }
        System.loadLibrary("tango_chromium");
        return true;
    }

    /**
     * Check if the Tango Core version is compatible with this app.
     * If not, the application will exit.
     *
     * @param callerActivity the caller activity of this function.
     */
    public static native void onCreate(Activity callerActivity, int activityOrientation, int sensorOrientation);

    /*
     * Called when the Tango service is connected.
     *
     * @param binder The native binder object.
     */
    public static native void onTangoServiceConnected(IBinder binder);

    /**
     * Disconnect and stop Tango service.
     */
    public static native void onPause();

    public static native void onDestroy();

    public static native void onConfigurationChanged(int activityOrientation, int sensorOrientation);
}
