# Overview

**NOTE:** The repository with the Chromium code to enable WebAR capabilities on top of the WebVR implementation is still not publicly available so much of the references to it in this documentation might not be applicable for those without access.

This project's goal is to provide an initial implementation of a possible Augmented Reality API for the Web. This documentation (specially this page you are reading) includes a tutorial on how to build your own version of Chromium that has WebAR capabilities. Also, on the side, you may find documentation of both the low level JS API exposed on top of some of the WebVR API already existing classes (and some new ones) and the utility library built on top of THREE.JS (called THREE.WebAR).

This WebAR specification and implementation is completely experimental so use it at your own risk. There is no guarantee that any of this code will ever make it to Chromium and even less to Chrome but it will provide the possibitliy to use the WebVR API on an actual Android application if needed. The whole implementation is built on the Tango device and SDK for the moment.

This documentation is divided in 2 sections: 
1. **How to build your own version of Chromium with WebAR** 
2. **Using the new VR/AR JavaScript APIs**

This way, anyone that wants to build and modify his/her own version of chromium, will be able to do so and those who just want to install a prebuilt version of Chromium and start using it right away, will also be able to do it.

## 1. How to build your own version of Chromium with WebAR

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
 3. `~/chromium$ fetch --nohooks android`
 	NOTE: This process may take quite some time (an hour?)
 4. Verify that the .gclient file has `target_os = ['android']` in it
 5. `~/chromium/src$ gclient sync`
 6. Checkout a specific tag to a new branch. The tag is `54.0.2796.3`. The name of the branch to checkout could be `webar_54.0.2796.3` for example.
 	`~/chromium/src$ git checkout -b webar_54.0.2796.3 54.0.2796.3`
 7. Create a folder for the final product compilation:
 	~/chromium/src$ mkdir -p out/webar_54.0.2796.3
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
 9. Copy and paste all the content from the current folder of the WebAR repo into the chromium/src folder. Override every possible conflict that may arise if you use the file explorer. Otherwise, you can use the following command line
 	`cp -r PATH_TO_THIS_FOLDER/* ~/chromium/src`
 10. `~/chromium/src$ gn args out/webar_54.0.2796.3`
	**NOTE**: just exit "q!" in vi when it opens and shows args.gn (modified in the previous step using a proper editor ;))
 11. `~/chromium$ src/build/install-build-deps-android.sh` 
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
This tutorial specified that the name of the out folder created during the setup process above is the same as the branch (54.0.2796.3). This is no coincidence, as the `build_install_run.sh` shell script provided along with this documentation allows to build the Chromium project depending on the current git branch name that is currently checked out. This script not only compiles Chromium but also the Tango native library called tango_chromium that handle the Tango SDK calls. Moreover, this script, it also installs the final APK on a connected device and runs it, so it is convenient that you to connect the Tango device via USB before executing it. The project that will be built by default is the Chromium WebView project.
```
~/chromium/src/build_install_run.sh
```

### Resolving some possible Chromium WebView crashes on some Android versions

Chromium webview v54.0.2796.3 seems to crash pretty consistently on some Android versions (5 and 6) on some internal checks/asserts related to both audio and touch handling. This is a list of some possible points in the chromium source code where these crashes may occur. It is recommended that some testing is performed before introducing these changes. The good thing with asserts/checks is that they show the specific line of code where the crash is happening so a simple review of the logcat when the crash happens should poing to the specific line of code and file where the assert is failing.

**NOTE**: It is recommended that if you introduce these changes, they are well documented (for example introducing a //WebAR BEGIN..//WebAR END block to correctly mark each change). Remember, these changes are not completely necessary and should only be included if your chromium product is consistently crashing when using audio and touch and showing check related errors on the logcat before crashing.

* `ui/events/android/motion_event_android.cc`

	Line 221:

		// DCHECK_LT(pointer_index, cached_pointer_count_);

	Line 231:

		// DCHECK_LT(pointer_index, cached_pointer_count_);

* `third_party/WebKit/Source/core/layout/LayoutBlockFlow.cpp`

	Line 3351:

		// ASSERT(!floatBox.hasSelfPaintingLayer());

* `third_party/WebKit/Source/modules/webaudio/DeferredTaskHandler.cpp`

	Line 313:

		// ASSERT(!isMainThread());

	Line 37:

		// ASSERT(!isAudioThread());

* `media/blink/multibuffer_data_source.cc`

 	Line 442:   

		// DCHECK(render_task_runner_->BelongsToCurrentThread());
		// DCHECK(reader_.get());

* `third_party/WebKit/Source/modules/webaudio/AudioListener.cpp`

	Line 207:

	    // DCHECK(!isMainThread());

* `third_party/WebKit/Source/modules/webaudio/BaseAudioContext.cpp`

	Line 221:

	    // DCHECK(!isAudioThread());

* `content/browser/renderer_host/render_widget_host_impl.cc`

	Line 1008:

	    // DCHECK(*is_in_gesture_scroll ||
	    //        (gesture_event.type == blink::WebInputEvent::GestureFlingStart &&
	    //         gesture_event.sourceDevice ==
	    //             blink::WebGestureDevice::WebGestureDeviceTouchpad));

* `content/common/input/input_event_stream_validator.cc`

	Line 31:

		// DCHECK(ValidateImpl(event, &error_msg_))
		//     << error_msg_
		//     << "\nInvalid Event: " << WebInputEventTraits::ToString(event);
	
* `third_party/WebKit/Source/core/input/EventHandler.cpp`

	Line 1406/1968/1993:

		// ASSERT(result.isRectBasedTest());

* `android_webview/browser/browser_view_renderer.cc`

	Line 574:

		// DCHECK_LE(0.f, scroll_offset_dip.x());
		// DCHECK_LE(0.f, scroll_offset_dip.y());
		// DCHECK(scroll_offset_dip.x() < max_scroll_offset_dip_.x() ||
		//        scroll_offset_dip.x() - max_scroll_offset_dip_.x() < kEpsilon)
		//     << scroll_offset_dip.x() << " " << max_scroll_offset_dip_.x();
		// DCHECK(scroll_offset_dip.y() < max_scroll_offset_dip_.y() ||
		//        scroll_offset_dip.y() - max_scroll_offset_dip_.y() < kEpsilon)
		//     << scroll_offset_dip.y() << " " << max_scroll_offset_dip_.y();

	Line 40:

		// const double kEpsilon = 1e-8;

* `ui/events/gesture_detection/gesture_detector.cc`

	Line 585:

		// Substituted these lines for the return of the current down event.
		// NOTREACHED();
		// return nullptr;
		return &current_down_event;

* `ui/events/gesture_detection/gesture_provider.cc`

	Line 360:

	    // DCHECK(scroll_event_sent_);

	Line 695:

		// DCHECK_GE(source_index, 0);

* `thrid_party/WebKit/Source/core/layout/HitTestResult.cpp`

	Line 468:

		// ASSERT(isRectBasedTest());

* `content/browser/renderer_host/input/input_router_impl.cc`

	Line 503:

		// Commented out the CHECK and added the conditional if.
		// DCHECK_GT(active_renderer_fling_count_, 0);
		// Note that we're only guaranteed to get a fling end notification from the
		// renderer, not from any other consumers. Consequently, the GestureEventQueue
		// cannot use this bookkeeping for logic like tap suppression.
		if (active_renderer_fling_count_ > 0)
			--active_renderer_fling_count_;

* `content/browser/renderer_host/input/touch_event_queue.cc`

	Line 33:

		// const double kMaxConceivablePlatformSlopRegionLengthDipsSquared = 60. * 60.;

	Line 315:

	    // DCHECK_LT((gfx::PointF(event.touches[0].position) -
	    //            touch_start_location_).LengthSquared(),
	    //           kMaxConceivablePlatformSlopRegionLengthDipsSquared);

## Using the new VR/AR JavaScript APIs

### Build the documentation

In order to build the documentation you are currently reading, there are some steps that need to be followed:

1. Install JSDoc: `npm install -g jsdoc`
2. `$ jsdoc WebARAPI.js THREE.WebAR/THREE.WebAR.js README.md`

### A basic overview of the WebAR JS API

This implementation of WebAR is an addition of some features on top of the WebVR API v1.0 specification. AR and VR share many common concepts like tracking and even a see through camera or a depth sensor can be found in both AR and VR devices. This API is still experimental and it is just a proposal os a possible solution.

The main point of entry for the WebAR API is still the VRDisplay, similarly as in  WebVR. Actually, if an AR device such as Tango (which this implementation is based on) wants to be used for 6DOF (6 Degrees Of Freedom) VR experiences, the WebVR API as is could be used. The getPose call will correctly return the position and orientation acquired from the underlying hardware implementation. 

But there are some new features that the WebVR v1.0 spec does not include and that provide additional functionality based on the AR underlying platform. These new characteristics can be identified using the VRDisplayCapabilities class that now exposes 2 new flags to specify if the VRDisplay is able to:

1. `hasPointCloud`: Retrieve a cloud of points acquired by a depth sensing device.
2. `hasSeeThroughCamera`: Use an undelying see through camera to show the real world.



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

