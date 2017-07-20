/*
 * Copyright 2017 Google Inc. All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// ==================================================================================
// VRDisplayCapabilities
// ==================================================================================

/**
* @name VRDisplayCapabilities
* @class 
* @description Added some properties to the VRDisplayCapabilities class of the WebVR spec {@link https://w3c.github.io/webvr/#interface-vrdisplay}.
*/

/**
* @name VRDisplayCapabilities#hasPointCloud
* @type {boolean}
* @description A flag that indicates if the VRDisplay is able to provide a point cloud (true) or not (false).
* @readonly
*/

/**
* @name VRDisplayCapabilities#hasSeeThroughCamera
* @type {boolean}
* @description A flag that indicates if the VRDisplay is able to provide a VRSeeThroughCamera instance (true) or not (false).
* @readonly
*/

/**
* @name VRDisplayCapabilities#hasADFSupport
* @type {boolean}
* @description A flag that indicates if the VRDisplay is able to support Area Description Files (true) or not (false).
* @readonly
* @see VRADF
*/

/**
* @name VRDisplayCapabilities#hasMarkerSupport
* @type {boolean}
* @description A flag that indicates if the VRDisplay is able to support marker detection (true) or not (false). 
* @readonly
* @see VRMarker
*/

// ==================================================================================
// VRDisplay
// ==================================================================================

/**
* @name VRDisplay
* @class
* @description WebAR devices will be exposed as VRDisplay instances. The pose estimation is exposed using the exact same methods as in any other VR display, although in the case of the Tango underlying implementation, the pose will be 6DOF (position and orientation). Some new methods have been added though to the VRDisplay class of the WebVR spec to provide new functionalities {@link https://w3c.github.io/webvr/#interface-vrdisplay}.
*/

/**
* @method VRDisplay#getMaxNumberOfPointsInPointCloud
* @description Returns the maximum number of points/vertices that the VRDisplay is able to represent. This value will be bigger than 0 only if the VRDisplay is able to provide a point cloud. 
* @see VRDisplayCapabilities
* @returns {long} - The maximum number of points/vertices that the VRDisplay is able to represent (0 if the underlying VRDisplay does not support point cloud provisioning). 
*/

/**
* @method VRDisplay#getPointCloud
* @description Updates an instance of the {@link VRPointCloud} structure that represents the point cloud acquired by the underlying hardware at the moment of the call. This process is similar to how the WebVR 1.1 spec requires to update a VRFrameData instance in order to get a new pose.
* @see VRDisplayCapabilities
* @param {VRPointCloud} pointCloud - The {@link VRPointCloud} instance to be updated in this call.
* @param {boolean} justUpdatePointCloud - A flag to indicate if the whole point cloud should be retrieved or just updated internally. Updating the point cloud without retrieving the points may be useful if the point cloud won't be used in JS (for rendering it, for exmaple) but picking will be used. This parameter should be true to only update the point cloud returning 0 points and false to both update and return all the points detected up until the moment of the call.
* @param {number} pointsToSkip - An integer value to indicate how many points to skip when all the points are returned (justUpdatePointCloud = false). This parameter allows to return a less dense point cloud by skipping 1, 2, 3, ... points. A value of 0 will return all the points. A value of 1 will skip every other point returning half the number of points (1/2), a value of 2 will skip 2 of every other points returning one third of the number of points (1/3), etc. In essence, this value will specify the number of point to return skipping some points. numberOfPointsToReturn = numberOfDetectedPoints / (pointsToSkip + 1). 
* @param {boolean} transformPoints - A flag to indicate that the resulting points should be transformed in the native side. In the case the points are transformed in the native side, the VRPointCloud structure will return an identity pointsTransformMatrix and a true pointsAlreadyTransformed. On the contrary, if the points are not transformed in the native side, the matrix to correctly transform them will be provided inside the VRPointCloud structure's pointsTransformMatrix and the pointsAlreadyTransformed flag will be false. 
* @returns {VRPointCloud} - An instance of a {@link VRPointCloud} with the points/vertices that the VRDisplay has detected or null if the underlying VRDisplay does not support point cloud provisioning.
*/

/**
* @method VRDisplay#getPickingPointAndPlaneInPointCloud
* @description Returns an instance of {@link VRPickingPointAndPlane} that represents a point and a plane normal defined at the collision point in 3D between the point cloud and the given 2D point of the screen. IMPORTANT: The point cloud needs to be at least updated by calling getPointCloud before calling this method. The returned value will always be null if the underlying VRDisplay does not support point cloud provisioning. The internal algorithm will use the provided 2D point to cast a ray against the point cloud and return the collision point and normal of the plane in the point cloud mesh.
* @see VRDisplayCapabilities
* @param {float} x - The horizontal normalized value (0-1) of the screen position.
* @param {float} y - The vertival normalized value (0-1) of the screen position.
* @returns {VRPickingPointAndPlane} - An instance of a {@link VRPickingPointAndPlane} to represent the collision point and plane normal of the ray traced from the passed (x, y) 2D position into the 3D mesh represented by the point cloud. null is returned if no support for point cloud is provided by the VRDisplay or if no colission has been detected.
*/

/**
* @method VRDisplay#getSeeThroughCamera
* @description Returns an instance of {@link VRSeeThroughCamera} that represents a see through camera (both for AR or VR). The underlying VRDisplay needs to be able to provide such a camera or this method will return null.
* @see VRDisplayCapabilities
* @returns {VRSeeThroughCamera} - An instance of a {@link VRSeeThroughCamera} to represent a see through camera or null if no camera is supported.
*/

/**
* @method VRDisplay#getADFs
* @description Returns an list of existing VRADF structures in the device. The ADFs can be created with other apps like the Area Description example in the Tango C Examples {@link https://github.com/googlesamples/tango-examples-c/tree/master/cpp_basic_examples/hello_area_description}
* @see VRADF
* @returns {array<VRADF>} - An array of {@link VRSeeThroughCamera} instances corresponding to the ADFs exisiting on the device.
*/

/**
* @method VRDisplay#enableADF
* @description Enable an ADF to be used to localize the pose estimation. Only one ADF can be enabled at the same time, so a previously emabled ADF is disabled when this call is made.
* @see VRADF
* @param {string} uuid - The UUID of the ADF to enable.
*/

/**
* @method VRDisplay#disableADF
* @description Disable the last enabled ADF. The pose estimation is based on the start of service from then on once an ADF is disabled.
* @see VRADF
*/

/**
* @name VRSeeThroughCamera#MARKER_TYPE_AR
* @type {long}
* @description A constant that represents the AR marker type. The value is equivalent to 0x01.
* @readonly
*/

/**
* @name VRSeeThroughCamera#MARKER_TYPE_QRCODE
* @type {long}
* @description A constant that represents the QR Code marker type. The value is equivalente to 0x02.
* @readonly
*/

/**
* @method VRDisplay#detectMarkers
* @description .
* @see VRADF
* @param {long} markerType - A number that represents the type of marker to detect. The supported types are specified as constants inside the {@link VRDisplay} MARKER_TYPE_XXX properties.
* @param {float} markerSize - The size in meters of the actual physical marker.
* @returns {array<VRMarker>} - An array of {@link VRMarker} instances corresponding to the detected markers of the specified type.
*/

// ==================================================================================
 // ==================================================================================
 +// VRPose
 +// ==================================================================================
 +
 +/**
 +* @name VRPose
 +* @class 
 +* @description Added some properties to the VRPose class of the WebVR spec {@link https://w3c.github.io/webvr/spec/1.1/#interface-vrpose}.
 +*/
 +
 +/**
 +* @name VRPose#localized
 +* @type {boolean}
 +* @description A flag that indicates if the VRPose has been localized to a (Tango-specific) ADF, and pose coordinates now use a localized space.
 +* @readonly
 +*/
 +
 +
 +// ==================================================================================
  // ==================================================================================   
  
/**
* @name VRPickingPointAndPlane
* @class
* @description A class that represents the point where a collision happened between a ray and the VRPointCloud mesh and the normal of the plane of that same collision.
*/

/**
* @name VRPickingPointAndPlane#point
* @type {Float32Array}
* @description An array of 3 values representing the 3D position of the collision point between a casted ray and the point cloud mesh.
* @readonly
*/

/**
* @name VRPickingPointAndPlane#plane
* @type {Float32Array}
* @description An array of 4 values representing the coeficients of the values in the equation of the plane where there was a collision between a casted ray and the point cloud mesh.
* @readonly
*/

// ==================================================================================
// VRPointCloud
// ==================================================================================

/**
* @name VRPointCloud
* @class
* @description A class that represents the point cloud acquired by the underlying VRDisplay when a call to getPointCloud is made. A point cloud is just a set of triplets (x, y, z) that represent each 3D position of each vertex/point in the point cloud. In order to make this structure as fast as possible, the Float32Array is always of the maximum vertex count possible depending on the underlywing VRDisplay. Of course, the exact number of points that have been correctly acquired is also provided in the VRPointCloud instance.
* NOTE: In order to improve performance, a single Float32Array instance is allocated with the maximum capacity of points that the underlying SDK could provide. This is why the numberOfPoints property is also passed along with the points property. It is up to the developer to correctly use/copy the values.
* To be able to use this structure, just create an instance of it and update it using the getPointCloud method described in the VRDisplay structure.
*/

/**
* @name VRPointCloud#numberOfPoints
* @type {long}
* @description The real number of points in the point cloud that have been identified by the underlying system.
* @readonly
*/

/**
* @name VRPointCloud#points
* @type {Float32Array}
* @description An array of triplets representing each 3D vertices of the point cloud. The size of this array is always of the maximum number of points the underlying platform can provide in order to improvde performance. The real number of points is provided in the numberOfPoints property. The remaining points when the real number of points is less than the maximum possible is filled with the maximum possible float value so they can be discarded.
* @readonly
*/

/**
* @name VRPointCloud#pointsTransformMatrix
* @type {Float32Array}
* @description An array that represents a transformation matrix (16 values) to be able to correctly transform the points in the case that when the point cloud update was requested the developer specified that the points should not be transformed in the native side. In the case the points are transformed in the native side this matrix will be an identity matrix.
* @readonly
*/

/**
* @name VRPointCloud#pointsAlreadyTransformed
* @type {boolesn}
* @description A flag that indicates if the points in the VRPointCloud have already been transformed in the native side or not. If the points have not been already transformed, the pointsTransformMatrix should be used to transform them in order to get the point cloud to be correctly rendered. If the flag is true, it means the points have already been transformed and thus, the pointsTransformMatrix is not needed (and it is actually an identity matrix).
* @readonly
*/

// ==================================================================================
// VRSeeThroughCamera
// ==================================================================================

/**
* @name VRSeeThroughCamera
* @class
* @description A class that represents a see through camera. Both VR and AR displays may provide access to a camera that can be used to show the reality around the user. This class provides information about the camera characteristics and it can also be used to render each frame acquired from the camera.
*/

/**
* @name VRSeeThroughCamera#width
* @type {long}
* @description The horizontal size of the camera.
* @readonly
*/

/**
* @name VRSeeThroughCamera#height
* @type {long}
* @description The vertical size of the camera.
* @readonly
*/

/**
* @name VRSeeThroughCamera#textureWidth
* @type {long}
* @description The horizontal size of the texture used to hold each camera frame. It can be the same or a bigger value than the camera width.
* @readonly
*/

/**
* @name VRSeeThroughCamera#textureHeight
* @type {long}
* @description The vertical size of the texture used to hold each camera frame. It can be the same or a bigger value than the camera height.
* @readonly
*/

/**
* @name VRSeeThroughCamera#focalLengthX
* @type {double}
* @description The hortizontal focal length.
* @readonly
*/

/**
* @name VRSeeThroughCamera#focalLengthY
* @type {double}
* @description The vertical focal length.
* @readonly
*/

/**
* @name VRSeeThroughCamera#pointX
* @type {double}
* @description The horizontal value that represents the center of the camera.
* @readonly
*/

/**
* @name VRSeeThroughCamera#pointY
* @type {double}
* @description The vertical value that represents the center of the camera.
* @readonly
*/

/**
* @name VRSeeThroughCamera#orientation
* @type {long}
* @description The orientation of the camera.
* @readonly
*/

// ==================================================================================
// VRADF
// ==================================================================================

/**
* @name VRADF
* @class
* @description A class that represents an Area Description File (ADF). An ADF can be
generated and stored in the current device using a different application like the 
area description example in the Tango C-Examples. An AR VRDisplay may request a list of
all the ADFs 
*/

/**
* @name VRADF#uuid
* @type {string}
* @description The unique identifier for the ADF. Use it to enable/disable an ADF.
* @readonly
*/

/**
* @name VRADF#name
* @type {string}
* @description A human readable name to easily identify and describe the content of the ADF.
* @readonly
*/

/**
* @name VRADF#creationTime
* @type {long}
* @description The time stamp in millisenconds when the ADF was created.
* @readonly
*/

// ==================================================================================
// VRMarker
// ==================================================================================

/**
* @name VRMarker
* @class
* @description A class that represents a marker. If the underlying technology supports markers, the corresponding VRDisplay allows to detect them returning instances of this type when markers are identified. The supported types of markers are specified with constants defined in the {@link VRDisplay} class as MARKER_TYPE_XXX.
*/

/**
* @name VRMarker#type
* @type {long}
* @description A number that identifies the type of a marker. The {@link VRDisplay} structure has the constants that define the supported marker types.
* @readonly
*/

/**
* @name VRMarker#id
* @type {long}
* @description A number that identifies the marker. For non QRCode markers, the id is enbedded in the physicar marker itself and this attribute provides its value. If the marker is a QRCode, there is no id and a content is provided.
* @readonly
*/

/**
* @name VRMarker#content
* @type {string}
* @description A string that represents the content of the detected marker. A content is provided inside QRCode type markers and it can represents from plain text to a URL.
* @readonly
*/

/**
* @name VRMarker#position
* @type {Float32Array}
* @description An array representing the 3D position of the detected marker.
* @readonly
*/

/**
* @name VRMarker#orientation
* @type {Float32Array}
* @description An array representing the quaternion that provides the orientation of the marker in the 3D space.
* @readonly
*/
