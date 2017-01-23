# Copyright 2017 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

# Rebuilt (-B) specifying the makefile to use and the output folder for the final libraries and intermediate files
echo "Rebuilding..."
# Add V=1 for a verbose output that shows the commands that are being executed.
$ANDROID_NDK_PATH/ndk-build NDK_APPLICATION_MK=./Application.mk NDK_LIBS_OUT=../libs NDK_OUT=./objs -B 
EXIT_CODE=$?
if [ $EXIT_CODE -ne 0 ]; then 
	exit $EXIT_CODE
fi
rm -rf ../../../../../third_party/tango/libtango_chromium
EXIT_CODE=$?
if [ $EXIT_CODE -ne 0 ]; then 
	exit $EXIT_CODE
fi
mkdir -p ../../../../../third_party/tango/libtango_chromium/armeabi-v7a
EXIT_CODE=$?
if [ $EXIT_CODE -ne 0 ]; then 
	exit $EXIT_CODE
fi
cp ../libs/armeabi-v7a/libtango_chromium.so ../../../../../third_party/tango/libtango_chromium/armeabi-v7a/libtango_chromium.so
EXIT_CODE=$?
if [ $EXIT_CODE -ne 0 ]; then 
	exit $EXIT_CODE
fi
cp TangoHandler.h ../../../../../third_party/tango/libtango_chromium
EXIT_CODE=$?
if [ $EXIT_CODE -ne 0 ]; then 
	exit $EXIT_CODE
fi
echo "Rebuilt!"

