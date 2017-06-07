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

#ifndef _TANGO_HANDLER_H_
#define _TANGO_HANDLER_H_

#include "tango_client_api.h"   // NOLINT
#include "tango_support_api.h"  // NOLINT

#include <pthread.h>

#include <ctime>

#include <jni.h>
#include <android/log.h>

#include <string>
#include <vector>
#include <queue>

#define LOG_TAG "Tango Chromium"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Some preprocessor symbols that allow to control some of the TangoHandler features/capabilities.
#define TANGO_USE_POINT_CLOUD
#define TANGO_USE_POINT_CLOUD_CALLBACK
#define TANGO_USE_CAMERA
// #define TANGO_USE_DRIFT_CORRECTION
// #define TANGO_USE_AREA_DESCRIPTION

#ifdef TANGO_USE_DRIFT_CORRECTION
#define TANGO_COORDINATE_FRAME TANGO_COORDINATE_FRAME_AREA_DESCRIPTION
#else
#define TANGO_COORDINATE_FRAME TANGO_COORDINATE_FRAME_START_OF_SERVICE
#endif

#define MAX_NUMBER_OF_TANGO_BUFFER_IDS 1

namespace tango_chromium {

class ADF {
public:
	ADF(const std::string& uuid, const std::string& name, unsigned long long creationTime): uuid(uuid), name(name), creationTime(creationTime)
	{
	}

	inline const std::string& getUUID() const
	{
		return uuid;
	}

	inline const std::string& getName() const
	{
		return name;
	}
	
	inline unsigned long long getCreationTime() const
	{
		return creationTime;
	}
private:
	std::string uuid;
	std::string name;
	unsigned long long creationTime;
};

// TangoHandler provides functionality to communicate with the Tango Service.
class TangoHandler {
public:
	static TangoHandler* getInstance();
	static void releaseInstance();

	TangoHandler();

	// TangoHandler(const TangoHandler& other) = delete;
	// TangoHandler& operator=(const TangoHandler& other) = delete;

	~TangoHandler();

	void onCreate(JNIEnv* env, jobject activity, int activityOrientation, int sensorOrientation);
	void onTangoServiceConnected(JNIEnv* env, jobject binder);
	void onPause();
	void onDeviceRotationChanged(int activityOrientation, int sensorOrientation);

	bool isConnected() const;

	bool getPose(TangoPoseData* tangoPoseData);
	bool getPoseMatrix(float* matrix);

	unsigned getMaxNumberOfPointsInPointCloud() const;
	bool getPointCloud(uint32_t* numberOfPoints, float* points, bool justUpdatePointCloud, unsigned pointsToSkip, bool transformPointsFromDepthToPose, float* pointsTransformMatrix);
	bool getPickingPointAndPlaneInPointCloud(float x, float y, double* point, double* plane);

	bool getCameraImageSize(uint32_t* width, uint32_t* height);
	bool getCameraImageTextureSize(uint32_t* width, uint32_t* height);
	bool getCameraFocalLength(double* focalLengthX, double* focalLengthY);
	bool getCameraPoint(double* x, double* y);
	bool updateCameraImageIntoTexture(uint32_t textureId);

#ifdef TANGO_USE_POINT_CLOUD_CALLBACK
	void onPointCloudAvailable(const TangoPointCloud* pointCloud);
#endif
	
	void onCameraFrameAvailable(const TangoImageBuffer* buffer);

	int getSensorOrientation() const;

	bool getADFs(std::vector<ADF>& adfs) const;
	void enableADF(const std::string& uuid);
	void disableADF();

private:
	void connect(const std::string& uuid);
	void disconnect();
	bool hasLastTangoImageBufferTimestampChangedLately();

	static TangoHandler* instance;

	bool connected;
	TangoConfig tangoConfig;
	TangoCameraIntrinsics tangoCameraIntrinsics;
	double lastTangoImageBufferTimestamp;
	std::time_t lastTangoImagebufferTimestampTime;

	unsigned maxNumberOfPointsInPointCloud;
	TangoSupportPointCloudManager* pointCloudManager;
	TangoPointCloud* latestTangoPointCloud;
	bool latestTangoPointCloudRetrieved;

	uint32_t cameraImageWidth;
	uint32_t cameraImageHeight;
	uint32_t cameraImageTextureWidth;
	uint32_t cameraImageTextureHeight;

	bool textureIdConnected;

	int activityOrientation;
	int sensorOrientation;

	std::string lastEnabledADFUUID;

	pthread_mutex_t tangoBufferIdsMutex;
	std::queue<TangoBufferId> tangoBufferIds;
};
}  // namespace tango_4_chromium

#endif  // _TANGO_HANDLER_H_
