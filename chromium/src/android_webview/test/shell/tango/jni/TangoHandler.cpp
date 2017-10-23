/*
 * Copyright 2014 Google Inc. All Rights Reserved.
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

#include <cstdlib>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>

#include <cassert>

#include <cmath>

#include "TangoHandler.h"

#include <sstream>

#include <thread>

namespace {

constexpr int kTangoCoreMinimumVersion = 9377;
constexpr int kMarkerDetectionFPS = 30;

const float ANDROID_WEBVIEW_ADDRESS_BAR_HEIGHT = 125;

void onPointCloudAvailable(void* context, const TangoPointCloud* pointCloud)
{
  tango_chromium::TangoHandler::getInstance()->onPointCloudAvailable(pointCloud);
}

void onFrameAvailable(void* context, TangoCameraId tangoCameraId, const TangoImageBuffer* imageBuffer)
{
  tango_chromium::TangoHandler::getInstance()->onFrameAvailable(imageBuffer);
}

void onTextureAvailable(void* context, TangoCameraId tangoCameraId)
{
  // Do nothing for now.
}

inline void multiplyMatrixWithVector(const float* m, const double* v, double* vr, bool addTranslation = true) {
  double v0 = v[0];
  double v1 = v[1];
  double v2 = v[2];
    vr[0] = m[ 0] * v0 + m[ 4] * v1 + m[ 8] * v2 + (addTranslation ? m[12] : 0);
    vr[1] = m[ 1] * v0 + m[ 5] * v1 + m[ 9] * v2 + (addTranslation ? m[13] : 0);
    vr[2] = m[ 2] * v0 + m[ 6] * v1 + m[10] * v2 + (addTranslation ? m[14] : 0);
}

inline void matrixInverse(const float* m, float* o)
{
    // based on http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm
    float* te = o;
    const float* me = m;

    float n11 = me[ 0 ], n21 = me[ 1 ], n31 = me[ 2 ], n41 = me[ 3 ],
      n12 = me[ 4 ], n22 = me[ 5 ], n32 = me[ 6 ], n42 = me[ 7 ],
      n13 = me[ 8 ], n23 = me[ 9 ], n33 = me[ 10 ], n43 = me[ 11 ],
      n14 = me[ 12 ], n24 = me[ 13 ], n34 = me[ 14 ], n44 = me[ 15 ],

      t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44,
      t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44,
      t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44,
      t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;

    assert(det != 0);

    float detInv = 1.0 / det;

    te[ 0 ] = t11 * detInv;
    te[ 1 ] = ( n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44 ) * detInv;
    te[ 2 ] = ( n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44 ) * detInv;
    te[ 3 ] = ( n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43 ) * detInv;

    te[ 4 ] = t12 * detInv;
    te[ 5 ] = ( n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44 ) * detInv;
    te[ 6 ] = ( n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44 ) * detInv;
    te[ 7 ] = ( n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43 ) * detInv;

    te[ 8 ] = t13 * detInv;
    te[ 9 ] = ( n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44 ) * detInv;
    te[ 10 ] = ( n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44 ) * detInv;
    te[ 11 ] = ( n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43 ) * detInv;

    te[ 12 ] = t14 * detInv;
    te[ 13 ] = ( n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34 ) * detInv;
    te[ 14 ] = ( n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34 ) * detInv;
    te[ 15 ] = ( n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33 ) * detInv;
}

inline void matrixTranspose(const GLfloat* m, GLfloat* o)
{
    if (o == m)
    {
        GLfloat t;
        t = m[1];
        o[1] = m[4];
        o[4] = t;
        t = m[2];
        o[2] = m[8];
        o[8] = t;
        t = m[3];
        o[3] = m[12];
        o[12] = t;
        t = m[6];
        o[6] = m[9];
        o[9] = t;
        t = m[7];
        o[7] = m[13];
        o[13] = t;
        t = m[11];
        o[11] = m[14];
        o[14] = t;
    }
    else
    {
        o[1] = m[4];
        o[4] = m[1];
        o[2] = m[8];
        o[8] = m[2];
        o[3] = m[12];
        o[12] = m[3];
        o[6] = m[9];
        o[9] = m[6];
        o[7] = m[13];
        o[13] = m[7];
        o[11] = m[14];
        o[14] = m[11];
    }
    o[0] = m[0];
    o[5] = m[5];
    o[10] = m[10];
    o[15] = m[15];
}

inline void matrixFrustum(float const & left,
             float const & right,
             float const & bottom,
             float const & top,
             float const & near,
             float const & far,
             float* matrix)
{

  float x = 2 * near / ( right - left );
  float y = 2 * near / ( top - bottom );

  float a = ( right + left ) / ( right - left );
  float b = ( top + bottom ) / ( top - bottom );
  float c = - ( far + near ) / ( far - near );
  float d = - 2 * far * near / ( far - near );

  matrix[ 0 ] = x;  matrix[ 4 ] = 0;  matrix[ 8 ] = a;  matrix[ 12 ] = 0;
  matrix[ 1 ] = 0;  matrix[ 5 ] = y;  matrix[ 9 ] = b;  matrix[ 13 ] = 0;
  matrix[ 2 ] = 0;  matrix[ 6 ] = 0;  matrix[ 10 ] = c; matrix[ 14 ] = d;
  matrix[ 3 ] = 0;  matrix[ 7 ] = 0;  matrix[ 11 ] = - 1; matrix[ 15 ] = 0;


    // matrix[0] = (float(2) * near) / (right - left);
    // matrix[5] = (float(2) * near) / (top - bottom);
    // matrix[2][0] = (right + left) / (right - left);
    // matrix[2][1] = (top + bottom) / (top - bottom);
    // matrix[10] = -(farVal + nearVal) / (farVal - nearVal);
    // matrix[2][3] = float(-1);
    // matrix[3][2] = -(float(2) * farVal * nearVal) / (farVal - nearVal);
}

inline void matrixProjection(float width, float height,
                      float fx, float fy,
                      float cx, float cy,
                      float near, float far,
                      float* matrix)
{
  const float xscale = near / fx;
  const float yscale = near / fy;

  const float xoffset = (cx - (width / 2.0)) * xscale;
  // Color camera's coordinates has y pointing downwards so we negate this term.
  const float yoffset = -(cy - (height / 2.0)) * yscale;

  matrixFrustum(xscale * -width / 2.0f - xoffset,
          xscale * width / 2.0f - xoffset,
          yscale * -height / 2.0f - yoffset,
          yscale * height / 2.0f - yoffset, near, far,
          matrix);
}

inline double dot(const double* v1, const double* v2)
{
  return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

inline void cross(const double* v1, const double* v2, double* out)
{
  // TODO: Check if v1 or v2 as equal to out.
  out[0] = v1[1] * v2[2] - v1[2] * v2[1];
  out[1] = v1[2] * v2[0] - v1[0] * v2[2];
  out[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

inline void transformPlane(const double* p, const float* m, double* pr)
{
  double pCopy[3] = { p[0], p[1], p[2] };

  pr[0] = p[0] * -p[3];
  pr[1] = p[1] * -p[3];
  pr[2] = p[2] * -p[3];

  multiplyMatrixWithVector(m, pr, pr);
  float mCopy[16];
  double normal[3];
  matrixInverse(m, mCopy);
  matrixTranspose(mCopy, mCopy);
  multiplyMatrixWithVector(mCopy, pCopy, normal, false);

  pr[3] = -dot(pr, normal);
  pr[0] = normal[0];
  pr[1] = normal[1];
  pr[2] = normal[2];
}

inline void matrixFromPointAndPlane(double* point, double* plane, float* m)
{
  const static double WORLD_UP[3] = {0, 1, 0}; 
  const static double THRESHOLD = 0.5;

  double normalY[3] = { 0, 1, 0 };
  if (fabs(dot(WORLD_UP, plane)) > THRESHOLD) {
    normalY[1] = 0; normalY[2] = 1;
  }
  double normalZ[3];
  cross(plane, normalY, normalZ);
  cross(normalZ, plane, normalY);
  memset(m, 0, sizeof(float) * 16);
  m[0] = m[5] = m[10] = m[15] = 1;
  m[ 0] = normalY[0];
  m[ 1] = normalY[1];
  m[ 2] = normalY[2];
  m[ 4] = plane[0];
  m[ 5] = plane[1];
  m[ 6] = plane[2];
  m[ 8] = normalZ[0];
  m[ 9] = normalZ[1];
  m[10] = normalZ[2];
  m[12] = point[0];
  m[13] = point[1];
  m[14] = point[2];
}

bool getADFMetadataValue(const std::string& uuid, const std::string& key, std::string& value)
{
  size_t size = 0;
  TangoAreaDescriptionMetadata metadata;

  // Get the metadata object from the Tango Service.
  int ret = TangoService_getAreaDescriptionMetadata(uuid.c_str(), &metadata);
  if (ret != TANGO_SUCCESS)
  {
    LOGE("getADFMetadataValue: Failed to get ADF metadata with error code: %d", ret);
    return false;
  }

  char* output;
  // Query specific key-value from the metadata object.
  ret = TangoAreaDescriptionMetadata_get(metadata, key.c_str(), &size, &output);
  if (ret != TANGO_SUCCESS)
  {
    LOGE("getADFMetadataValue: Failed to get ADF metadata value for key '%s' with error code: %d", key.c_str(), ret);
    return false;
  }
  if (key == "date_ms_since_epoch")
  {
    uint64_t date = *(uint64_t*)output;
    std::stringstream converter;
    converter << date;
    value = converter.str();
  }
  else
  {
    value = output;
  }

  // We are done with the metadata, so free the used memory.
  ret = TangoAreaDescriptionMetadata_free(metadata);

  return true;
}

bool addADF(const std::string& uuid, std::vector<tango_chromium::ADF>& adfs)
{
  std::string name;
  if (!getADFMetadataValue(uuid, "name", name))
  {
    return false;
  }
  std::string creationTimeString;
  if (!getADFMetadataValue(uuid, "date_ms_since_epoch", creationTimeString))
  {
    return false;
  }
  unsigned long long creationTime = 0;
  std::stringstream converter(creationTimeString);
  if (!(converter >> creationTime))
  {
    LOGE("getADF: Failed to convert the creation time string '%s' to the creation time number.", creationTimeString.c_str());
    return false;
  }
  adfs.push_back(tango_chromium::ADF(uuid, name, creationTime));
  return true;
}

} // End anonymous namespace

namespace tango_chromium {

// bool TangoHandler::getPoseAtTime()
// {
//   if (TangoSupport_getPoseAtTime(
//     timestamp, TANGO_COORDINATE_FRAME_START_OF_SERVICE,
//     TANGO_COORDINATE_FRAME_CAMERA_COLOR, TANGO_SUPPORT_ENGINE_OPENGL,
//     TANGO_SUPPORT_ENGINE_TANGO, ROTATION_IGNORED, &pose) != TANGO_SUCCESS)
//   {
//     LOGE("%s: could not calculate color camera pose at time '%lf'", __func__, timestamp);
//     return result;
//   }
// }

TangoHandler* TangoHandler::instance = 0;

TangoHandler* TangoHandler::getInstance()
{
  if (instance == 0)
  {
    instance = new TangoHandler();
  }
  return instance;
}

void TangoHandler::releaseInstance()
{
  delete instance;
  instance = 0;
}

TangoHandler::TangoHandler(): connected(false)
  , tangoConfig(nullptr)
  , lastTangoImageBufferTimestamp(0)
  , latestTangoPointCloud(0)
  , latestTangoPointCloudRetrieved(false)
  , maxNumberOfPointsInPointCloud(0)
  , pointCloudManager(0)
  , cameraImageWidth(0)
  , cameraImageHeight(0)
  , cameraImageTextureWidth(0)
  , cameraImageTextureHeight(0)
  , textureIdConnected(false)
  , lastMarkerTangoImageBufferTimestamp(0)
  , imageBufferManager(nullptr)
  , poseForMarkerDetectionIsCorrect(false)
{
}

TangoHandler::~TangoHandler()
{
#ifdef TANGO_USE_POINT_CLOUD

  if (pointCloudManager != 0)
  {
    TangoSupport_freePointCloudManager(pointCloudManager);
  }

#endif

  TangoConfig_free(tangoConfig);
  tangoConfig = nullptr;

  jniEnv->DeleteGlobalRef(mainActivityJObject);
}

void TangoHandler::onCreate(JNIEnv* env, jobject activity, int activityOrientation, int sensorOrientation)
{
  // Check the installed version of the TangoCore.  If it is too old, then
  // it will not support the most up to date features.
  int version = 0;
  TangoErrorType result;

  result = TangoSupport_GetTangoVersion(env, activity,
      &version);
  if (result != TANGO_SUCCESS || version < kTangoCoreMinimumVersion)
  {
    LOGE("TangoHandler::OnCreate, Tango Core version is out of date.");
    std::exit (EXIT_SUCCESS);
  }

  // Setup all the JNI needed to make calls from C++ to Java
  jniEnv = env;
  jniEnv->GetJavaVM(&javaVM);
  javaVM->AttachCurrentThread(&jniEnv, NULL);
  mainActivityJObject = jniEnv->NewGlobalRef(activity);
  mainActivityJClass = jniEnv->GetObjectClass(mainActivityJObject);
  requestADFPermissionJMethodID = 
    jniEnv->GetMethodID(mainActivityJClass, "requestADFPermission", "()V");

  this->activityOrientation = activityOrientation;
  this->sensorOrientation = sensorOrientation;
}

void TangoHandler::onTangoServiceConnected(JNIEnv* env, jobject binder)
{
  if (TangoService_setBinder(env, binder) != TANGO_SUCCESS) {
    LOGE("TangoHandler::onTangoServiceConnected, TangoService_setBinder error");
    std::exit (EXIT_SUCCESS);
  }

  connect("");
}


void TangoHandler::connect(const std::string& uuid)
{
  TangoErrorType result;

  // TANGO_CONFIG_DEFAULT is enabling Motion Tracking and disabling Depth
  // Perception.
  tangoConfig = TangoService_getConfig(TANGO_CONFIG_DEFAULT);
  if (tangoConfig == nullptr)
  {
    LOGE("TangoHandler::onTangoServiceConnected, TangoService_getConfig error.");
    std::exit (EXIT_SUCCESS);
  }

  // Enable Depth Perception.
  result = TangoConfig_setBool(tangoConfig, "config_enable_depth", true);
  if (result != TANGO_SUCCESS)
  {
    LOGE("TangoHandler::onTangoServiceConnected, config_enable_depth activation failed with error code: %d.", result);
    std::exit(EXIT_SUCCESS);
  }

  // Setup depth perception
  if (TangoConfig_setInt32(tangoConfig, "config_depth_mode", TANGO_POINTCLOUD_XYZC) != TANGO_SUCCESS)
  {
    LOGE("TangoHandler::onTangoServiceConnected, TangoConfig_setInt32(\"config_depth_mode\", %d): Failed\n", 0);
    std::exit(EXIT_SUCCESS);
  }

  // Note that it is super important for AR applications that we enable low
  // latency IMU integration so that we have pose information available as
  // quickly as possible. Without setting this flag, you will often receive
  // invalid poses when calling getPoseAtTime() for an image.
  result = TangoConfig_setBool(tangoConfig, "config_enable_low_latency_imu_integration", true);
  if (result != TANGO_SUCCESS) {
    LOGE("TangoHandler::onTangoServiceConnected, failed to enable low latency imu integration.");
    std::exit(EXIT_SUCCESS);
  }

#ifdef TANGO_USE_DRIFT_CORRECTION

  // Drift correction allows motion tracking to recover after it loses tracking.
  // The drift corrected pose is is available through the frame pair with
  // base frame AREA_DESCRIPTION and target frame DEVICE.
  result = TangoConfig_setBool(tangoConfig, "config_enable_drift_correction", true);
  if (result != TANGO_SUCCESS) {
    LOGE("TangoHandler::onTangoServiceConnected, enabling config_enable_drift_correction "
      "failed with error code: %d", result);
    std::exit(EXIT_SUCCESS);
  }

#endif  

#ifdef TANGO_USE_POINT_CLOUD

  if (pointCloudManager == 0)
  {
    int maxPointCloudVertexCount_temp = 0;
    result = TangoConfig_getInt32(tangoConfig, "max_point_cloud_elements", &maxPointCloudVertexCount_temp);
    if (result != TANGO_SUCCESS)
    {
      LOGE("TangoHandler::onTangoServiceConnected, Get max_point_cloud_elements failed");
      std::exit(EXIT_SUCCESS);
    }
    maxNumberOfPointsInPointCloud = static_cast<uint32_t>(maxPointCloudVertexCount_temp);

    result = TangoSupport_createPointCloudManager(maxNumberOfPointsInPointCloud, &pointCloudManager);
    if (result != TANGO_SUCCESS)
    {
      LOGE("TangoHandler::onTangoServiceConnected, TangoSupport_createPointCloudManager failed");
      std::exit(EXIT_SUCCESS);
    }

  #ifdef TANGO_USE_POINT_CLOUD_CALLBACK

    result = TangoService_connectOnPointCloudAvailable(::onPointCloudAvailable);
    if (result != TANGO_SUCCESS) {
      LOGE("TangoHandler::onTangoServiceConnected, Failed to connect to point cloud callback with error code: %d", result);
      std::exit(EXIT_SUCCESS);
    }

  #endif

  }

#endif

#ifdef TANGO_USE_CAMERA 

  // Enable color camera from config.
  result = TangoConfig_setBool(tangoConfig, "config_enable_color_camera", true);
  if (result != TANGO_SUCCESS) {
    LOGE("TangoHandler::onTangoServiceConnected, config_enable_color_camera() failed with error code: %d", result);
    std::exit(EXIT_SUCCESS);
  }

  result = TangoService_connectOnTextureAvailable(TANGO_CAMERA_COLOR, this, ::onTextureAvailable);
  if (result != TANGO_SUCCESS)
  {
    LOGE("TangoHandler::connect, failed to connect texture callback with error code: %d", result);
    std::exit(EXIT_SUCCESS);
  }

#endif

  // If there is a uuid, then activate it
  if (uuid != "")
  {
    // jniEnv->CallVoidMethod(mainActivityJObject, requestADFPermissionJMethodID);

    result = TangoConfig_setString(tangoConfig, "config_load_area_description_UUID", uuid.c_str());
    if (result != TANGO_SUCCESS)
    {
      LOGE("TangoHandler::connect: setup the UUID(%s) failed with error code: %d", uuid.c_str(), result);
    }
  }
  lastEnabledADFUUID = uuid;

  // Connect the tango service.
  if (TangoService_connect(this, tangoConfig) != TANGO_SUCCESS)
  {
    LOGE("TangoHandler::connect, TangoService_connect error.");
    std::exit (EXIT_SUCCESS);
  }

  // Get the intrinsics for the color camera and pass them on to the depth
  // image. We need these to know how to project the point cloud into the color
  // camera frame.
  result = TangoService_getCameraIntrinsics(TANGO_CAMERA_COLOR, &tangoCameraIntrinsics);
  if (result != TANGO_SUCCESS) {
    LOGE("TangoHandler::connect: Failed to get the intrinsics for the color camera.");
    std::exit(EXIT_SUCCESS);
  }

#ifdef TANGO_USE_MARKERS

  // Register for color frame callback as we'll need color images for
  // marker detection.
  if (imageBufferManager == nullptr)
  {
    result = TangoSupport_createImageBufferManager(
        TANGO_HAL_PIXEL_FORMAT_YCrCb_420_SP, tangoCameraIntrinsics.width,
        tangoCameraIntrinsics.height, &imageBufferManager);
    if (result != TANGO_SUCCESS) {
      LOGE("TangoHandler::connect, failed to create image buffer manager with error code: %d", result);
      std::exit(EXIT_SUCCESS);
    }
  }

  result = TangoService_connectOnFrameAvailable(TANGO_CAMERA_COLOR, this, ::onFrameAvailable);
  if (result != TANGO_SUCCESS)
  {
    LOGE("TangoHandler::connect, failed to connect frame callback with error code: %d", result);
    std::exit(EXIT_SUCCESS);
  }

#endif

  // By default, use the camera width and height retrieved from the tango camera intrinsics.
  cameraImageWidth = cameraImageTextureWidth = tangoCameraIntrinsics.width;
  cameraImageHeight = cameraImageTextureHeight = tangoCameraIntrinsics.height;

  // Initialize TangoSupport context.
  TangoSupport_initialize(TangoService_getPoseAtTime,
                          TangoService_getCameraIntrinsics);

  connected = true;

  updateCameraIntrinsics();
}

void TangoHandler::disconnect()
{
  TangoService_disconnect();

  cameraImageWidth = cameraImageHeight = 
    cameraImageTextureWidth = cameraImageTextureHeight = 0;

  textureIdConnected = false;

  connected = false;
}

void TangoHandler::onPause()
{
  disconnect();
}

void TangoHandler::onDeviceRotationChanged(int activityOrientation, int sensorOrientation)
{
  this->activityOrientation = activityOrientation;
  this->sensorOrientation = sensorOrientation;
  updateCameraIntrinsics();
}

void TangoHandler::resetPose()
{
  TangoService_resetMotionTracking();
}

bool TangoHandler::updateCameraIntrinsics()
{
  if (!connected) {
    LOGE("TangoHandler::updateCameraIntrinsics, is not connected.");
    return false;
  }

  int result = TangoSupport_getCameraIntrinsicsBasedOnDisplayRotation(
      TANGO_CAMERA_COLOR, static_cast<TangoSupportRotation>(activityOrientation),
      &tangoCameraIntrinsics);

  if (result != TANGO_SUCCESS) {
    LOGE(
        "TangoHandler::updateCameraIntrinsics, failed to get camera intrinsics "
        "with error code: %d",
        result);
    return false;
  }

  /*
  LOGE("TangoHandler::updateCameraIntrinsics, success. fx: %f, fy: %f, width: %d, height: %d, cx: %f, cy: %f",
    tangoCameraIntrinsics.fx,
    tangoCameraIntrinsics.fy,
    tangoCameraIntrinsics.width,
    tangoCameraIntrinsics.height,
    tangoCameraIntrinsics.cx,
    tangoCameraIntrinsics.cy);
  */

  // Always subtract the height of the address bar since we cannot
  // get rid of it
  tangoCameraIntrinsics.height -= ANDROID_WEBVIEW_ADDRESS_BAR_HEIGHT;

  // Update the stored values for width and height
  cameraImageWidth = cameraImageTextureWidth = tangoCameraIntrinsics.width;
  cameraImageHeight = cameraImageTextureHeight = tangoCameraIntrinsics.height;

  return true;
}

bool TangoHandler::isConnected() const
{
  return connected;
}

bool TangoHandler::getPose(TangoPoseData* tangoPoseData, bool* localized)
{
  bool result = connected;
  *localized = false;
  if (connected)
  {
    latestTangoPointCloudRetrieved = false;

    double timestamp = hasLastTangoImageBufferTimestampChangedLately() ? lastTangoImageBufferTimestamp : 0;

    if (lastEnabledADFUUID != "")
    {
      result = TangoSupport_getPoseAtTime(
        timestamp, TANGO_COORDINATE_FRAME_AREA_DESCRIPTION,
        TANGO_COORDINATE_FRAME_CAMERA_COLOR, TANGO_SUPPORT_ENGINE_OPENGL,
        TANGO_SUPPORT_ENGINE_OPENGL, 
        static_cast<TangoSupportRotation>(activityOrientation), tangoPoseData) == TANGO_SUCCESS;
      if (!result)
      {
        LOGE("TangoHandler::getPose: Failed to get the pose for area description.");
      }
      else if (tangoPoseData->status_code != TANGO_POSE_VALID)
      {
        LOGE("TangoHandler::getPose: Getting the Area Description pose did not work. Falling back to device pose estimation.");
      }
      else 
      {
        poseForMarkerDetectionMutex.lock();
        poseForMarkerDetectionIsCorrect = TangoSupport_getPoseAtTime(
          timestamp, TANGO_COORDINATE_FRAME_AREA_DESCRIPTION,
          TANGO_COORDINATE_FRAME_CAMERA_COLOR, TANGO_SUPPORT_ENGINE_OPENGL, 
          TANGO_SUPPORT_ENGINE_TANGO, ROTATION_IGNORED,
          &poseForMarkerDetection) == TANGO_SUCCESS;
        poseForMarkerDetectionMutex.unlock();
        *localized = true;
      }
    }

    if (lastEnabledADFUUID == "" || tangoPoseData->status_code != TANGO_POSE_VALID)
    {
      result = TangoSupport_getPoseAtTime(
        timestamp, TANGO_COORDINATE_FRAME_START_OF_SERVICE,
        TANGO_COORDINATE_FRAME_CAMERA_COLOR, TANGO_SUPPORT_ENGINE_OPENGL, 
        TANGO_SUPPORT_ENGINE_OPENGL, static_cast<TangoSupportRotation>(activityOrientation), tangoPoseData) == TANGO_SUCCESS;
      if (!result)
      {
        LOGE("TangoHandler::getPose: Failed to get the pose.");
      }
      else
      {
        poseForMarkerDetectionMutex.lock();
        poseForMarkerDetectionIsCorrect = TangoSupport_getPoseAtTime(
          timestamp, TANGO_COORDINATE_FRAME_START_OF_SERVICE,
          TANGO_COORDINATE_FRAME_CAMERA_COLOR, TANGO_SUPPORT_ENGINE_OPENGL, 
          TANGO_SUPPORT_ENGINE_TANGO, ROTATION_IGNORED, 
          &poseForMarkerDetection) == TANGO_SUCCESS;
        poseForMarkerDetectionMutex.unlock();
      }
    }
  }

  return result;
}

bool TangoHandler::getPoseMatrix(float* matrix)
{
  bool result = false;

  double timestamp = hasLastTangoImageBufferTimestampChangedLately() ? lastTangoImageBufferTimestamp : 0.0;

  TangoMatrixTransformData tangoMatrixTransformData;
  TangoSupport_getMatrixTransformAtTime(
    timestamp, TANGO_COORDINATE_FRAME_START_OF_SERVICE,
    TANGO_COORDINATE_FRAME_CAMERA_COLOR, TANGO_SUPPORT_ENGINE_OPENGL,
    TANGO_SUPPORT_ENGINE_OPENGL, static_cast<TangoSupportRotation>(activityOrientation), &tangoMatrixTransformData);
  if (tangoMatrixTransformData.status_code != TANGO_POSE_VALID) {
    LOGE("TangoHandler::getPoseMatrix: Could not find a valid matrix transform at time %lf for the color camera.", timestamp);
    return result;
  }
  memcpy(matrix, tangoMatrixTransformData.matrix, 16 * sizeof(float));
  result = true;
  return result;
}

bool TangoHandler::getProjectionMatrix(float near, float far, float* projectionMatrix)
{
  if (!connected) return false;

  bool result = this->updateCameraIntrinsics();

  if (!result) {
    LOGE(
      "TangoHandler::getProjectionMatrix, failed to get camera intrinsics");
    return false;
  }

  float image_width = static_cast<float>(tangoCameraIntrinsics.width);
  float image_height = static_cast<float>(tangoCameraIntrinsics.height);
  float fx = static_cast<float>(tangoCameraIntrinsics.fx);
  float fy = static_cast<float>(tangoCameraIntrinsics.fy);
  float cx = static_cast<float>(tangoCameraIntrinsics.cx);
  float cy = static_cast<float>(tangoCameraIntrinsics.cy);

  matrixProjection(
    image_width, image_height, fx, fy, cx, cy, near,
    far, projectionMatrix);

  return true;
}

unsigned TangoHandler::getMaxNumberOfPointsInPointCloud() const
{
  return maxNumberOfPointsInPointCloud;
}

bool TangoHandler::getPointCloud(uint32_t* numberOfPoints, float* points, bool justUpdatePointCloud, unsigned pointsToSkip, bool transformPoints, float* pointsTransformMatrix)
{
  // In case the point cloud retrieval fails, 0 points should be returned.
  *numberOfPoints = 0;

  pointsToSkip += 1;

  if (connected)
  {
    TangoErrorType result = TangoSupport_getLatestPointCloud(pointCloudManager, &latestTangoPointCloud);
    if (result == TANGO_SUCCESS)
    {
      latestTangoPointCloudRetrieved = true;

      if (lastEnabledADFUUID != "")
      {
        TangoSupport_getMatrixTransformAtTime(
          latestTangoPointCloud->timestamp,
          TANGO_COORDINATE_FRAME_AREA_DESCRIPTION,
          TANGO_COORDINATE_FRAME_CAMERA_DEPTH, TANGO_SUPPORT_ENGINE_OPENGL,
          TANGO_SUPPORT_ENGINE_TANGO, static_cast<TangoSupportRotation>(activityOrientation), &depthCameraMatrixTransform);
      }

      if (lastEnabledADFUUID == "" || depthCameraMatrixTransform.status_code != TANGO_POSE_VALID)
      {
        TangoSupport_getMatrixTransformAtTime(
          latestTangoPointCloud->timestamp,
          TANGO_COORDINATE_FRAME_START_OF_SERVICE,
          TANGO_COORDINATE_FRAME_CAMERA_DEPTH, TANGO_SUPPORT_ENGINE_OPENGL,
          TANGO_SUPPORT_ENGINE_TANGO, static_cast<TangoSupportRotation>(activityOrientation), &depthCameraMatrixTransform);
      }

      if (depthCameraMatrixTransform.status_code == TANGO_POSE_VALID)
      {
        // If only the update was requested, return with 0 points.
        if (justUpdatePointCloud)
        {
          return true;
        }

        // It is possible that the transform matrix retrieval fails but the count is already there/correct.
        // TODO: Soon, the transformation of the points should be done in a shader in the application side, so the matrix retrieval could inside this method will be avoided.
        *numberOfPoints = std::ceil(latestTangoPointCloud->num_points / pointsToSkip);

        TangoPointCloud tangoPointCloud;
        tangoPointCloud.version = latestTangoPointCloud->version;
        tangoPointCloud.timestamp = latestTangoPointCloud->timestamp;
        tangoPointCloud.num_points = latestTangoPointCloud->num_points;

        if (transformPoints)
        {
          memcpy(pointsTransformMatrix, depthCameraMatrixTransform.matrix, 16 * sizeof(float));
          tangoPointCloud.points = new float[latestTangoPointCloud->num_points][4];
          result = TangoSupport_transformPointCloud(depthCameraMatrixTransform.matrix, latestTangoPointCloud, &tangoPointCloud);
        }
        else
        {
          // Set the transform matrix to identity
          memset(pointsTransformMatrix, 0, 16 * sizeof(float));
          pointsTransformMatrix[0] = pointsTransformMatrix[5] = pointsTransformMatrix[10] = pointsTransformMatrix[15] = 1; 
          tangoPointCloud.points = latestTangoPointCloud->points;
          // result is already TANGO_SUCCESS
        }
        if (result == TANGO_SUCCESS)
        {
          uint32_t offset;
          float* ptrToPoints = tangoPointCloud.points[0];
          // float* ptrToPoints = latestTangoPointCloud->points[0];
          for (uint32_t i = 0, j = 0; i < tangoPointCloud.num_points; i += pointsToSkip, j += 3)
          {
            offset = i * 4;
            points[j    ] = ptrToPoints[offset    ];
            points[j + 1] = ptrToPoints[offset + 1];
            points[j + 2] = ptrToPoints[offset + 2];
          }
          // Cannot make this call anymore as the structure is a 4 value array per point (the fourth being the confidence factor).
          // memcpy(points, latestTangoPointCloud->points[0], sizeof(float) * latestTangoPointCloud->num_points * 3);
        }
        else
        {
          LOGE("TangoHandler::getPointCloud, transforming the point cloud with the depth camera transform matrix failed.");
        }
        if (transformPoints)
        {
          delete [] tangoPointCloud.points;
        }
      }
      else
      {
        LOGE("TangoHandler::getPointCloud, retrieving the depth camera transform matrix failed.");
      }
    }
    else
    {
      LOGE("TangoHandler::getPointCloud, retrieving the latest point cloud failed.");
    }
  }
  return connected;
}

bool TangoHandler::hitTest(float x, float y, std::vector<Hit>& hits)
{
  bool result = false;

  if (connected)
  {
    double timestamp = hasLastTangoImageBufferTimestampChangedLately() ? lastTangoImageBufferTimestamp : 0.0;

    if (!latestTangoPointCloud || !latestTangoPointCloudRetrieved)
    {
     uint32_t numberOfPoints;
     if (!getPointCloud(&numberOfPoints, nullptr, true, 0, false, nullptr))
     {
       LOGE("%s: could not get point cloud", __func__);
     }
     latestTangoPointCloudRetrieved = true;
    }

    TangoPoseData tangoPose;
    if (TangoSupport_calculateRelativePose(
      latestTangoPointCloud->timestamp, TANGO_COORDINATE_FRAME_CAMERA_DEPTH,
      timestamp, TANGO_COORDINATE_FRAME_CAMERA_COLOR, &tangoPose) != TANGO_SUCCESS)
    {
      LOGE("%s: could not calculate color camera pose at time '%lf'", __func__, timestamp);
      return result;
    }
    float uv[] = {x, y};
    double identity_translation[3] = {0.0, 0.0, 0.0};
    double identity_orientation[4] = {0.0, 0.0, 0.0, 1.0};
    double point[3];
    double plane[4];
    if (TangoSupport_fitPlaneModelNearPoint(
      latestTangoPointCloud, identity_translation, identity_orientation,
      uv, static_cast<TangoSupportRotation>(activityOrientation),
      tangoPose.translation,
      tangoPose.orientation,
      point, plane) != TANGO_SUCCESS)
    {
      LOGE("%s: could not calculate picking point and plane", __func__);
      return result;
    }
    if (depthCameraMatrixTransform.status_code != TANGO_POSE_VALID) {
      LOGE("TangoHandler::getPickingPointAndPlaneInPointCloud: Could not find a valid matrix transform at "
      "time %lf for the depth camera.", latestTangoPointCloud->timestamp);
      return result;
    }

    multiplyMatrixWithVector(depthCameraMatrixTransform.matrix, point, point);

    transformPlane(plane, depthCameraMatrixTransform.matrix, plane);

    Hit hit;
    matrixFromPointAndPlane(point, plane, hit.modelMatrix);
    hits.push_back(hit);

    result = true;
  }

  return result;
}

bool TangoHandler::getCameraImageSize(uint32_t* width, uint32_t* height)
{
  bool result = true;

  *width = cameraImageWidth;
  *height = cameraImageHeight;

  return result;
}

bool TangoHandler::getCameraImageTextureSize(uint32_t* width, uint32_t* height)
{
  bool result = true;

  *width = cameraImageTextureWidth;
  *height = cameraImageTextureHeight;

  return result;
}

bool TangoHandler::getCameraFocalLength(double* focalLengthX, double* focalLengthY)
{
  bool result = true;
  *focalLengthX = tangoCameraIntrinsics.fx;
  *focalLengthY = tangoCameraIntrinsics.fy;
  return result;
}

bool TangoHandler::getCameraPoint(double* x, double* y)
{
  bool result = true;
  *x = tangoCameraIntrinsics.cx;
  *y = tangoCameraIntrinsics.cy;
  return result;
}

void TangoHandler::onFrameAvailable(const TangoImageBuffer* imageBuffer)
{
  TangoSupport_updateImageBuffer(imageBufferManager, imageBuffer);
}

bool TangoHandler::updateCameraImageIntoTexture(uint32_t textureId)
{
  if (!connected) return false;

  // if (!textureIdConnected)
  // {
  //   TangoErrorType result = TangoService_connectTextureId(TANGO_CAMERA_COLOR, textureId, nullptr, nullptr);
  //   if (result != TANGO_SUCCESS)
  //   {
  //     LOGE("TangoHandler::updateCameraImageIntoTexture: Failed to connect the texture id with error code: %d", result);
  //     return false;
  //   }
  //   textureIdConnected = true;
  // }

  TangoErrorType result = TangoService_updateTextureExternalOes(TANGO_CAMERA_COLOR, textureId, &lastTangoImageBufferTimestamp);

  std::time(&lastTangoImagebufferTimestampTime);

  // LOGI("JUDAX: TangoHandler::updateCameraImageIntoTexture lastTangoImageBufferTimestamp = %lf, result = %d, textureId = %d", lastTangoImageBufferTimestamp, result, textureId);

  return result == TANGO_SUCCESS;
}

#ifdef TANGO_USE_POINT_CLOUD_CALLBACK

void TangoHandler::onPointCloudAvailable(const TangoPointCloud* pointCloud)
{
  TangoSupport_updatePointCloud(pointCloudManager, pointCloud);
}

#endif

int TangoHandler::getActivityOrientation() const
{
  return activityOrientation;
}

int TangoHandler::getSensorOrientation() const
{
  return sensorOrientation;
}

bool TangoHandler::getADFs(std::vector<ADF>& adfs) const
{
  // jniEnv->CallVoidMethod(mainActivityJObject, requestADFPermissionJMethodID);

  TangoConfig tango_config_ = TangoService_getConfig(TANGO_CONFIG_DEFAULT);
  if (tango_config_ == nullptr) {
    LOGE("TangoHandler::getADFs failed to get default config form");
    return false;
  }

  char* uuids = nullptr;
  int ret = TangoService_getAreaDescriptionUUIDList(&uuids);
  if (ret != TANGO_SUCCESS) {
    LOGE("TangoHandler::getADFs failed to get the area description with error code: %d", ret);
    return false;
  }

  adfs.clear();
  std::string uuid;
  bool thereIsADF = false; // This flag allows us to know if there is an ADF to be added (the uuid has been filled up).
  for (unsigned int i = 0; uuids[i] != 0; i++)
  {
    if (uuids[i] == ',')
    {
      if (thereIsADF && !addADF(uuid, adfs))
      {
        LOGE("TangoHandler::getADFs failed to create the ADF for uuid '%s'", uuid.c_str());
        // For now, if one ADF fails, continue retrieving the others.
      }
      uuid = "";
      thereIsADF = false;
    }
    else 
    {
      thereIsADF = true;
      uuid += uuids[i];
    }
  }
  // Add the last ADF
  if (thereIsADF && !addADF(uuid, adfs))
  {
    LOGE("TangoHandler::getADFs failed to create the ADF for uuid '%s'", uuid.c_str());
    // For now, if one ADF fails, continue retrieving the others.
  }
  return true;
}

void TangoHandler::enableADF(const std::string& uuid)
{
  if (lastEnabledADFUUID != uuid)
  {
    if (connected)
    {
      disconnect();
    }
    connect(uuid);
  }
}

void TangoHandler::disableADF()
{
  if (lastEnabledADFUUID != "")
  {
    if (connected)
    {
      disconnect();
    }
    connect("");
  }
}

bool TangoHandler::getMarkers(TangoSupportMarkerType markerType, float markerSize, std::vector<Marker>& markers)
{
  if (connected)
  {
    // Copy the currently detected markers to the passed container.
    // Once copied, clear the detected markers.
    markerDetectionMutex.lock();
    markers.insert(markers.begin(), detectedMarkers.begin(), detectedMarkers.end());
    detectedMarkers.clear();
    markerDetectionMutex.unlock();

    // Marker detection is a time-consuming process. This is to make sure marker
    // detection process runs at a frequency no higher than a pre-defined FPS.
    if (lastTangoImageBufferTimestamp < lastMarkerTangoImageBufferTimestamp + 1.0 / kMarkerDetectionFPS)
      return true;

    lastMarkerTangoImageBufferTimestamp = lastTangoImageBufferTimestamp;

    // Start a new detection query in a different thread
    std::thread t([this, markerType, markerSize]()
    {
      // Get latest image buffer.
      TangoImageBuffer* imageBuffer = nullptr;
      TangoErrorType status = TangoSupport_getLatestImageBuffer(
          imageBufferManager, &imageBuffer);
      if (status == TANGO_SUCCESS)
      {
        if (status == TANGO_SUCCESS)
        {
          TangoSupportMarkerParam param;
          param.type = markerType;
          param.marker_size = markerSize;
          TangoSupportMarkerList markerList;
          
          double translation[3];
          double orientation[4];
          // Get the translation and orientation from the last pose.
          poseForMarkerDetectionMutex.lock();
          memcpy(translation, poseForMarkerDetection.translation, 
            sizeof(double) * 3);
          memcpy(orientation, poseForMarkerDetection.orientation, 
            sizeof(double) * 4);
          poseForMarkerDetectionMutex.unlock();

          if (TangoSupport_detectMarkers(imageBuffer, TANGO_CAMERA_COLOR, translation, orientation, &param, &markerList) == TANGO_SUCCESS)
          {
            markerDetectionMutex.lock();
            detectedMarkers.clear();
            for (int i = 0; i < markerList.marker_count; ++i)
            {
              int id = 0;
              std::string content;
              switch(markerType)
              {
                case TANGO_MARKER_ARTAG:
                  id = atoi(markerList.markers[i].content);
                  break;
                case TANGO_MARKER_QRCODE:
                  content = std::string(markerList.markers[i].content, markerList.markers[i].content_size);
                  break;
              }
              detectedMarkers.push_back(Marker(markerType, id, 
                content, markerList.markers[i].translation, 
                markerList.markers[i].orientation));
            }
            markerDetectionMutex.unlock();
            TangoSupport_freeMarkerList(&markerList);
          }
        }
      }
    });

    // Let the thread run independently from the rendering thread.
    t.detach();
  }

  return connected;
}

bool TangoHandler::hasLastTangoImageBufferTimestampChangedLately()
{
  std::time_t currentTime;
  std::time(&currentTime);
  return std::difftime(currentTime, lastTangoImagebufferTimestampTime) < 1.0;
}

}  // namespace tango_chromium
