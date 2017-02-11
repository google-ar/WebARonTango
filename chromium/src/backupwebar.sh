# Copyright 2017 Google Inc. All Rights Reserved.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Get the branch name to know the output folder
BRANCH_NAME=$(git symbolic-ref -q HEAD)
BRANCH_NAME=${BRANCH_NAME##refs/heads/}
BRANCH_NAME=${BRANCH_NAME:-HEAD}
# Remove the backup folder for the branch
rm -rf ../Backup_WebAR/$BRANCH_NAME/*
# WebKit vr
mkdir -p ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/WebKit/Source/modules/vr
if [ $? -ne 0 ]; then exit 1; fi
cp third_party/WebKit/Source/modules/vr/VRController.* ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/WebKit/Source/modules/vr/
if [ $? -ne 0 ]; then exit 1; fi
cp third_party/WebKit/Source/modules/vr/VRDisplay.* ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/WebKit/Source/modules/vr/
if [ $? -ne 0 ]; then exit 1; fi
cp third_party/WebKit/Source/modules/vr/VRPointCloud.* ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/WebKit/Source/modules/vr/
if [ $? -ne 0 ]; then exit 1; fi
cp third_party/WebKit/Source/modules/vr/VRSeeThroughCamera.* ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/WebKit/Source/modules/vr/
if [ $? -ne 0 ]; then exit 1; fi
cp third_party/WebKit/Source/modules/vr/VRDisplayCapabilities.* ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/WebKit/Source/modules/vr/
if [ $? -ne 0 ]; then exit 1; fi
cp third_party/WebKit/Source/modules/vr/VRPickingPointAndPlane.* ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/WebKit/Source/modules/vr/
if [ $? -ne 0 ]; then exit 1; fi
cp third_party/WebKit/Source/modules/vr/BUILD.gn ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/WebKit/Source/modules/vr/
if [ $? -ne 0 ]; then exit 1; fi
cp third_party/WebKit/Source/modules/modules_idl_files.gni ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/WebKit/Source/modules
if [ $? -ne 0 ]; then exit 1; fi
# WebKit WebGL
mkdir -p ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/WebKit/Source/modules/webgl
if [ $? -ne 0 ]; then exit 1; fi
cp third_party/WebKit/Source/modules/webgl/WebGLRenderingContextBase.* ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/WebKit/Source/modules/webgl/
if [ $? -ne 0 ]; then exit 1; fi
cp third_party/WebKit/Source/modules/webgl/WebGL2RenderingContextBase.* ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/WebKit/Source/modules/webgl/
if [ $? -ne 0 ]; then exit 1; fi
# ThirdParty Tango
mkdir -p ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/tango
if [ $? -ne 0 ]; then exit 1; fi
cp -r third_party/tango ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/
if [ $? -ne 0 ]; then exit 1; fi
# ThirdParty ZXing
mkdir -p ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/zxing
if [ $? -ne 0 ]; then exit 1; fi
cp -r third_party/zxing ../Backup_WebAR/$BRANCH_NAME/chromium/src/third_party/
if [ $? -ne 0 ]; then exit 1; fi
# VR device Tango
# NOTE: Could copy only the elements that have been changed.
mkdir -p ../Backup_WebAR/$BRANCH_NAME/chromium/src/device/vr
if [ $? -ne 0 ]; then exit 1; fi
cp -r device/vr ../Backup_WebAR/$BRANCH_NAME/chromium/src/device/
if [ $? -ne 0 ]; then exit 1; fi
mkdir -p ../Backup_WebAR/$BRANCH_NAME/chromium/src/android_webview/test
if [ $? -ne 0 ]; then exit 1; fi
# GPU command buffer
mkdir -p ../Backup_WebAR/$BRANCH_NAME/chromium/src/gpu/command_buffer/service
if [ $? -ne 0 ]; then exit 1; fi
cp gpu/BUILD.gn ../Backup_WebAR/$BRANCH_NAME/chromium/src/gpu
if [ $? -ne 0 ]; then exit 1; fi
cp gpu/command_buffer/service/BUILD.gn ../Backup_WebAR/$BRANCH_NAME/chromium/src/gpu/command_buffer/service
if [ $? -ne 0 ]; then exit 1; fi
cp gpu/command_buffer/build_gles2_cmd_buffer.py ../Backup_WebAR/$BRANCH_NAME/chromium/src/gpu/command_buffer
if [ $? -ne 0 ]; then exit 1; fi
cp gpu/command_buffer/cmd_buffer_functions.txt ../Backup_WebAR/$BRANCH_NAME/chromium/src/gpu/command_buffer
if [ $? -ne 0 ]; then exit 1; fi
cp gpu/command_buffer/service/gles2_cmd_decoder.cc ../Backup_WebAR/$BRANCH_NAME/chromium/src/gpu/command_buffer/service
if [ $? -ne 0 ]; then exit 1; fi
cp gpu/command_buffer/service/gles2_cmd_decoder_passthrough_doer_prototypes.h ../Backup_WebAR/$BRANCH_NAME/chromium/src/gpu/command_buffer/service
if [ $? -ne 0 ]; then exit 1; fi
cp gpu/command_buffer/service/gles2_cmd_decoder_passthrough_doers.cc ../Backup_WebAR/$BRANCH_NAME/chromium/src/gpu/command_buffer/service
if [ $? -ne 0 ]; then exit 1; fi
# Android Chromium Webview
# NOTE: Could copy only the elements that have been changed.
cp -r android_webview/test/shell ../Backup_WebAR/$BRANCH_NAME/chromium/src/android_webview/test
if [ $? -ne 0 ]; then exit 1; fi
cp android_webview/test/BUILD.gn ../Backup_WebAR/$BRANCH_NAME/chromium/src/android_webview/test
if [ $? -ne 0 ]; then exit 1; fi
cp android_webview/BUILD.gn ../Backup_WebAR/$BRANCH_NAME/chromium/src/android_webview/
if [ $? -ne 0 ]; then exit 1; fi
# Also copy the suppressions.xml file as there is a lint warning that needs to be suppressed in the webview apk.
mkdir -p ../Backup_WebAR/$BRANCH_NAME/chromium/src/build/android/lint
if [ $? -ne 0 ]; then exit 1; fi
cp build/android/lint/suppressions.xml ../Backup_WebAR/$BRANCH_NAME/chromium/src/build/android/lint
if [ $? -ne 0 ]; then exit 1; fi
# Remove the temporary files
rm -rf ../Backup_WebAR/$BRANCH_NAME/chromium/src/android_webview/test/shell/tango/libs
if [ $? -ne 0 ]; then exit 1; fi
rm -rf ../Backup_WebAR/$BRANCH_NAME/chromium/src/android_webview/test/shell/tango/obj
if [ $? -ne 0 ]; then exit 1; fi
rm -rf ../Backup_WebAR/$BRANCH_NAME/chromium/src/android_webview/test/shell/tango/jni/objs
if [ $? -ne 0 ]; then exit 1; fi
rm -rf ../Backup_WebAR/$BRANCH_NAME/chromium/src/android_webview/test/shell/VRWebGL
if [ $? -ne 0 ]; then exit 1; fi
# APK
mkdir ../Backup_WebAR/$BRANCH_NAME/bin
if [ $? -ne 0 ]; then exit 1; fi
cp out/$BRANCH_NAME/apks/AndroidWebView.apk ../Backup_WebAR/$BRANCH_NAME/bin/ChromiumAR.apk
if [ $? -ne 0 ]; then exit 1; fi
# THREE.WebAR.js
mkdir ../Backup_WebAR/$BRANCH_NAME/THREE.WebAR
if [ $? -ne 0 ]; then exit 1; fi
cp ~/Coding/judax.github.io/webar/examples/libs/THREE.WebAR.js ../Backup_WebAR/$BRANCH_NAME/THREE.WebAR
if [ $? -ne 0 ]; then exit 1; fi
# Build script, notes, backup script, examples, ... 
cp build_install_run.sh ../Backup_WebAR/$BRANCH_NAME/chromium/src/
if [ $? -ne 0 ]; then exit 1; fi
# cp Notes*.txt ../Backup_WebAR/$BRANCH_NAME/chromium/src/
mkdir -p ../Backup_WebAR/$BRANCH_NAME/examples
if [ $? -ne 0 ]; then exit 1; fi
cp -r ~/Coding/judax.github.io/webar/examples/* ../Backup_WebAR/$BRANCH_NAME/examples
if [ $? -ne 0 ]; then exit 1; fi
cp ./backupwebar.sh ../Backup_WebAR/$BRANCH_NAME/chromium/src
if [ $? -ne 0 ]; then exit 1; fi

# Copy from Backup_WebAR to the final repo, WebAR
# rm -rf ../WebAR/chromium
# rm -rf ../WebAR/examples
# rm -rf ../WebAR/THREE.WebAR
# cp -r ../Backup_WebAR/$BRANCH_NAME/* ../WebAR


