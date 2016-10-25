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

// ==================================================================================
// VRDisplay
// ==================================================================================

/**
* @name VRDisplay
* @class
* @description WebAR devices will be exposed as VRDisplay instances. The pose estimation is exposed using the exact same methods as in any other VR display. Some methods have been added though to the VRDisplay class of the WebVR spec {@link https://w3c.github.io/webvr/#interface-vrdisplay}.
*/

/**
* @method VRDisplay#getMaxPointCloudVertexCount
* @description Returns the maximum number of points/vertices that the VRDisplay is able to represent. This value will be bigger than 0 only if the VRDisplay is able to provide a point cloud. 
* @see VRDisplayCapabilities
* @returns {long} - The maximum number of points/vertices that the VRDisplay is able to represent (0 if the underlying VRDisplay does not support point cloud provisioning). 
*/

/**
* @method VRDisplay#getPointCloud
* @description Returns an instance of {@link VRPointCloud} that represents the point cloud acquired by the underlying hardware at the moment of the call.
* @see VRDisplayCapabilities
* @returns {VRPointCloud} - The maximum number of points/vertices that the VRDisplay is able to represent or null if the underlying VRDisplay is does not support point cloud provisioning.
*/

/**
* @method VRDisplay#getPickingPointAndPlaneInPointCloud
* @description Returns an instance of {@link VRPickingPointAndPlane} that represents a point and a plane normal defined at the collision point in 3D between the underlying point cloud and the given 2D point of the screen. The returned value will always be null if the underlying VRDisplay does not support point cloud provisioning. The internal algorithm will use the provide 2D point to cast a ray against the point cloud and return the collision point and normal of the plane in the point cloud mesh.
* @see VRDisplayCapabilities
* @param {float} x - The horizontal normalized value (0-1) of the screen position.
* @param {float} y - The vertival normalized value (0-1) of the screen position.
* @returns {VRPickingPointAndPlane} - An instance of a {@link VRPickingPointAndPlane} to represent the collision point and plane normal of the ray traced from the passed (x, y) 2D position into the 3D mesh represented by the point cloud.
*/

/**
* @method VRDisplay#getSeeThroughCamera
* @description Returns an instance of {@link VRSeeThroughCamera} that represents a see through camera (both for AR or VR). The underlying VRDisplay needs to be able to provide such a camera or this method will return null.
* @see VRDisplayCapabilities
* @returns {VRSeeThroughCamera} - An instance of a {@link VRSeeThroughCamera} to represent a see through camera or null if no camera is supported.
*/

// ==================================================================================
// ==================================================================================

/**
* @name VRPickingPointAndPlane
* @class
* @description A class that represents the point where a collision has occured between a ray and the VRPointCloud mesh and the normal of the plane of that same collision.
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

/**
* @name VRPickingPointAndPlane#correctValues
* @type {boolean}
* @description A flag that indicates if the values of the point and plane are correct (true) or no collision was found between the casted ray and the point cloud mesh (false).
* @readonly
*/

// ==================================================================================
// VRPointCloud
// ==================================================================================

/**
* @name VRPointCloud
* @class
* @description A class that represents the point cloud acquired by the underlying VRDisplay. A point cloud is just a set of triplets that represent each 3D position of each vertex/point in the point cloud. In order to make this structure as fast as possible, the Float32Array is always of the maximum vertex count possible depending on the underlywing VRDisplay. Of course, the exact number of points that have been correctly acquired is also provided in the VRPointCloud instance.
*/

/**
* @name VRPickingPointAndPlane#vertexCount
* @type {long}
* @description The real number of vertices in the point cloud that have been identified by the underlying system.
* @readonly
*/

/**
* @name VRPickingPointAndPlane#vertices
* @type {Float32Array}
* @description An array of triplets representing each 3D vertices of the point cloud.
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
* @description The horizontal size of the texture used to hold each camera frame. I can be the same or a bigger value than the camera width.
* @readonly
*/

/**
* @name VRSeeThroughCamera#textureHeight
* @type {long}
* @description The vertical size of the texture used to hold each camera frame. I can be the same or a bigger value than the camera height.
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
