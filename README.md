# Overview

This project's goal is to provide an initial implementation of a possible Augmented Reality (AR) API for the Web and start a conversation on the subject.

# Index

* Disclaimer
* Using the WebAR prototype and the new WebAR APIs
  * Install the APK
  * Overview of the WebAR APIs
  * Using the WebAR APIs in ThreeJS
  * Examples
* How to build your own version of Chromium with WebAR APIs
* Supported devices
* How to build this documentation
* License
* 

# Disclaimer

Defining how a web standard will look like is a complex conversation. All the code and proposals in this project are not meant to be the definitive implementation of AR capabilities for the web, but some prototypes you can play around with at your own risk. I will try to keep this project alive and implement as many AR devices/platforms as possible.

# Hands on WebAR

This project is built on top of Chromium, an open source web browser. You have 2 options to start working with the WebAR prototype:

1. If you would like to use a precompiled version of the prototype and start playing asap, you may find the section <a href="#using_the_new_vr_ar_javascript_apis"><b>Using the new VR/AR JavaScript APIs</b></a> very useful.

2. If you would like to compile your own version of Chromium with WebAR capabilities and contribute to the project, this documentation includes a tutorial on<a href="#how_to_build_your_own_version_of_chromium_with_webar"><b>How to build your own version of Chromium with WebAR</b></a>.

## <a name="using_the_new_vr_ar_javascript_apis"></a> Using the new VR/AR JavaScript APIs

### Build the documentation

In order to build the documentation you are currently reading, there are some steps that need to be followed:

1. Install JSDoc: `npm install -g jsdoc`
2. `$ jsdoc WebARAPI.js THREE.WebAR/THREE.WebAR.js README.md`

### A basic overview of the WebAR JS API

This implementation of WebAR is an addition of some features on top of the [WebVR API v1.0 specification](https://webvr.info/). AR and VR share many common concepts like tracking and even a see through camera or a depth sensor can be found in both AR and VR devices. This API is still experimental and it is just a proposal os a possible solution.

The main point of entry for the WebAR API is still the VRDisplay, similarly as in  WebVR. Actually, if an AR device such as Tango (which this implementation is based on) wants to be used for 6DOF (6 Degrees Of Freedom) VR experiences, the WebVR API as is could be used. The getPose call will correctly return the position and orientation acquired from the underlying hardware implementation. 

But there are some new features that the WebVR v1.0 spec does not include and that provide additional functionality based on the AR underlying platform. These new characteristics can be identified using the VRDisplayCapabilities class that now exposes 2 new flags to specify if the VRDisplay is able to:

* [hasPointCloud](./VRDisplayCapabilities.html): Retrieve a cloud of points acquired by a depth sensing device.
* [hasSeeThroughCamera](./VRDisplayCapabilities.html): Use an undelying see through camera to show the real world.

If any of these flags are true, a new set of functionalities and APIs can be used always using the [VRDisplay](./VRDisplay.html) as a starting point to retrieve them. The new methods are:

* [getMaxPointCloudVertexCount](./VRDisplay.html): Provides the maximum number of points in the point cloud.
* [getPointCloud](./VRDisplay.html): Updates and/or retrieves the points in the [point cloud](./VRPointCloud.html).
* [getPickingPointAndPlaneInPointCloud](./VRDisplay.html): Allows to calculate a colission [point and plane](./VRPickingPointAndPlane.html) between a 2D position and a ray casted on to the point cloud.
* [getSeeThroughCamera](./VRDisplay.html): Retrieves a structure that represents the [see through camera](VRSeeThroughCamera.html) so it can be used for both correct fustrum calculation and for rendering the video feed.

At a glance it is obvious that some new data structures/classes have been created to support some new functionalities as the underlying Tango platform allows new types of interactions/features. Most of the calls are pretty straightforward and the documentation might provide some idea of how they could be integrated in any or nex web application. The one that might need a bit more explanation is the VRSeeThroughCamera class as it even provides some useful information (what are called the camera intrinsics), it still does not expose how it could be used to render the camera feed in an application. In the current implementation, the approach that has been selected is to create a new overloaded function in the [WebGL API](https://www.khronos.org/registry/webgl/specs/1.0). The [WebGLRenderingContext](https://www.khronos.org/registry/webgl/specs/1.0/#5.14) now exposes the following function:

```
void texImage2D(GLenum target, GLint level, GLenum internalformat, GLenum format, GLenum type, VRSeeThroughCamera? source);
```

This approach has some benefits:

1. There is no need to retrieve the pixels of the image.
2. There is full control over the camera image in WebGL (in a fragment shader for example).
3. It uses a common way to handle video content (texImage2D already has a HTMLVideoElement overload).

But the current implementation has a problem too as the way the camera image is handled inside the texImage2D call requires to use an OpenGL extension that is not available in WebGL at the moment. The Chromium modification that you can find in the repository includes the activation of this extension internally, but you also need to recall that you will need to use the extension in your shader:

```
#extension GL_OES_EGL_image_external : require
...

uniform samplerExternalOES map;
...
```

## Some notes about developing WebAR apps using ThreeJS

**IMPORTANT**: In order to use the external image OES extension, a modification to the ThreeJS engine is required. In the `getSingularSetter` function that is able to identify the set functions for the different types of uniforms/attributes in a shader, a new type needs to be added as follows:
```
function getSingularSetter( type ) {

	switch ( type ) {

		case 0x1406: return setValue1f; // FLOAT
		case 0x8b50: return setValue2fv; // _VEC2
		case 0x8b51: return setValue3fv; // _VEC3
		case 0x8b52: return setValue4fv; // _VEC4

		case 0x8b5a: return setValue2fm; // _MAT2
		case 0x8b5b: return setValue3fm; // _MAT3
		case 0x8b5c: return setValue4fm; // _MAT4

		case 0x8b5e: case 36198: return setValueT1; // SAMPLER_2D  // case 36198: Added by WebAR
		case 0x8b60: return setValueT6; // SAMPLER_CUBE

		case 0x1404: case 0x8b56: return setValue1i; // INT, BOOL
		case 0x8b53: case 0x8b57: return setValue2iv; // _VEC2
		case 0x8b54: case 0x8b58: return setValue3iv; // _VEC3
		case 0x8b55: case 0x8b59: return setValue4iv; // _VEC4

	}

}
```

## <a name="how_to_build_your_own_version_of_chromium_with_webar"></a> 1. How to build your own version of Chromium with WebAR

This repository includes only the modifications on the Chromium repository that allows to add Tango/WebAR capabilities into JavaScript. Chromium is a complex project with gigabytes of source code, resources and third party libraries. This repository does not include all Chromium but just the files necessary to make the changes to it in order to enable WebAR, so you will also have to checout the Chromium repository (how to do so will be explained in this tutorial). 

### The folder structure in this repo

Let's review the content on this repository to better understand it:

`/android_webview`: The modifications to the android webview Chromium project to be able to load the Tango handling dynamic library, store the URLs for future executions, read QRCodes, etc. This folder, internally, contains the Tango handling library and all the build files and resources needed. 

`/device`: The modifications to the vr device files to add both the tango_vr_device class and the modifications to the vr service to be able to expose some specific data related to AR (point cloud, see through camera data, picking, etc.).

`/examples`: A set of some basic examples based on straight WebGL or (mostly) using the ThreeJS framework.

`/gpu`: All the modifications in the GL command buffer to be able to use the external texture extension and make the right call to Tango implementation when the texImage2D is called using the VRSeeThroughCamera instance.

`/third_party`: Both the inclusion of the tango third party libraries (and the tango handling library) and the WebKit vr and webgl IDL classes modifications to be able to expose all the functionalities. ZXing is also added to be able to add the QRCode reading functionality to the WebView APK.

Building the modified version of Chromium is a 2 step process: 

1. Clone the Chromium project (copying the changes in this repository) and prepare it to be built
1. Build, install and run.

### 1.1 Clone the Chromium project (copying the changes in this repository) and prepare it to be built

Chromium cloning/building instruction are available online. 

[https://www.chromium.org/developers/how-tos/android-build-instructions](https://www.chromium.org/developers/how-tos/android-build-instructions)

Anyway, in order to help with the process, we recommend you follow the following steps. Remember, Tango is only available on the Android platform for the moment so in order to be able to use the modifications present in this project, you need to compile Chromium for Android that can only be done on Linux. Unfortunately, this document does not include instructions on how to setup a linux machine. Let's assume that the machine is installed along with:

* Java JDK 1.8 and JRE 1.8
* Android SDK
* Android NDK 12b
* GIT
* Setup the PATH variable to point to the tools.

Open a terminal window to be able 
 1. Install depot_tools: [Tutorial](https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up)
	1. `git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git`
	2. `export PATH=$PATH:/path/to/depot_tools`
 2. `$ mkdir ~/chromium && cd ~/chromium`
 3. `~/chromium$ fetch --nohooks android`.          **NOTE**: This process may take quite some time (an hour?)
 4. Verify that the `.gclient` file has `target_os = ['android']` in it
 5. `cd src` and then `~/chromium/src$ gclient sync`.          **NOTE**: This process may take some time too.
 6. Checkout a specific tag to a new branch. The tag used for this build is `54.0.2796.3`. The name of the branch to checkout could be `webar_54.0.2796.3` for example: `~/chromium/src$ git checkout -b webar_54.0.2796.3 54.0.2796.3`. Choose the name of the brnach you like but remember it to create a corresponding out folder.
 7. Create a folder for the final product compilation with the same name as the branch: `~/chromium/src$ mkdir -p out/webar_54.0.2796.3`
 8. Create and edit a new file `out/webar_54.0.2796.3/args.gn` with the command `~/chromium/src$ gedit out/webar_54.0.2796.3/args.gn` and copy and paste the following content in it:
	```
	target_os = "android"
	target_cpu = "arm"  # (default)
	is_debug = false  # (default)

	# Other args you may want to set:
	is_component_build = true
	is_clang = true
	symbol_level = 1  # Faster build with fewer symbols. -g1 rather than -g2
	enable_incremental_javac = true  # Much faster; experimental
	```
 9. Copy and paste all the content from the current folder of the WebAR repo into the chromium/src folder. Override every possible conflict that may arise if you use the file explorer. Otherwise, you can use the following command line: `cp -r PATH_TO_THIS_FOLDER/* ~/chromium/src`
 10. `~/chromium/src$ gn args out/webar_54.0.2796.3`.          **NOTE**: just exit "q!" in vi when it opens and shows args.gn (modified in the previous step using a proper editor ;))
 11. `~/chromium/src$ build/install-build-deps-android.sh` 
 12. Execute the following commands and select the right choice on each. Do not worry if some commands do not have any effect.
 	```
	sudo update-alternatives --config javac
	sudo update-alternatives --config java
	sudo update-alternatives --config javaws
	sudo update-alternatives --config javap
	sudo update-alternatives --config jar
	sudo update-alternatives --config jarsigner
	```
 13. `~/chromium/src$ gclient sync`
 14. `~/chromium/src$ . build/android/envsetup.sh`

### 1.2: Build, install and run

**IMPORTANT:** some changes have been done to the Chromium command buffer. These changes may require to rebuild the command buffer. The Python script to do so does not execute along with the regular building process so the script needs to be executed with the following command at least once (and everytime a new command is created in the command buffer):
```
~/chromium/src/python gpu/command_buffer/build_gles2_cmd_buffer.py
```
This tutorial specified that the name of the out folder created during the setup process above is the same as the branch (webar_54.0.2796.3). This is no coincidence, as the `build_install_run.sh` shell script provided along with this documentation allows to build the Chromium project depending on the current checked out git branch. This script not only compiles Chromium but also the Tango native library called `tango_chromium` that handle the Tango SDK calls. Moreover, this script also installs the final APK on to a connected device and runs it, so it is convenient that you to connect the Tango device via USB before executing it. The project that will be built by default is the Chromium WebView project, the only one that has been modified to provide Tango/WebAR capabilities.
```
~/chromium/src/build_install_run.sh
```

