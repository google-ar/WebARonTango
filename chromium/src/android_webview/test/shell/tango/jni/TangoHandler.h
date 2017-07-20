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

#include <ctime>

#include <jni.h>
#include <android/log.h>

#include <string>
#include <vector>
#include <queue>

#include <mutex>

#define LOG_TAG "Tango Chromium"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Some preprocessor symbols that allow to control some of the TangoHandler features/capabilities.
#define TANGO_USE_POINT_CLOUD
#define TANGO_USE_POINT_CLOUD_CALLBACK
#define TANGO_USE_CAMERA
#define TANGO_USE_MARKERS

#define MAX_NUMBER_OF_TANGO_BUFFER_IDS 1

namespace tango_chromium {

class ADF 
{
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

class Marker
{
public:
	Marker(TangoSupportMarkerType type, int id, const std::string& content, const double* position, const double* orientation): type(type), id(id), content(content)
	{
		memcpy(this->position, position, sizeof(this->position));
		memcpy(this->orientation, orientation, sizeof(this->orientation));
	}

	TangoSupportMarkerType getType() const
	{
		return type;
	}

	int getId() const
	{
		return id;
	}

	std::string getContent() const
	{
		return content;
	}

	const double* getPosition() const
	{
		return position;
	}

	const double* getOrientation() const
	{
		return orientation;
	}
private:
	TangoSupportMarkerType type;
	int id;
	std::string content;
	double position[3];
	double orientation[4];
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

	bool getPose(TangoPoseData* tangoPoseData, bool* isLocalized);
	bool getPoseMatrix(float* matrix);

	unsigned getMaxNumberOfPointsInPointCloud() const;
	bool getPointCloud(uint32_t* numberOfPoints, float* points, bool justUpdatePointCloud, unsigned pointsToSkip, bool transformPoints, float* pointsTransformMatrix);
	bool getPickingPointAndPlaneInPointCloud(float x, float y, double* point, double* plane);

	bool getCameraImageSize(uint32_t* width, uint32_t* height);
	bool getCameraImageTextureSize(uint32_t* width, uint32_t* height);
	bool getCameraFocalLength(double* focalLengthX, double* focalLengthY);
	bool getCameraPoint(double* x, double* y);
	bool updateCameraImageIntoTexture(uint32_t textureId);

#ifdef TANGO_USE_POINT_CLOUD_CALLBACK
	void onPointCloudAvailable(const TangoPointCloud* pointCloud);
#endif
	
	void onFrameAvailable(const TangoImageBuffer* imageBuffer);

	int getSensorOrientation() const;

	bool getADFs(std::vector<ADF>& adfs) const;
	void enableADF(const std::string& uuid);
	void disableADF();

	bool detectMarkers(TangoSupportMarkerType markerType, float markerSize, std::vector<Marker>& markers);

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
	TangoMatrixTransformData depthCameraMatrixTransform;

	uint32_t cameraImageWidth;
	uint32_t cameraImageHeight;
	uint32_t cameraImageTextureWidth;
	uint32_t cameraImageTextureHeight;

	bool textureIdConnected;

	int activityOrientation;
	int sensorOrientation;

	std::string lastEnabledADFUUID;

	std::mutex tangoBufferIdsMutex;
	std::queue<TangoBufferId> tangoBufferIds;

	JNIEnv* jniEnv;
	JavaVM* javaVM;
	jclass mainActivityJClass;
	jobject mainActivityJObject;
	jmethodID requestADFPermissionJMethodID;

	double lastMarkerTangoImageBufferTimestamp;
	std::mutex markerDetectionMutex;
	std::vector<Marker> detectedMarkers;
	TangoSupportImageBufferManager* imageBufferManager;

	TangoPoseData poseForMarkerDetection;
	std::mutex poseForMarkerDetectionMutex;
	bool poseForMarkerDetectionIsCorrect;
};
}  // namespace tango_4_chromium

#endif  // _TANGO_HANDLER_H_
