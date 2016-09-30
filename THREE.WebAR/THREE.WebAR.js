var THREE = THREE || require("three");

/**
* The WebAR namespace inside the THREE namespace. Inside this namespace, different utilities to be able to handle WebAR functionalities on top of the ThreeJS framework/engine.
* @namespace
*/
THREE.WebAR = {};

/**
* A class that allows to manage the point cloud acquisition and representation in ThreeJS. A buffer geometry is generated to represent the point cloud. The point cloud is provided using a VRDisplay instance that shows the capability to do so. The point cloud is actually exposed using a TypedArray. The array includes 3 values per point in the cloud. There are 2 ways of exposing this array:
* 1.- Using a new TypedArray for every frame/update. The advantage is that the TypedArray is always of the correct size depending on the number of points detected. The disadvantage is that there is a performance hit from the creation and copying of the array (and future garbage collection).
* 2.- Using the same reference to a single TypedArray. The advantage is that the performance is as good as it can get with no creation/destruction and copy penalties. The disadvantage is that the size of the array is the biggest possible point cloud provided by the underlying hardware. The non used values are filled with Infinity.
* @constructor
* @param {window.VRDisplay} vrDisplay - The reference to the VRDisplay instance that is capable of providing the point cloud.
* @param {boolean} usePointCloudVerticesDirectly - A flag to specify if a new TypedArray will be used in each frame with the exact number of points in the cloud or reuse a single reference to a TypedArray with the maximum number of points provided by the underlying hardware (non correct values are filled with Inifinity).
*
* NOTE: The buffer geometry that can be retrieved from instances of this class can be used along with THREE.PointGeometry to render the point cloud using points. This class represents the vertices colors with the color white.
*/
THREE.WebAR.VRPointCloud = function(vrDisplay, usePointCloudVerticesDirectly) {

	this._vrDisplay = vrDisplay;

	this._lastPointCloudVertexCount = 0;

	this._usePointCloudVerticesDirectly = usePointCloudVerticesDirectly;

	this._bufferGeometry = new THREE.BufferGeometry();
	this._bufferGeometry.frustumCulled = false;

	var positions = vrDisplay ? (usePointCloudVerticesDirectly ? vrDisplay.getPointCloud().vertices : new Float32Array( vrDisplay.getMaxPointCloudVertexCount() * 3 )) : new Float32Array([-1, 1, -2, 1, 1, -2, 1, -1, -2, -1, -1, -2 ]);
	var colors = new Float32Array( positions.length );

	var color = new THREE.Color();

	for ( var i = 0; i < colors.length; i += 3 ) {
		if (vrDisplay) {
			positions[ i ]     = Infinity;
			positions[ i + 1 ] = Infinity;
			positions[ i + 2 ] = Infinity;
		}
		color.setRGB( 1, 1, 1 );
		colors[ i ]     = color.r;
		colors[ i + 1 ] = color.g;
		colors[ i + 2 ] = color.b;
	}

	this._positions = new THREE.BufferAttribute( positions, 3 );
	this._bufferGeometry.addAttribute( 'position', this._positions );
	this._colors = new THREE.BufferAttribute( colors, 3 );
	this._bufferGeometry.addAttribute( 'color', this._colors );

	this._bufferGeometry.computeBoundingSphere();

	return this;
};

/**
* Returns the THREE.BufferGeometry instance that represents the points in the pont cloud.
* @return {THREE.BufferGeometry} - The buffer geometry that represents the points in the point cloud.
*/
THREE.WebAR.VRPointCloud.prototype.getBufferGeometry = function() {
	return this._bufferGeometry;
};

THREE.WebAR.VRPointCloud.prototype.update = function(camera) { // TODO: Camera should not be needed to be passed when the getPose problem is solved.
	if (!this._vrDisplay) return;
	var pointCloud = this._vrDisplay.getPointCloud();
	if (!this._usePointCloudVerticesDirectly) {
		if (pointCloud.vertices != null && pointCloud.vertexCount > 0) {
			var vertexCount = Math.min(pointCloud.vertexCount, this._positions.length);
			var pointCloudValueCount = vertexCount * 3;
			for (var i = 0; i < pointCloudValueCount; i++) {
				this._positions.array[i] = pointCloud.vertices[i];
			}
			var lastPointCloudValueCount = this._lastPointCloudVertexCount * 3;
			for (var i = pointCloudValueCount; i < lastPointCloudValueCount; i++) {
				this._positions.array[i] = Infinity;
			}
			this._lastPointCloudVertexCount = vertexCount;
			this._positions.needsUpdate = true;
		}
	}
	else {
		this._positions.needsUpdate = true;
	}
};

THREE.WebAR.createVRSeeThroughCameraMesh = function(vrDisplay) {
	var uvs;
	var video;
	if (vrDisplay) {
		var seeThroughCamera = vrDisplay.getSeeThroughCamera();
		video = seeThroughCamera;
		// HACK: Needed to tell the THEE.VideoTextue that the video is ready and that the texture needs update.
		video.readyState = 2;
		video.HAVE_CURRENT_DATA = 2;
        uvs = new Float32Array([
            0.0, 0.0,
            seeThroughCamera.width / seeThroughCamera.textureWidth, 0.0, 
            seeThroughCamera.width / seeThroughCamera.textureWidth, seeThroughCamera.height / seeThroughCamera.textureHeight,
            0.0, seeThroughCamera.height / seeThroughCamera.textureHeight
        ]);
	}
	else {
		var video = document.createElement("video");
		video.src = "sintel.webm";
		video.play();
		uvs = new Float32Array([
			0.0, 0.0, 
			1.0, 0.0,
			1.0, 1.0,
			0.0, 1.0
        ]);
	}

	var geometry = new THREE.BufferGeometry();
	geometry.addAttribute("position", new THREE.BufferAttribute( new Float32Array([
		-1.0,  1.0,  0.0,
         1.0,  1.0,  0.0,
         1.0, -1.0,  0.0,
        -1.0, -1.0,  0.0
	]), 3));
	geometry.setIndex(new THREE.BufferAttribute( new Uint16Array([0, 1, 2, 0, 2, 3]), 1));
	geometry.addAttribute("uv", new THREE.BufferAttribute( uvs, 2 ));
	geometry.computeBoundingSphere();

	var videoTexture = new THREE.VideoTexture(video);
	videoTexture.minFilter = THREE.LinearFilter;
	videoTexture.magFilter = THREE.LinearFilter;
	videoTexture.format = THREE.RGBFormat;			
	videoTexture.flipY = false;

	var material = new THREE.MeshBasicMaterial( {color: 0xFFFFFF, side: THREE.DoubleSide, map: videoTexture } );
	var mesh = new THREE.Mesh(geometry, material);
	return mesh;
};

THREE.WebAR.createVRSeeThroughCamera = function(vrDisplay, near, far) {
	var camera;
	if (vrDisplay) {
		var seeThroughCamera = vrDisplay.getSeeThroughCamera();
		var width = seeThroughCamera.width;
		var height = seeThroughCamera.height;
		var fx = seeThroughCamera.focalLengthX;
		var fy = seeThroughCamera.focalLengthY;
		var cx = seeThroughCamera.pointX;
		var cy = seeThroughCamera.pointY;

        var xscale = near / fx;
        var yscale = near / fy;

        var xoffset = (cx - (width / 2.0)) * xscale;
        // Color camera's coordinates has y pointing downwards so we negate this term.
        var yoffset = -(cy - (height / 2.0)) * yscale;

        camera = new THREE.Camera();
        camera.projectionMatrix.makeFrustum(xscale * -width / 2.0 - xoffset, xscale * width / 2.0 - xoffset,yscale * -height / 2.0 - yoffset, yscale * height / 2.0 - yoffset, near, far);
	}
	else {
		camera = new THREE.PerspectiveCamera( 60, window.innerWidth / window.innerHeight, near, far );
	}
	return camera;
};

THREE.WebAR._worldUp = new THREE.Vector3(0.0, 1.0, 0.0);
THREE.WebAR._normalY = new THREE.Vector3();
THREE.WebAR._normalZ = new THREE.Vector3();
THREE.WebAR._rotationMatrix = new THREE.Matrix4();

THREE.WebAR.positionAndRotateObject3DWithPickingPointAndPlaneInPointCloud = function(pointAndPlane, object3d, scale) {
	var planeNormal = new THREE.Vector3(pointAndPlane.plane[0], pointAndPlane.plane[1], pointAndPlane.plane[2]);
	THREE.WebAR._normalY.set(0.0, 1.0, 0.0);
	var threshold = 0.5;
	if (planeNormal.dot(THREE.WebAR._worldUp) > threshold) {
		THREE.WebAR._normalY.set(0.0, 0.0, 1.0);
	}
	THREE.WebAR._normalZ.crossVectors(planeNormal, THREE.WebAR._normalY).normalize();
	THREE.WebAR._normalY.crossVectors(THREE.WebAR._normalZ, planeNormal).normalize();
	THREE.WebAR._rotationMatrix.elements[ 0] = planeNormal.x;
	THREE.WebAR._rotationMatrix.elements[ 4] = planeNormal.y;
	THREE.WebAR._rotationMatrix.elements[ 8] = planeNormal.z;
	THREE.WebAR._rotationMatrix.elements[ 1] = THREE.WebAR._normalY.x;
	THREE.WebAR._rotationMatrix.elements[ 5] = THREE.WebAR._normalY.y;
	THREE.WebAR._rotationMatrix.elements[ 9] = THREE.WebAR._normalY.z;
	THREE.WebAR._rotationMatrix.elements[ 2] = THREE.WebAR._normalZ.x;
	THREE.WebAR._rotationMatrix.elements[ 6] = THREE.WebAR._normalZ.y;
	THREE.WebAR._rotationMatrix.elements[10] = THREE.WebAR._normalZ.z;
	object3d.quaternion.setFromRotationMatrix(THREE.WebAR._rotationMatrix);

	object3d.position.set(pointAndPlane.point[0], pointAndPlane.point[1], pointAndPlane.point[2]);
	object3d.position.add(planeNormal.multiplyScalar(scale));

  // glm::vec3 normal_Y = glm::vec3(0.0f, 1.0f, 0.0f);
  // const glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
  // const float kWorldUpThreshold = 0.5f;
  // if (glm::dot(plane_normal, world_up) > kWorldUpThreshold) {
  //   normal_Y = glm::vec3(0.0f, 0.0f, 1.0f);
  // }

  // const glm::vec3 normal_Z = glm::normalize(glm::cross(plane_normal, normal_Y));
  // normal_Y = glm::normalize(glm::cross(normal_Z, plane_normal));

  // glm::mat3 rotation_matrix;
  // rotation_matrix[0] = plane_normal;
  // rotation_matrix[1] = normal_Y;
  // rotation_matrix[2] = normal_Z;
  // const glm::quat rotation = glm::toQuat(rotation_matrix);

  // cube_->SetRotation(rotation);
  // cube_->SetPosition(glm::vec3(area_description_position) +
  //                    plane_normal * kCubeScale);

};

