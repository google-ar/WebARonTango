/*
 * Copyright 2016 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jni.h>

#include "TangoHandler.h"

using namespace tango_chromium;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_org_chromium_android_1webview_shell_TangoJniNative_onCreate(JNIEnv* env, jobject obj, jobject caller_activity, jint activityOrientation, jint sensorOrientation) 
{
	TangoHandler::getInstance()->onCreate(env, caller_activity, activityOrientation, sensorOrientation);
}

JNIEXPORT void JNICALL
Java_org_chromium_android_1webview_shell_TangoJniNative_onDestroy(JNIEnv* env, jobject obj) 
{
	TangoHandler::releaseInstance();
}

JNIEXPORT void JNICALL
Java_org_chromium_android_1webview_shell_TangoJniNative_onTangoServiceConnected(JNIEnv* env, jobject, jobject iBinder) 
{
	TangoHandler::getInstance()->onTangoServiceConnected(env, iBinder);
}

JNIEXPORT void JNICALL
Java_org_chromium_android_1webview_shell_TangoJniNative_onPause(JNIEnv*, jobject) 
{
	TangoHandler::getInstance()->onPause();
}

JNIEXPORT void JNICALL
Java_org_chromium_android_1webview_shell_TangoJniNative_onConfigurationChanged(JNIEnv*, jobject, int activityOrientation, int sensorOrientation) 
{
	TangoHandler::getInstance()->onDeviceRotationChanged(activityOrientation, sensorOrientation);
}

JNIEXPORT void JNICALL
Java_org_chromium_android_1webview_shell_TangoJniNative_resetPose(JNIEnv*, jobject, int activityOrientation, int sensorOrientation) 
{
  TangoHandler::getInstance()->resetPose();
}

#ifdef __cplusplus
}
#endif
