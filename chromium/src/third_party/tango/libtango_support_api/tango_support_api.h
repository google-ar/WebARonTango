// Copyright 2015 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TANGO_SUPPORT_API_H_
#define TANGO_SUPPORT_API_H_

#include <tango_client_api.h>

#include <stdint.h>

/// @file tango_support_api.h
/// @brief File containing the Project Tango Support C API. The Project Tango
///   Support C API provides helper functions useful to external developers for
///   manipulating Project Tango data. The Project Tango Support C API is
///   experimental and subject to change.

#ifdef __cplusplus
extern "C" {
#endif

/// @defgroup MiscUtilities Miscellaneous Utility Functions
/// @brief Functions that do not fit into any other group.
/// @{

/// @brief Get the version code of the installed TangoCore package.
/// @param jni_env A pointer to the JNI Context of the native activity. This
///   must be of type JNIEnv*, and implicit type conversion should do the right
///   thing without requiring a cast.
/// @param activity The native activity object handle of the calling native
///   activity.  This should be of type jobject, and implicit type conversion
///   should do the right thing without requiring a cast.
/// @param version Filled out with the version of the installed
///   TangoCore package or 0 if it is not installed.
/// @return @c TANGO_SUCCESS if the version was able to be successfully found.
///   @c TANGO_ERROR if some other error happened.
TangoErrorType TangoSupport_GetTangoVersion(void* jni_env, void* activity,
                                            int* version);

/// @brief Typedef for getPostAtTime function signature; required by the
/// @c TangoSupport_initialize method.
typedef TangoErrorType (*TangoSupport_GetPoseAtTimeFn)(
    double timestamp, TangoCoordinateFramePair frame, TangoPoseData* pose);

/// @brief Typedef for getCameraIntrinsics function signature; required by the
///   @c TangoSupport_initialize method.
typedef TangoErrorType (*TangoSupport_GetCameraIntrinsicsFn)(
    TangoCameraId camera_id, TangoCameraIntrinsics* intrinsics);

/// @brief Initialize the support library with any function pointers or values
///   that are required for functionality provided by the library. This version
///   requires providing each of the necessary initialization parameters and
///   only needs to be used if specialized parameters are necessary. Generally
///   either this version or @c TangoSupport_initializeLibrary should be called
///   during application initialization, but not both.
///
/// @param getPoseAtTime The function to call to retrieve device pose
///   information. In practice this is TangoService_getPoseAtTime, except
///   for testing.
/// @param getCameraIntrinsics The function to call to retrieve camera
///   intrinsics information. In practice this is
///   TangoService_getCameraIntrinsics, except for testing.
void TangoSupport_initialize(
    TangoSupport_GetPoseAtTimeFn getPoseAtTime,
    TangoSupport_GetCameraIntrinsicsFn getCameraIntrinsics);

/// @brief Initialize the support library with any function pointers or values
///   that are required for functionality provided by the library. Generally
///   either this version or @c TangoSupport_initialize should be called
///   during application initialization, but not both.
inline void TangoSupport_initializeLibrary() {
  TangoSupport_initialize(TangoService_getPoseAtTime,
                          TangoService_getCameraIntrinsics);
}

/// @}

/// @defgroup CallbackHelpers Callback Data Support Functions
/// @brief Functions for managing data received from callbacks.
/// @{

/// The TangoSupportImageBufferManager maintains a set of image buffers to
/// manage transferring a TangoImageBuffer from the callback thread to a render
/// or computation thread. This holds three buffers internally (back, swap,
/// front). The back buffer is used as the destination for pixels copied from
/// the callback thread. When the copy is complete the back buffer is swapped
/// with the swap buffer while holding a lock. Calling
/// @c TangoSupport_getLatestImagebuffer holds the lock to exchange the swap
/// buffer with the front buffer (if there is newer data in the swap buffer
/// than the current front buffer).
struct TangoSupportImageBufferManager;

/// @brief Create an object for maintaining a set of image buffers for a
///   specified image format and size.
///
/// @param format The format of the color camera image.
/// @param width The width in pixels of the color images.
/// @param height The height in pixels of the color images.
/// @param manager A handle to the manager object.
/// @return @c TANGO_SUCCESS if allocation was successful; @c TANGO_INVALID if
///   manager is NULL.
TangoErrorType TangoSupport_createImageBufferManager(
    TangoImageFormatType format, int width, int height,
    TangoSupportImageBufferManager** manager);

/// @brief Delete the image buffer manager object.
///
/// @param manager A handle to the manager to delete.
/// @return @c TANGO_SUCCESS if memory was freed successfully; @c TANGO_INVALID
///   otherwise.
TangoErrorType TangoSupport_freeImageBufferManager(
    TangoSupportImageBufferManager* manager);

/// @brief Limit copying of the incoming image to a specific range of
///   scan lines.
///
/// This is an optimization when only a portion of the image is
/// required. For the @p begin_line and @p end_line parameters, the
/// following must be true:
///
/// 0 <= @p begin_line <= @p end_line <= (image_height - 1)
///
/// @param manager A handle to the image buffer manager.
/// @param y_only If non-zero and the image is YUV, copy only the Y-portion
///   (grayscale intensities) of the image. If zero, copy Y and UV portions.
/// @param begin_line The first scan line row to copy. Must be less than
///   end_line.
/// @param end_line The last scan line row to copy. Must be greater than
///   begin_line and smaller than the image height.
/// @return @c TANGO_SUCCESS if copy region was updated successfully, or
///   @c TANGO_INVALID if the preconditions are not satisfied.
TangoErrorType TangoSupport_setImageBufferCopyRegion(
    TangoSupportImageBufferManager* manager, int y_only, uint32_t begin_line,
    uint32_t end_line);

/// @brief Updates the back buffer of the manager with new data from the
///   callback. This should be called from the image callback thread.
///
/// @param manager A handle to the image buffer manager.
/// @param image_buffer New image buffer data from the camera callback thread.
/// @return Returns @c TANGO_SUCCESS on update of the back image buffer. Returns
///   @c TANGO_INVALID otherwise.
TangoErrorType TangoSupport_updateImageBuffer(
    TangoSupportImageBufferManager* manager,
    const TangoImageBuffer* image_buffer);

/// @brief Check if updated color image data is available. If so, swap new data
///   to the front buffer and set image_buffer to point to the front buffer.
///   This should be called from a single computation or render thread. Set
///   new_data to true when image_buffer points to new data.
///
/// @param manager A handle to the image buffer manager.
/// @param image_buffer After the call contains a pointer to the most recent
///   camera image buffer.
/// @param new_data True if latest_point_cloud points to new data. False
///   otherwise.
/// @return Returns @c TANGO_SUCCESS if params are valid. Returns
///   @c TANGO_INVALID otherwise.
TangoErrorType TangoSupport_getLatestImageBufferAndNewDataFlag(
    TangoSupportImageBufferManager* manager, TangoImageBuffer** image_buffer,
    bool* new_data);

/// @brief Check if updated color image data is available. If so, swap new data
///   to the front buffer and set image_buffer to point to the front buffer.
//    This should be called from a single computation or render thread.
///
/// @param manager A handle to the image buffer manager.
/// @param image_buffer After the call contains a pointer to the most recent
///   camera image buffer.
/// @return Returns @c TANGO_SUCCESS if params are valid. Returns
///   @c TANGO_INVALID otherwise.
TangoErrorType TangoSupport_getLatestImageBuffer(
    TangoSupportImageBufferManager* manager, TangoImageBuffer** image_buffer);

/// @}

/// @defgroup DepthSupport Depth Interface Support Functions
/// @brief Functions for managing depth data.
/// @{

/// @brief Initializes an empty point cloud with a buffer large enough to store
///   the specific maximum point cloud size. The logical number of vertices
///   (xyz_count) is initialized to zero.
///
/// @param max_point_cloud_size The maximum number of vertices in the point
///   cloud. This value should typically be retrieved from TangoConfig
///   max_point_cloud_elements.
/// @param point_cloud A pointer to the point cloud to be initialized. Cannot be
///   NULL.
/// @return @c TANGO_SUCCESS on successful allocation, or @c TANGO_INVALID if
///   @p point_cloud is NULL.
TangoErrorType TangoSupport_createPointCloud(uint32_t max_point_cloud_size,
                                             TangoPointCloud* point_cloud);

/// @brief Deletes a point cloud.
///
/// @param point_cloud A pointer to the point cloud to be deleted. Cannot be
///   NULL.
/// @return @c TANGO_SUCCESS if memory was freed successfully, or
///   @c TANGO_INVALID if @p point_cloud was NULL.
TangoErrorType TangoSupport_freePointCloud(TangoPointCloud* point_cloud);

/// @brief Performs a deep copy between two point clouds. The point clouds must
///   have been initialized with the same maximum size.
///
/// @param input_point_cloud The point cloud to be copied. Cannot be NULL.
/// @param output_point_cloud The output point cloud. Cannot be NULL.
/// @return @c TANGO_SUCCESS if copy was successful, @c TANGO_INVALID if
///   @p input_point_cloud or @p output_point_cloud is NULL.
TangoErrorType TangoSupport_copyPointCloud(
    const TangoPointCloud* input_point_cloud,
    TangoPointCloud* output_point_cloud);

/// @brief Fits a plane to a point cloud near a user-specified location. This
///   occurs in two passes. First, all points are projected to the image plane
///   and only points near the user selection are kept. Then a plane is fit to
///   the subset using RANSAC. After the RANSAC fit, all inliers from the
///   original input point cloud are used to refine the plane model. The output
///   is in the coordinate system of the input point cloud.
///
/// @param point_cloud The input point cloud. Cannot be NULL and must have at
///   least three points.
/// @param color_camera_T_point_cloud The pose of the point cloud relative to
///   the color camera used to obtain uv_coordinates.
/// @param uv_coordinates The UV coordinates for the user selection. This is
///   expected to be between (0.0, 0.0) and (1.0, 1.0) and can be computed from
///   pixel coordinates by dividing by the width or height. Cannot be NULL.
/// @param intersection_point The intersection of the fitted plane with the user
///   selected camera-ray, in point cloud coordinates, accounting for distortion
///   by undistorting the input uv coordinate. Cannot be NULL.
/// @param plane_model The four parameters a, b, c, d for the general plane
///   equation ax + by + cz + d = 0 of the plane fit. The first three
///   components are a unit vector. The output is in the coordinate system of
///   the point cloud. Cannot be NULL.
/// @return @c TANGO_SUCCESS on success, @c TANGO_INVALID on invalid input, and
///   @c TANGO_ERROR on failure.
TangoErrorType TangoSupport_fitPlaneModelNearPoint(
    const TangoPointCloud* point_cloud,
    const TangoPoseData* color_camera_T_point_cloud,
    const float uv_coordinates[2], double intersection_point[3],
    double plane_model[4]);

/// The TangoSupportPointCloudManager maintains a set of point clouds to
/// manage transferring a TangoPointCloud from the callback thread to a render
/// or computation thread. This holds three buffers internally (back, swap,
/// front). The back buffer is used as the destination for data copied from
/// the callback thread. When the copy is complete the back buffer is swapped
/// with the swap buffer while holding a lock. If there is newer data in
/// the swap buffer than the current front buffer then calling SwapBuffer holds
/// the lock and swaps the swap buffer with the front buffer.
struct TangoSupportPointCloudManager;

/// @brief Create an object for maintaining a set of point clouds for a
///   specified size.
///
/// @param max_points Maximum number of points in TangoPointCloud. Get value
///   from config.
/// @param manager A handle to the manager object.
/// @return @c TANGO_SUCCESS on successful creation, @c TANGO_INVALID if
///   @p max_points <= 0.
TangoErrorType TangoSupport_createPointCloudManager(
    size_t max_points, TangoSupportPointCloudManager** manager);

/// @brief Delete the point cloud manager object.
///
/// @param manager A handle to the manager to delete.
/// @return A TangoErrorType value of @c TANGO_SUCCESS on free.
TangoErrorType TangoSupport_freePointCloudManager(
    TangoSupportPointCloudManager* manager);

/// @brief Updates the back buffer of the manager. Can be safely called from
///   the callback thread. Update is skipped if point cloud is empty.
///
/// @param manager A handle to the point cloud manager.
/// @param point_cloud New point cloud data from the camera callback thread.
/// @return A TangoErrorType value of @c TANGO_INVALID if manager
///   or point_cloud are NULL. Returns @c TANGO_SUCCESS if update
///   is successful.
TangoErrorType TangoSupport_updatePointCloud(
    TangoSupportPointCloudManager* manager, const TangoPointCloud* point_cloud);

/// @brief Check if updated point cloud data is available. If so, swap new data
///   to the front buffer and set latest_point_cloud to point to the front
///   buffer. This should be called from a single computation or render thread.
///
/// @param manager A handle to the point cloud manager.
/// @param point_cloud After the call contains a pointer to the most recent
///   depth camera buffer.
/// @return @c TANGO_SUCCESS on successful assignment, @c TANGO_INVALID if
///   @p manager is NULL.
TangoErrorType TangoSupport_getLatestPointCloud(
    TangoSupportPointCloudManager* manager,
    TangoPointCloud** latest_point_cloud);

/// @brief Check if updated point cloud data is available. If so, swap new data
///   to the front buffer and set latest_point_cloud to point to the front
///   buffer. This should be called from a single computation or render thread.
///   Set @p new_data to true if latest_point_cloud points to new point cloud.
///
/// @param manager A handle to the point cloud manager.
/// @param point_cloud After the call contains a pointer to the most recent
///   depth camera buffer.
/// @param new_data True if latest_point_cloud points to new data. False
///   otherwise.
/// @return @c TANGO_SUCCESS on successful assignment, @c TANGO_INVALID if
///   @p manager is NULL.
TangoErrorType TangoSupport_getLatestPointCloudAndNewDataFlag(
    TangoSupportPointCloudManager* manager,
    TangoPointCloud** latest_point_cloud, bool* new_data);

/// @}

/// @defgroup TransformationSupport Transformation Support
/// @brief Functions for supporting easy transformation between different
///   frames.
/// @{

/// @brief Coordinate conventions supported by the Tango Support API.
typedef enum {
  /// OpenGL coordinate convention.
  TANGO_SUPPORT_COORDINATE_CONVENTION_OPENGL,
  /// Unity3D coordinate convention.
  TANGO_SUPPORT_COORDINATE_CONVENTION_UNITY,
  /// Tango start of service or area description coordinate convention.
  TANGO_SUPPORT_COORDINATE_CONVENTION_TANGO,
} TangoCoordinateConventionType;

// Enumeration of support engines. Every engine conversion
// corresponds to an axis swap from the Tango-native frame
typedef enum {
  // Tango native frame, has a different convention
  // for forward, right, and up
  // for each reference frame
  TANGO_SUPPORT_ENGINE_TANGO,

  // OpenGL frame, -Z forward, X right, Y up
  TANGO_SUPPORT_ENGINE_OPENGL,

  // Unity frame, +Z forward, X, right, Y up
  TANGO_SUPPORT_ENGINE_UNITY,

  // UnrealEngine frame, X forward, Y right, Z up
  TANGO_SUPPORT_ENGINE_UNREAL,

  // etc.
} TangoSupportEngineType;

/// @brief Struct to hold transformation float matrix and its metadata.
typedef struct TangoMatrixTransformData {
  /// Timestamp of the time that this pose estimate corresponds to.
  double timestamp;

  /// Matrix in column major order.
  float matrix[16];

  /// The status of the pose, according to the pose lifecycle.
  TangoPoseStatusType status_code;
} TangoMatrixTransformData;

/// @brief Struct to hold transformation double matrix and its metadata.
typedef struct TangoDoubleMatrixTransformData {
  /// Timestamp of the time that this pose estimate corresponds to.
  double timestamp;

  /// Matrix in column major order.
  double matrix[16];

  /// The status of the pose, according to the pose lifecycle.
  TangoPoseStatusType status_code;
} TangoDoubleMatrixTransformData;

// Display orientation. This is a relative orientation between
// default (natural) screen orientation and current screen orientation.
// The orientation is calculated based on a clockwise rotation.
//
// The index number mirrors Android display rotation constant. This
// allows the developer to directly pass in the index value returned from
// Android.
typedef enum {
  // 0 degree rotation (natural orientation)
  ROTATION_0,

  // 90 degree rotation.
  ROTATION_90,

  // 90 degree rotation.
  ROTATION_180,

  // 270 degree rotation.
  ROTATION_270
} TangoSupportDisplayRotation;

/// @brief Calculates the relative pose from the target frame at time
///   target_timestamp to the base frame at time base_timestamp.
///
/// @param base_timestamp The timestamp for base frame position. Must be
///   non-negative. If set to 0.0, the most recent pose estimate is used.
/// @param base_frame the coordinate frame type of target frame. Must be
///   TANGO_COORDINATE_FRAME_CAMERA_*.
/// @param target_timestamp The timestamp for target frame position. Must be
///   non-negative. If set to 0.0, the most recent pose estimate is used.
/// @param target_frame The coordinate frame type of base frame. Must be
///   TANGO_COORDINATE_FRAME_CAMERA_*.
/// @param base_frame_T_target_frame A TangoPoseData object with the calculated
///   orientation and translation. The output represents the transform from
///   target frame to base frame.
/// @return A TangoErrorType value of @c TANGO_SUCCESS on successful
///   calculation, @c TANGO_INVALID if inputs are not supported, or
///   @c TANGO_ERROR if an internal transform cannot be calculated.
TangoErrorType TangoSupport_calculateRelativePose(
    double base_timestamp, TangoCoordinateFrameType base_frame,
    double target_timestamp, TangoCoordinateFrameType target_frame,
    TangoPoseData* base_frame_T_target_frame);

/// @brief Get a pose at a given timestamp from the base to the target frame
///   using the specified engine's coordinate system conventions.
///
/// @param timestamp Time specified in seconds. This behaves the same as the
///   @p timestamp parameter in @c TangoService_getPoseAtTime.
/// @param base_frame The base frame of reference to use in the query.
/// @param target_frame The target frame of reference to use in the query.
/// @param engine The coordinate system conventions to use for the @p pose data.
///   Can be OpenGL, Unity, or Tango.
/// @param display_rotation_type The index of the display rotation between
///   display's default (natural) orientation and current orientation.
/// @param pose The pose of target with respect to base frame of reference,
///   accounting for the specified engine and display rotation.
/// @return @c TANGO_SUCCESS on success, @c TANGO_INVALID on invalid input,
///   and @c TANGO_ERROR on failure.
TangoErrorType TangoSupport_getPoseAtTime(
    double timestamp, TangoCoordinateFrameType base_frame,
    TangoCoordinateFrameType target_frame, TangoSupportEngineType engine,
    TangoSupportDisplayRotation display_rotation_type, TangoPoseData* pose);

/// @brief Calculate the tranformation matrix between specified frames and
///   engine types. The output matrix uses floats and is in column-major order.
///
/// @param timestamp The timestamp of the transformation matrix of interest.
/// @param base_frame The frame of reference the matrix converts to.
/// @param target_frame The frame of reference the matrix converts from.
/// @param base_engine Specifies the final orientation convention the matrix
///   converts to.
/// @param target_engine Specifies the original orientation convention the
///   matrix converts from.
/// @param display_rotation_type Specifies how the display screen is
///   oriented. NOT YET UTILIZED.
/// @param matrix_transform The final matrix output with metadata.
/// @return @c TANGO_INVALID on invalid input. @c TANGO_ERROR if the
///   pose calculation returns error. @c TANGO_SUCCESS otherwise.
TangoErrorType TangoSupport_getMatrixTransformAtTime(
    double timestamp, TangoCoordinateFrameType base_frame,
    TangoCoordinateFrameType target_frame, TangoSupportEngineType base_engine,
    TangoSupportEngineType target_engine,
    TangoSupportDisplayRotation display_rotation_type,
    TangoMatrixTransformData* matrix_transform);

/// @brief Calculate the tranformation matrix between specified frames and
///   engine types. The output matrix uses doubles and is in column-major order.
///
/// @param timestamp The timestamp of the transformation matrix of interest.
/// @param base_frame The frame of reference the matrix converts to.
/// @param target_frame The frame of reference the matrix converts from.
/// @param base_engine Specifies the final orientation convention the matrix
///   converts to.
/// @param target_engine Specifies the original orientation convention the
///   matrix converts from.
/// @param display_rotation_type Specifies how the display screen is
///   oriented. NOT YET UTILIZED.
/// @param matrix_transform The final matrix output with metadata.
/// @return @c TANGO_INVALID on invalid input. @c TANGO_ERROR if the
///   pose calculation returns error. @c TANGO_SUCCESS otherwise.
TangoErrorType TangoSupport_getDoubleMatrixTransformAtTime(
    double timestamp, TangoCoordinateFrameType base_frame,
    TangoCoordinateFrameType target_frame, TangoSupportEngineType base_engine,
    TangoSupportEngineType target_engine,
    TangoSupportDisplayRotation display_rotation_type,
    TangoDoubleMatrixTransformData* matrix_transform);

/// @brief Multiplies a point by a matrix. No projective divide is done, the W
///   component is dropped. We explicitly support the case where point == out to
///   do an in-place transform.
///
/// @param matrix_transform The matrix the point is multiplied by.
/// @param point The original point.
/// @param out The ouput point.
/// @return @c TANGO_INVALID on invalid input; @c TANGO_SUCCESS otherwise.
TangoErrorType TangoSupport_doubleTransformPoint(
    const double matrix_transform[16], const double point[3], double out[3]);

/// @brief Multiplies a pose (represented as a position and a quaternion) by a
///   matrix. No projective divide is done, the W component is dropped. We
///   explicitly support the case where point == out to do an in-place
///   transform.
///
/// @param matrix_transform The matrix the pose is transformed by.
/// @param position The original pose's translation component.
/// @param quaternion The original pose's rotation component.
/// @param out_position The final pose's translation component.
/// @param out_quaternion The final pose's rotation component.
/// @return @c TANGO_INVALID on invalid input; @c TANGO_SUCCESS otherwise.
TangoErrorType TangoSupport_doubleTransformPose(
    const double matrix_transform[16], const double position[3],
    const double quaternion[4], double out_position[3],
    double out_quaternion[4]);

/// @brief Multiplies a point cloud (represented as a TangoPointCloud) by a
///   matrix. No projective divide is done, the W component is dropped. We
///   explicitly support the case where point == out to do an in-place
///   transform. The points in the output point cloud must be allocated before
///   calling this function.
///
/// @param matrix_transform The matrix all the points are transformed by.
/// @param point_cloud The original point cloud.
/// @param out The point cloud after translation.
/// @return @c TANGO_INVALID on invalid input; @c TANGO_SUCCESS otherwise.
TangoErrorType TangoSupport_doubleTransformPointCloud(
    const double matrix_transform[16], const TangoPointCloud* point_cloud,
    TangoPointCloud* out);

/// @brief Multiplies a point by a matrix. No projective divide is done, the W
///   component is dropped. We explicitly support the case where point == out to
///   do an in-place transform.
///
/// @param matrix_transform The matrix the point is multiplied by.
/// @param point The original point.
/// @param out The ouput point.
/// @return @c TANGO_INVALID on invalid input; @c TANGO_SUCCESS otherwise.
TangoErrorType TangoSupport_transformPoint(const float matrix_transform[16],
                                           const float point[3], float out[3]);

/// @brief Multiplies a pose (represented as a position and a quaternion) by a
///   matrix. No projective divide is done, the W component is dropped. We
///   explicitly support the case where point == out to do an in-place
///   transform.
///
/// @param matrix_transform The matrix the pose is transformed by.
/// @param position The original pose's translation component.
/// @param quaternion The original pose's rotation component.
/// @param out_position The final pose's translation component.
/// @param out_quaternion The final pose's rotation component.
/// @return @c TANGO_INVALID on invalid input; @c TANGO_SUCCESS otherwise.
TangoErrorType TangoSupport_transformPose(const float matrix_transform[16],
                                          const float position[3],
                                          const float quaternion[4],
                                          float out_position[3],
                                          float out_quaternion[4]);

/// @brief Multiplies a point cloud (represented as a TangoPointCloud) by a
///   matrix. No projective divide is done, the W component is dropped. We
///   explicitly support the case where point == out to do an in-place
///   transform. The points in the output point cloud must be allocated
///   before calling this function.
///
/// @param matrix_transform The matrix all the points are transformed by.
/// @param point_cloud The original point cloud.
/// @param out The point cloud after translation.
/// @return @c TANGO_INVALID on invalid input; @c TANGO_SUCCESS otherwise.
TangoErrorType TangoSupport_transformPointCloud(
    const float matrix_transform[16], const TangoPointCloud* point_cloud,
    TangoPointCloud* out);

/// @}

/// @defgroup ProjectionSupport Projection and Unprojection Functions
/// @brief Functions for projecting points or unprojecting pixels.
/// @{

/// @brief Calculates the pixel coordinates in the camera frame of a
///   user-specified 3D point expressed in the same camera frame, accounting for
///   intrinsic distortion.
///
/// @param camera_id The camera id the intrinsics are being queried for.
/// @param camera_point The point (x, y, z) to be projected, expressed in the
///   camera frame. Cannot be NULL.
/// @param pixel_coordinates The pixel coordinates of the projected point.
///   This is between (0.0, 0.0) and (width, height) if the point projects into
///   the image. Cannot be NULL.
/// @param is_distorted_pixel_in_image A flag valued 1 if the projected pixel
///   coordinate are valid and inside the image (between 0 and width/height).
///   0 otherwise.
/// @return @c TANGO_SUCCESS on success or @c TANGO_INVALID on invalid input.
TangoErrorType TangoSupport_projectCameraPointToDistortedPixel(
    const TangoCameraId camera_id, const float camera_point[3],
    float pixel_coordinates[2], int* is_distorted_pixel_in_image);

/// @brief Calculates camera ray in the camera frame of a user-specified pixel
///   expressed in the distorted image, accounting for intrinsic distortion.
///
/// @param camera_id The camera id the intrinsics are being queried for.
/// @param pixel_coordinates The pixel coordinates of the point to unproject.
///   This normally is between (0.0, 0.0) and (width, height).
///   Cannot be NULL.
/// @param camera_ray The ray (x, y, 1.0) corresponding to the pixel after
///   undistortion, expressed in the camera frame. Cannot be NULL.
/// @return @c TANGO_SUCCESS on success or @c TANGO_INVALID on invalid input for
///   the distortion model
TangoErrorType TangoSupport_DistortedPixelToCameraRay(
    const TangoCameraId camera_id, const float pixel_coordinates[2],
    float camera_ray[3]);

/// @}

/// @defgroup DepthInterpolationSupport Depth Interpolation Support Functions
/// @brief Functions for interpolating depth.
/// @{

/// @brief Calculates the depth in the color camera space at a user-specified
///   location using nearest-neighbor interpolation.
///
/// @param point_cloud The point cloud. Cannot be NULL and must have at least
///   one point.
/// @param color_camera_T_point_cloud The pose of the point cloud relative to
///   the color camera used to obtain uv_coordinates.
/// @param uv_coordinates The UV coordinates for the user selection. This is
///   expected to be between (0.0, 0.0) and (1.0, 1.0) and can be computed from
///   pixel coordinates by dividing by the width or height. Cannot be NULL.
/// @param color_camera_point The point (x, y, z), where (x, y) is the
///   back-projection of the UV coordinates to the color camera space and z is
///   the z coordinate of the point in the point cloud nearest to the user
///   selection after projection onto the image plane. If there is not a point
///   cloud point close to the user selection after projection onto the image
///   plane, then the point will be set to (0.0, 0.0, 0.0) and @c TANGO_ERROR
///   will be returned.
/// @return @c TANGO_SUCCESS on success, @c TANGO_ERROR if a valid point is not
///   found, or @c TANGO_INVALID on invalid input.
TangoErrorType TangoSupport_getDepthAtPointNearestNeighbor(
    const TangoPointCloud* point_cloud,
    const TangoPoseData* color_camera_T_point_cloud,
    const float uv_coordinates[2], float color_camera_point[3]);

/// @brief The TangoSupportDepthInterpolator contains references to camera
///   intrinsics and cached data structures needed to upsample depth data to
///   a camera image.
struct TangoSupportDepthInterpolator;

/// @brief Create an object for depth interpolation.
///
/// @param interpolator A handle to the interpolator object.
/// @return @c TANGO_SUCCESS on successful creation, or @c TANGO_INVALID if
///   @p intrinsics was null.
TangoErrorType TangoSupport_createDepthInterpolator(
    TangoSupportDepthInterpolator** interpolator);

/// @brief Free the depth interpolation object.
///
/// @param A handle to the interpolator object.
/// @return @c TANGO_SUCCESS
TangoErrorType TangoSupport_freeDepthInterpolator(
    TangoSupportDepthInterpolator* interpolator);

/// @brief Calculates the depth in the color camera space at a user-specified
///   location using bilateral filtering weighted by both spatial distance from
///   the user coordinate and by intensity similarity.
///
/// @param interpolator A handle to the interpolator object. The intrinsics of
///   this interpolator object must match those of the image_buffer.
/// @param point_cloud The point cloud. Cannot be NULL and must have at least
///   one point.
/// @param image_buffer The RGB image buffer. This must have intrinsics matching
///   those used to create the interpolator object. Cannot be NULL.
/// @param color_camera_T_point_cloud The pose of the point cloud relative to
///   the color camera used to obtain uv_coordinates.
/// @param uv_coordinates The UV coordinates for the user selection. This is
///   expected to be between (0.0, 0.0) and (1.0, 1.0) and can be computed from
///   pixel coordinates by dividing by the width or height. Cannot be NULL.
/// @param color_camera_point The point (x, y, z), where (x, y) is the
///   back-projection of the UV coordinates to the color camera space and z is
///   the bilateral interpolation of the z coordinate of the point. If there is
///   not a point cloud point close to the user selection after projection onto
///   the image plane, then the point will be set to (0.0, 0.0, 0.0) and
///   @c TANGO_ERROR will be returned.
/// @return @c TANGO_SUCCESS on success, @c TANGO_ERROR if a valid point is not
///   found, or @c TANGO_INVALID on invalid input.
TangoErrorType TangoSupport_getDepthAtPointBilateral(
    const TangoSupportDepthInterpolator* interpolator,
    const TangoPointCloud* point_cloud, const TangoImageBuffer* image_buffer,
    const TangoPoseData* color_camera_T_point_cloud,
    const float uv_coordinates[2], float color_camera_point[3]);

/// @brief A structure to hold depth values for image upsampling. The units of
///   the depth are the same as for @c TangoPointCloud.
struct TangoSupportDepthBuffer {
  float* depths;
  uint32_t width;
  uint32_t height;
};

/// @brief Allocate memory for a depth buffer for the given image resolution.
///
/// @param width The width of the image. This should match the width given by
///   the camera intrinsics.
/// @param height The height of the image. This should match the height given by
///   the camera intrinsics.
/// @param depth_buffer The depth buffer to initialize.
/// @return @c TANGO_SUCCESS on success, @c TANGO_INVALID on invalid input, and
///   @c TANGO_ERROR on failure.
TangoErrorType TangoSupport_initializeDepthBuffer(
    uint32_t width, uint32_t height, TangoSupportDepthBuffer* depth_buffer);

/// @brief Free memory for the depth buffer.
///
/// @param depth_buffer The depth buffer to free.
/// @return @c TANGO_SUCCESS on success, @c TANGO_INVALID on invalid input, and
///   @c TANGO_ERROR on failure.
TangoErrorType TangoSupport_freeDepthBuffer(
    TangoSupportDepthBuffer* depth_buffer);

/// @brief Upsamples the depth data to the resolution of the color image. This
///   uses the resolution specified by the intrinsics used to construct the
///   interpolator. This function fills depth around each sample using a fixed
///   radius. The resolution of the intrinsics provided to the interpolator and
///   the the resolution of the output depth_buffer must match.
///
/// @param interpolator A handle to the interpolator object. The intrinsics of
///   this interpolator object must match those of the image_buffer. Cannot be
///   NULL.
/// @param point_cloud The point cloud. Cannot be NULL and must have at least
///   one point.
/// @param color_camera_T_point_cloud The pose of the point cloud relative to
///   the color camera used to obtain uv_coordinates. Cannot be NULL.
/// @param depth_buffer A buffer for output of the depth data. Each pixel
///   contains a depth value (in meters) or zero if there is no depth data near
///   enough to the pixel. Cannot be NULL.
/// @return @c TANGO_SUCCESS on success, @c TANGO_INVALID on invalid input, and
///   @c TANGO_ERROR on failure.
TangoErrorType TangoSupport_upsampleImageNearestNeighbor(
    const TangoSupportDepthInterpolator* interpolator,
    const TangoPointCloud* point_cloud,
    const TangoPoseData* color_camera_T_point_cloud,
    TangoSupportDepthBuffer* depth_buffer);

/// @brief Upsamples the depth data to the resolution of the color image. This
///   uses the resolution specified by the intrinsics used to construct the
///   interpolator. This function fills depth around each sample using a
///   bilateral filtering approach. The resolution of the intrinsics provided
///   to the interpolator and the resolution of the output depth_buffer must
///   match.
///
/// @param interpolator A handle to the interpolator object. The intrinsics of
///   this interpolator object must match those of the image_buffer. Cannot be
///   NULL.
/// @param approximate If non-zero, uses an approximation technique that is
///   faster but somewhat less accurate. If zero, use the a slower technique
///   that is slightly more accurate.
/// @param point_cloud The point cloud. Cannot be NULL and must have at least
///   one point.
/// @param color_camera_T_point_cloud The pose of the point cloud relative to
///   the color camera used to obtain uv_coordinates. Cannot be NULL.
/// @param depth_buffer A buffer for output of the depth data. Each pixel
///   contains a depth value (in meters) or zero if there is no depth data near
///   enough to the pixel. Cannot be NULL.
/// @return @c TANGO_SUCCESS on success, @c TANGO_INVALID on invalid input, and
///   @c TANGO_ERROR on failure.
TangoErrorType TangoSupport_upsampleImageBilateral(
    const TangoSupportDepthInterpolator* interpolator, int approximate,
    const TangoPointCloud* point_cloud, const TangoImageBuffer* image_buffer,
    const TangoPoseData* color_camera_T_point_cloud,
    TangoSupportDepthBuffer* depth_buffer);

/// @brief Finds a similarity transformation (rotation, translation, and
///   scaling) given two sets of correspondence points. This uses the Umeyama
///   algorithm (http://www.cis.jhu.edu/software/lddmm-similitude/umeyama.pdf)
///   which minimizes the mean squared error. The returned transform is stored
///   in column-major order.
///
/// NOTE: If less than three non-collinear points are passed then this will
/// return one of the many possible transforms that make that correspondence.
///
/// @param src_points An array of 3D source points.
/// @param dest_points An array of 3D destination points.
/// @param num_points Number of correspondence points.
/// @param src_frame_T_dest_frame_matrix An array for output of the
///   transformation.
/// @return @c TANGO_SUCCESS on success, @c TANGO_INVALID on invalid input, and
///   @c TANGO_ERROR on failure.
TangoErrorType TangoSupport_findCorrespondenceSimilarityTransform(
    double (*src_points)[3], double (*dest_points)[3], int num_points,
    double src_frame_T_dest_frame_matrix[16]);

/// @}

/// @defgroup EdgeDetectionSupport Edge Detection Support Functions
/// @brief Functions for detecting edges.
/// @{

/// @brief A structure to define an edge, including the closest point on the
/// edge to the input point to the method that returned the edge.
struct TangoSupportEdge {
  float end_points[2][3];
  float closest_point_on_edge[3];
};

/// @brief Find the list of edges "close" to the user-specified location and
///   that are on the plane estimated from the input location. The edges are
///   detected in the color camera image and specified in the depth frame.
///
/// @param point_cloud The point cloud. Cannot be NULL and must have sufficient
///   points to estimate the plane at the location of the input.
/// @param image_buffer The RGB image buffer. Cannot be NULL.
/// @param uv_coordinates The UV coordinates for the user selection. This is
///   expected to be between (0.0, 0.0) and (1.0, 1.0) and can be computed from
///   pixel coordinates by dividing by the width or height. Cannot be NULL.
/// @param edges An array of 3D edges close to the input point and specified in
///   the depth frame. The edges will lie on the plane estimated at the location
///   of the input point. The array should be deleted by calling
///   TangoSupport_freeEdgeList. Cannot be NULL.
/// @param number_of_edges The number of edges in @p edges. Cannot be NULL.
/// @return @c TANGO_SUCCESS on success, @c TANGO_INVALID on invalid input, and
///   @c TANGO_ERROR on failure.
TangoErrorType TangoSupport_findEdgesNearPoint(
    const TangoPointCloud* point_cloud, const TangoImageBuffer* image_buffer,
    const float uv_coordinates[2], TangoSupportEdge** edges,
    int* number_of_edges);

/// @brief Free memory allocated in call to TangoSupport_findEdgesNearPoint.
///
/// @param edges Edge list to free.
/// @return @c TANGO_SUCCESS on success.
TangoErrorType TangoSupport_freeEdgeList(TangoSupportEdge** edges);

/// @}

/// @defgroup VolumeSupport Volume Measurement Support Functions
/// @brief Functions for measuring volumes.
/// @{

/// struct for storing a Tango volume object
struct TangoSupportVolume;

/// @brief Create a volume measurement object.
/// @return Pointer to a volume measurement object.
TangoSupportVolume* TangoSupport_createVolume();

/// @brief Delete a volume measurement created by TangoSupport_createVolume().
/// @param volume Pointer to the volume object to be destroyed.
void TangoSupport_deleteVolume(TangoSupportVolume** volume);

/// @brief Add the reference plane for the volume measurement. The reference
///   plane is a surface the volume object is on. This routine must be called
///   before calling TangoSupport_addSeedPointToVolume() or
///   TangoSupport_addPointCloudToVolume() methods.
///
/// NOTE: This function expects a transformation from the input frame to the
/// output frame. The output frame has to be a right-hand 3D world frame and
/// the same output frame should be used throughout the calls to "this"
/// volume measurement.
///
/// @param volume Pointer to the volume measurement object.
/// @param plane_model Plane model specified in the input frame.
/// @param translation Translation component of the transformation from the
///   input frame to the output frame.
/// @param orientation Rotation component (as a quaternion) of the
///   transformation from the input frame to the output frame.
/// @return @c TANGO_SUCCESS on success and @c TANGO_INVALID on invalid input.
TangoErrorType TangoSupport_addReferencePlaneToVolume(
    TangoSupportVolume* volume, const double plane_model[4],
    const double translation[3], const double orientation[4]);

/// @brief Add a seed point to the volume measurement. Seed points are used as
///   starting locations when searching point clouds for volume object
///   boundaries. A seed point by itself is also used to define the volume
///   boundary. The reference plane of the volume must be specified by
///   TangoSupport_addReferencePlaneToVolume() method before calling this
///   routine.
///
/// NOTE: This function expects a transformation from the input frame to the
/// output frame. The output frame has to be a right-hand 3D world frame and
/// the same output frame should be used throughout the calls to "this"
/// volume measurement.
///
/// @param volume Pointer to the volume measurement object.
/// @param seed_point The seed point specified in the input frame.
/// @param translation Translation component of the transformation from the
///   input frame to the output frame.
/// @param orientation Rotation component (as a quaternion) of the
///   transformation from the input frame to the output frame.
/// @return @c TANGO_SUCCESS on success, @c TANGO_INVALID on invalid input, and
///   @c TANGO_ERROR on failure.
TangoErrorType TangoSupport_addSeedPointToVolume(TangoSupportVolume* volume,
                                                 const float seed_point[3],
                                                 const double translation[3],
                                                 const double orientation[4]);

/// @brief Add a point cloud to the volume measurement. Point clouds are used
///   to grow the volume from input seed points to the boundary of the object.
///   This routine should be called after at least one call to
///   TangoSupport_addSeedPointCloudToVolume() method.
///
/// NOTE: This function expects a transformation from the input frame to the
/// output frame. The output frame has to be a right-hand 3D world frame and
/// the same output frame should be used throughout the calls to "this"
/// volume measurement.
///
/// @param volume Pointer to the volume measurement object.
/// @param point_cloud Point cloud specified in the input frame.
/// @param translation Translation component of the transformation from the
///   input frame to the output frame.
/// @param orientation Rotation component (as a quaternion) of the
///   transformation from the input frame to the output frame.
/// @return @c TANGO_SUCCESS on success, @c TANGO_INVALID on invalid input, and
///   @c TANGO_ERROR on failure.
TangoErrorType TangoSupport_addPointCloudToVolume(
    TangoSupportVolume* volume, const TangoPointCloud* point_cloud,
    const double translation[3], const double orientation[4]);

/// @brief Get the current volume measurement information, including both the
///   volume size and the 8 corner points of the volume.
///
/// NOTE: The volume_points are in the output frame specified by previous call
/// of TangoSupport_addReferencePlaneToVolume(),
/// TangoSupport_addSeedPointToVolume() and TangoSupport_addPointCloudToVolume()
/// methods. It is important that the output frame specified in those routines
/// is consistent.
///
/// @param volume Pointer to the volume measurement object.
/// @param volume_size The size of the volume, in mm^3.
/// @param volume_points Array of points defining the oriented bounding-box of
///   the detected volume in the output frame.
/// @return @c TANGO_SUCCESS on success, @c TANGO_INVALID on invalid input, and
///   @c TANGO_ERROR on failure.
TangoErrorType TangoSupport_getVolumeOutput(TangoSupportVolume* volume,
                                            float* volume_size,
                                            float volume_points[8][3]);

/// @}

/// @defgroup CornerDetectionSupport Corner Detection Support Functions
/// @brief Functions for detecting corners
/// @{

/// @brief A structure to define a corner, including the corner point, and
///   indices to edges that are associated to the corner. The edges are input
///   parameters to TangoSupport_detectCorners() routine.
struct TangoSupportCorner {
  float corner_point[3];
  float distance_to_poi;
  int* edges;
  int edge_count;
};

/// @brief A structure that stores a list of corners. After calling
///   TangoSupport_detectCorners() with a TangoSupportCornerList object, the
///   object needs to be released by calling TangoSupport_freeCornerList()
///   function.
struct TangoSupportCornerList {
  TangoSupportCorner* corners;
  int corner_count;
};

/// @brief Detect corners among a list of edges.
///
/// @param point_of_interest The user-specified location.
/// @param edges A list of edges, usually returned by
///   TangoSupport_findEdgesNearPoint() routine.
/// @param number_of_edges The number of edges in point_of_interest list.
/// @param corner_list The structure to hold result corners. The structure
///   should be deleted by calling TangoSupport_freeCornerList. Cannot be NULL.
/// @return @c TANGO_SUCCESS on success, @c TANGO_INVALID on invalid input, and
///   @c TANGO_ERROR on failure.
TangoErrorType TangoSupport_detectCorners(const float point_of_interest[3],
                                          const TangoSupportEdge** edges,
                                          const int number_of_edges,
                                          TangoSupportCornerList* corner_list);

/// @brief Free memory allocated in TangoSupport_detectCorners.
///
/// @param Corner list to free.
/// @return @c TANGO_SUCCESS on success.
TangoErrorType TangoSupport_freeCornerList(TangoSupportCornerList* corner_list);

/// @}
#ifdef __cplusplus
}
#endif

#endif  // TANGO_SUPPORT_API_H_
