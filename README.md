# WebARonTango

**An experimental browser for Android that lets developers create Augmented Reality (AR) experiences using web technologies. An [ARCore Android version](https://github.com/google-ar/WebARonTango) and an [ARKit iOS version](https://github.com/google-ar/WebARonARKit) are also available.**

<img alt="PointCloud" src="markdown/images/WebARExamplePicking.gif" style="float: left; object-fit: cover; width: 45%; height: 20em; margin-right: 1em; "><img alt="Occlusion" src="markdown/images/WebARExampleOcclusion.gif" style="width: 45%; height: 20em; object-fit: cover;">

**Note:** This is not an official Google product. Nor is it a fully-featured web browser. Nor are the enabling JavaScript APIs standards, or on the standardization path. WebARonARTango is only meant to enable developer experimentation.

## <a name="DifferencesBetweenWebARonTangoAndOthers">Differences between WebARonTango and WebARonARCore/WebARonARKit</a>

All of the prototypes are build on top of the WebVR 1.1 spec and for the basic AR capabilities (motion tracking, camera feed rendering and hit testing) they all share a common workflow. However, WebARonTango exposes some additional functionalities and lacks others compared to the ARCore and ARKit based projects.

### What does WebARonTango have that WebARonARCore/WebARonARKit does not?

* Depth point cloud support: WebARonTango provides access to the depth point cloud that the infrared camera in the Tango spec includes.

* **ADF support**: For now, WebARonTango exposes a very simple access to the Area Description capabilities of Tango. Maybe in the future, the other projects may expose this functionality too.

* **Marker support**: For now, WebARonTango exposes QRCode and AR marker support. Maybe in the future, the other projects may expose this functionality too.

### What does WebARonARCore/WebARonARKit have that WebARonTango does not?

* **Plane support**: ARCore and ARKit provide marker detection capabilities. Tango does not for now as it already includes a depth point cloud support to be able to identify real world poses. 

## Getting started

### <a name="GetATangoDevice">1. Get a Tango enabled device</a>

In order to test this prototype browser, you will need a Tango enabled device. Tango is a hardware and software specification from Google to enable AR on mobile devices. Tango enabled devices include a wide field of view (FOV) camera and an infrared camera on top of the regular RGB camera to provide advanced AR and real world understanding capabilities. Check out the [Supported Devices](#SupportedDevices) section for further information.

### <a name="InstallTheWebARonTangoAPK">2. Install the WebARonTango APK</a>

Directly from a Tango enabled device: 

* Visit [this link](https://github.com/google-ar/WebARonTango/raw/master/apk/WebARonTango.apk) from a web browser on your Android device to download and install the WebARonTango APK.

...or by using ADB:

* Download the [WebARonTango APK](https://github.com/google-ar/WebARonTango/raw/master/apk/WebARonTango.apk). 
* Install the APK to your device:
  * `$ adb install -r /path/to/WebARonTango.apk`

Alternatively, the WebARonTango APK can be [built and installed from source](#BuildingFromSource).

### <a name="ViewingExamples">3. Viewing examples</a>
A [list of example scenes](https://developers.google.com/ar/develop/web/getting-started#examples) compatible with WebARonTango and [WebARonARKit](https://github.com/google-ar/WebARonARKit) are available at [developers.google.com](https://developers.google.com/ar/develop/web/getting-started#examples).

### <a name="BuildingScenes">4. Building your own scenes</a>
To build AR web experiences that work with WebARonTango (or [WebARonARCore for Android](https://github.com/google-ar/WebARonARCore) or [WebARonARKit for iOS](https://github.com/google-ar/WebARonARKit)), we recommend **[three.ar.js](https://github.com/google-ar/three.ar.js)**, a helper library that works with the popular [three.js](http://threejs.org) WebGL framework. [Three.ar.js](https://github.com/google-ar/three.ar.js) provides common AR building blocks, such as a visible reticle that draws on top of real world surfaces, and [example scenes](https://github.com/google-ar/three.ar.js#examples).

There are also some Tango [specific examples](https://github.com/google-ar/three.ar.js/tree/master/examples/tango) in the three.ar.js project repo that use the [specific features](#DifferencesBetweenWebARonTangoAndOthers) exposed in this prototype.

### <a name="debugging">5. Debugging</a>

WebARonTango uses WebViews, which is a similar debugging process to debugging Chrome for Android tabs. Check out the prereqs for your device at [Get Started with Remote Debugging Android Devices](https://developers.google.com/web/tools/chrome-devtools/remote-debugging/), and learn more about [Remote Debugging WebViews](https://developers.google.com/web/tools/chrome-devtools/remote-debugging/webviews#open_a_webview_in_devtools) by opening `chrome://inspect` in the desktop browser while your device is connected via USB.

## <a name="BuildingFromSource">Building the WebARonTango APK from source</a>

WebARonTango can optionally be built and installed from source. Instructions for [cloning and building Chromium](https://www.chromium.org/developers/how-tos/android-build-instructions) are available at [chromium.org](https://www.chromium.org/developers/how-tos/android-build-instructions)

Prerequisites:

* Linux machine
* GIT
* Python

We recommend the following steps:

1. Open a terminal window
2. Clone this repo in a folder like "WebARonTango" and remember its location as we will use it in step 8 below: `$ git clone https://github.com/google-ar/WebARonTango.git`
3. Install depot_tools. You can follow this [tutorial](https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up) or simply follow these 2 steps:
  * `git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git`
  * `export PATH=$PATH:/path/to/depot_tools`
4. Create a folder to contain `chromium` and move to it: `$ mkdir ~/chromium && cd ~/chromium`
5. Checkout the Chromium repo: `~/chromium$ fetch --nohooks android`. **Note**: This process may take a long time (an hour?)
6. Enter the `src` folder: `$ cd src`.
7. Create a new branch from a specific tag: `$ git checkout -b webarontango_57.0.2987.5 57.0.2987.5`
8. Copy the content from the `src` folder in the cloned WebARonTango in step 1: `$ cp -r PATH/TO/WEBARONTANGO/chromium/src/* .`
9. Synchronize the dependencies with this command: `~/chromium/src$ gclient sync --disable-syntax-validation`. **Note**: This process may take some time too.
10. Create a folder where to make the final product compilation: `~/chromium/src$ mkdir -p out/build`.
11. Create and edit a new file `out/build/args.gn`. Copy and paste the following content in the `args.gn` file:
```
  target_os = "android"
  target_cpu = "arm64"
  is_debug = false
  is_component_build = true
  enable_webvr = true
  proprietary_codecs = false
  ffmpeg_branding = "Chromium"
  enable_nacl = false
  remove_webcore_debug_symbols = true
```
12. Prepare to build: `~/chromium/src$ gn args out/build`. **Note**: once the command is executed, the vi editor will show you the content of the `args.gn` file just edited a few steps before. Just exit by pressing ESC and typing colon and `x`.
13. Install the build dependencies: `~/chromium/src$ build/install-build-deps-android.sh`
14. Synchronize the resources once again: `~/chromium/src$ gclient sync --disable-syntax-validation`
15. Setup the environment: `~/chromium/src$ . build/android/envsetup.sh`

##### Build, install and run

The line below not only compiles Chromium but also installs the final APK on to a connected device and runs it, so it is convenient that you to connect the device via USB before executing it. The project that will be built by default is the Chromium WebView project, the only one that has been modified to provide AR capabilities.
```
~/chromium/src$ ./build_install_run.sh
```
You can review the content of the script to see what it does (it is a fairly simple script) but if you would like to compile the final APK on your own you could do it by executing the following command:
```
~/chromium/src$ ninja -C out/build
```

The final APK will be built in the folders `~/chromium/src/apk` and `~/chromium/src/out/build/apks`.

## <a name="HowWebARonTangoWorks">How WebARonTango works</a>

WebARonTango is built of two essential technologies: Tango and Chromium. We also extend the WebVR 1.1 API, which gives us much of what we need for augmented reality, with a few more essentials, such as motion tracking, rendering of the camera's video feed, and basic understanding of the real world. For details, see [WebVR API extension for smartphone AR](https://github.com/google-ar/three.ar.js/blob/master/webvr_ar_extension.md)

## <a name="KnownIssues">Known issues</a>
* The current implementation of WebAR is built on top of the Chromium WebView flavor. This has some implementation advantages but some performance and use disadvantages. We are working on making the implementation on a full version of Chromium.
* Pausing/resuming/switching away from the app causes screen to turn black. This is a consequence of having built the implementation on top of the WebView flavor of Chromium. A proper implementation on full Chromium or a rebase to a more recent Chromium WebView version (>57.0.2987.5) might solve this problem.
* The [Web Speech](https://dvcs.w3.org/hg/speech-api/raw-file/tip/speechapi.html) API is a standard web API for text to speech and speech to text conversion that is available in Chromium. As WebARonTango is built on top of the WebView version of Chromium, does not provide this functionality by default. There is a solution though, using a polyfill we provide, but in order to use it, you need to either a) include the [three.ar.js](https://github.com/google-ar/three.ar.js) library before making any use of the Web Speech API or b) include the [ARSpeechRecognition.js](https://github.com/google-ar/three.ar.js/blob/master/src/ARSpeechRecognition.js) file also before making any reference to the Web Speech API. Only speech recognition is supported, not speech synthesis for now.

## <a name="FutureWork">Future work</a>
* Add more AR-related features.
* Adapt the implementation to the WebVR 2.0 spec proposal.
* Implement the prototype on full Chromium (not on the WebView flavor) and to a newer tag version (>57.0.2987.5).
* Improve the VRPassThroughCamera rendering pipeline either making it obscure for the developer or by using regular WebGL textures and shader samplers without having to use the external image texture extension.

## <a name="License">License</a>
Apache License Version 2.0 (see the `LICENSE` file inside this repo).

# <a name="SupportedDevices">Supported devices</a>

The current prototype that provides AR on the web experimental functionalities has been built on top of Tango and has been tested on the following devices:

* Tango Development Kit Tablet
  * Android version: 4.4.2
  * Tango Core version: 1.53:2017.04.28-hopak-release-0-gb1545f34:190015395:stable
  * Tango SDK version: Hopak

* Lenovo Phab 2 Pro
  * Android version: 6.0.1
  * Tango Core version: 1.53:2017.04.28-hopak-release-0-gb1545f34:230015395:stable
  * Tango SDK version: Hopak

* Asus Zenfone AR
  * Android version: 7.0
  * Tango Core version: 1.55:2017.06.23-release-m20-release-0-gb571120a1:240016802:stable
  * Tango SDK version: Hopak







<!--
# Index

* [Overview](#overview)
* [Disclaimer](#disclaimer)
* [How to use this repo](#how_to_use_this_repo)
* [Using the Chromium WebAR prototype and the new APIs](#using_the_chromium_webar_prototype_and_the_new_apis)
  * [How to install and use the Chromium WebAR prototype](#how_to_install_and_use_the_chromium_webar_prototype_on_android)
  * [Examples](#examples)
  * [Overview of the WebAR APIs](#overview_of_the_webar_apis)
  * [Using the WebAR APIs in ThreeJS](#using_the_webar_apis_in_threejs)
* [How to build your own version of Chromium with WebAR](#how_to_build_your_own_version_of_chromium_with_webar)
* [Supported devices](#supported_devices)
* [License](#license)
* [Known issues](#known_issues)
* [Future work](#future_work)

# <a name="overview">Overview</a>

This project's goal is to provide an initial implementation of a possible Augmented Reality (AR) API for the Web on top of Chromium. The initial prorotype is built on top of the [Tango](https://get.google.com/tango/) platform for Android by Google. Other projects provide support for other platforms like ARCore () for Android and ARKit () for iOS with a similar set of features. There is a precompiled and working prototype you can use right away or a tutorial on how to build your own version of modified Chromium prototype with AR capabilities on top of Tango.

A major objective of this project is to get a conversation going on the subject of how to provide Augmented Reality capabilities to the web: WebAR.

# <a name="disclaimer">Disclaimer</a>

**This is not an official Google product.**

Defining how a web standard will look like is a complex conversation. All the code and proposals in this project are not meant to be the definitive implementatios of AR capabilities for the web, but some prototypes you can play around with at your own risk and have some starting point to build upon.

# <a name="how_to_use_this_repo">How to use this repo</a>

This repository can be used in 2 ways:

1. Installing the Chromium prototype, learning about the new WebAR APIs and trying the examples: <a href="#using_the_chromium_webar_prototype_and_the_new_apis"><b>Using the WebAR prototype and the new APIs</b></a>.

2. Compiling you own version of Chromium with WebAR capabilities and contributing to the project: <a href="#how_to_build_your_own_version_of_chromium_with_webar"><b>How to build your own version of Chromium with WebAR</b></a>.

# <a name="using_the_chromium_webar_prototype_and_the_new_apis">Using the Chromium WebAR prototype and the new APIs</a>

## <a name="how_to_install_and_use_the_prototype">How to install and use the Chromium WebAR prototype</a>

The `bin` folder in this repo holds the precompiled versions of Chromium that support the WebAR API. Check the [Supported devices](#supported_devices) section to learn what devices/platforms are currently supported and have been tested. If you install and execute the build on a non-tango device, a warning message will be shown and you won't be able to use the application.

### <a name="how_to_install_and_use_the_chromium_webar_prototype_on_android">How to install and use the Chromium WebAR prototype on Android</a>

To install the APK you can use the Android SDK `adb` command from the command line. Assuming that you are in the `bin` folder:

```
bin$ adb install -r ChromiumAR.apk
```

The `-r` parameter will reinstall the APK in case you already had it. There are other ways to install the APK like downloading it directly to your device via email for example and allowing Android to install it for you.

The installed application will display the `ChromiumAR` name with the Android icon.

<img src="markdown/images/WebARChromiumIcon.png"/>

When executed, it will show an overly simplified version of a browser with just a QRCode button (explained later), the URL bar and the back and forward buttons as this version of the prototype is using the WebView "flavor" of Chromium.

<img src="markdown/images/WebARChromiumHead.png"/>

The QRCode button

<img src="markdown/images/QRCodeButton.png"/>

allows to introduce URLs encoded in QRCodes. I personally do not like introducing long/complex URLs using the on screen touch keyboard on Android, so QRCodes can come handy (not mandatory to be used). In order to use this functionality the installation of the [Barcode Scanner App](https://play.google.com/store/apps/details?id=com.google.zxing.client.android) from GooglePlay will be required if it is not already installed on the device. Do not worry, the app itself will prompt you to install it and redirect you to the store automagically the first time you press the QRCode button if the Barcode Scanner app is not yet present.

There are multiple QRCode generators around the web but I highly recommend to use [The QRCode Generator](https://www.the-qrcode-generator.com/).

The last introduced and loaded URL will be stored for future executions of the app. 

## <a name="examples">Examples</a>

This repository includes several examples to show the basics of how to use the WebAR Chromium prototype and the WebAr API on top of the WebVR API. The examples have been divided into raw WebGL and ThreeJS based trying to fill out the needs of developers that want to go deeper level and use WebVR+WebGL directly or the ones that would like to use a higher level engine like ThreeJS.

If you have a Tango ready device and have installed the prototype APK, you can use the QRCodes provided below to directly point to the available examples.

* **Raw WebGL based examples:** Available inside the `examples/webgl` folder.
  * `video`: It shows the basic functionality of rendering the video feed that is associated to the `VRSeeThroughCamera`. 
    * URL: [http://judax.github.io/webar/examples/webgl/video/](http://judax.github.io/webar/examples/webgl/video/)
    * QRCode:
      * <img src="markdown/images/qrcode_example_webgl_video.png"/>

* **ThreeJS based examples:** Available inside the `examples/threejs` folder. These examples use the support `THREE.WebAR.js` library to simplify some boilerplate code while using the WebVR/WebAR API.
  * `video`: It shows the basic functionality of rendering the video feed that is associated to the `VRSeeThroughCamera`.
    * URL: [http://judax.github.io/webar/examples/threejs/video/](http://judax.github.io/webar/examples/threejs/video/)
    * QRCode: 
      * <img src="markdown/images/qrcode_example_threejs_video.png"/>
  * `picking`: It shows how an 3D model can be placed in the real world using the point cloud functionality. It also allows to show the `VRPointCloud`, do 6DOF motion tracking and enable/disable the `VRSeeThroughCamera`. It even allows to perform continuous picking to place a 3D model in the real space in real time.
    * <img src="markdown/images/WebARExamplePicking.gif"/>
    * URL: <a href="http://judax.github.io/webar/examples/threejs/picking/" target="_blank">http://judax.github.io/webar/examples/threejs/picking/</a>
    * QRCode: 
      * <img src="markdown/images/qrcode_example_threejs_picking.png"/>
  * `occlusion`: similar to the `picking` example but in this case uses the point cloud to enable depth based occlusion so real objects can occlude virtual objects.
    * <img src="markdown/images/WebARExampleOcclusion.gif"/>
    * URL: [http://judax.github.io/webar/examples/threejs/occlusion/](http://judax.github.io/webar/examples/threejs/occlusion/)
    * QRCode: 
      * <img src="markdown/images/qrcode_example_threejs_occlusion.png"/>
  * `marker`: an example on how to detect markers.
    * <img src="markdown/images/WebARExampleMarker.gif"/>
    * URL: [http://judax.github.io/webar/examples/threejs/marker/](http://judax.github.io/webar/examples/threejs/marker/)
    * QRCode: 
      * <img src="markdown/images/qrcode_example_threejs_marker.png"/>
  * **WayFair Google I/O 2017 prototype:** An online shopping experience prototype. This project is currently not open source but you can test it:
    * <img src="markdown/images/WebARWayfairPrototype_long.gif"/>
    * URL: [https://webar-wayfair-prototype.appspot.com/](https://webar-wayfair-prototype.appspot.com/)
    * QRCode: 
    * <img src="markdown/images/qrcode_example_wayfair.png"/>

* **ADF example:** Available inside the `examples/adf` folder it shows a very basic use of the ADF API. It lists the existing ADFs in the device and enables the first one in the list. To be able to create ADFs, please, check the hello world area description APK in the bin folder.
    * URL: [http://judax.github.io/webar/examples/adf/](http://judax.github.io/webar/examples/adf/)
    * QRCode:
      * <img src="markdown/images/qrcode_example_adf.png"/>

### <a name="overview_of_the_webar_apis">Overview of the WebAR APIs</a>

This implementation of WebAR is an addition of some features on top of the [WebVR API specification](https://webvr.info/) version 1.1. AR and VR share many common concepts like motion tracking or even a see through camera or a depth sensor, as they can be found in both AR and VR devices (Google Tango, Microsoft Hololens, HTC Vive, ...). If you are not familiar with the [WebVR API](https://webvr.info/), I highly recommend that you review it before continuing as some basic knowledge of it will be assumed in the following paragraphs.

All the documentation specific to the new APIs inside WebVR can be found online at: [http://judax.github.io/webar/doc/webarapi](http://judax.github.io/webar/doc/webarapi) that is generated from the file WebARAPI.js found in this repository.

The main point of entry for the WebAR API is still the `VRDisplay` instance as in WebVR. Actually, if an AR device such as Tango wants to be used for 6DOF (6 Degrees Of Freedom) VR experiences (non AR), the WebVR API as is could be used. The `getPose` and `getFrameData` calls will correctly return the position and orientation acquired from the underlying hardware implementation. 

This prototype exposes a new `VRDisplay` instance with the name of `"Tango VR Device"` for now when the `navigator.getVRDisplays` call is made.

But there are some new features that the WebVR spec does not include and that provide additional functionality based on the underlying AR platform. These new characteristics can be identified using the `VRDisplayCapabilities` instance obtained from the `VRDisplay` instance that now exposes 2 new flags to specify if the device:

* [hasPointCloud](http://judax.github.io/webar/doc/webarapi/VRDisplayCapabilities.html): The `VRDisplay` instance is able to provide a point cloud acquired by a depth sensing device in the underlying platform.
* [hasSeeThroughCamera](http://judax.github.io/webar/doc/webarapi/VRDisplayCapabilities.html): The `VRDisplay` instance is able to use an underlying see through camera to show the real world.
* [hasMarkerSupport](http://judax.github.io/webar/doc/webarapi/VRDisplayCapabilities.html): The `VRDisplay` instance is able to use an underlying marker detection technology.
* [hasADFSupport](http://judax.github.io/webar/doc/webarapi/VRDisplayCapabilities.html): The `VRDisplay` instance is able to use ADFs for relocalization of the pose.

If any of these flags are enabled (true), a new set of functionalities and APIs can be used always using the [VRDisplay](http://judax.github.io/webar/doc/webarapi/VRDisplay.html) as a starting point. The new methods in the `VRDisplay` instance are:

* [getMaxNumberOfPointsInPointCloud](http://judax.github.io/webar/doc/webarapi/VRDisplay.html): Provides the maximum number of points that may be acquired in the point cloud.
* [getPointCloud](http://judax.github.io/webar/doc/webarapi/VRDisplay.html): Updates and/or also retrieves the points in the [VRPointCloud](http://judax.github.io/webar/doc/webarapi/VRPointCloud.html) new type.
* [getPickingPointAndPlaneInPointCloud](http://judax.github.io/webar/doc/webarapi/VRDisplay.html): Allows to calculate a collision represented by the new type [VRPointAndPlane](http://judax.github.io/webar/doc/webarapi/VRPickingPointAndPlane.html) between a normalized 2D position and a ray casted on to the point cloud.
* [getSeeThroughCamera](http://judax.github.io/webar/doc/webarapi/VRDisplay.html): Retrieves an instance of the new type [VRSeeThroughCamera](http://judax.github.io/webar/doc/webarapi/VRSeeThroughCamera.html) so it can be used for both correct fustrum calculation and for rendering the camera video feed synchronized with the calculated pose.
* [detectMarkers](http://judax.github.io/webar/doc/webarapi/VRDisplay.html): Detects markers using the video feed. It returns a list of instances of type [VRMarker](http://judax.github.io/webar/doc/webarapi/VRMarker.html) that represent each marker detected. The call has to specify the type of marker to be detected and the physical size in meters of it.
* [getADFs](http://judax.github.io/webar/doc/webarapi/VRDisplay.html): Return the list of ADFs (Area Description File) that are stored in the device. The list includes instances of type [VRADF](http://judax.github.io/webar/doc/webarapi/VRADF.html) that represent each ADF in the device (created with some other tool like the one provided in the tango c examples in the bin folder). 
* [enableADF](http://judax.github.io/webar/doc/webarapi/VRDisplay.html): Enables an ADF that is stored in the device. Once the ADF is enabled, if the system is able to localize according to it, the pose will be based upon it. Check the [VRPose](http://judax.github.io/webar/doc/webarapi/VRPose.html) class to see the new flag that allows to know if the pose has been localized against the ADF or not. Only one ADF can be enabled at a time so enabling a different one will cancel the previous one.
* [disableADF](http://judax.github.io/webar/doc/webarapi/VRDisplay.html): Disables the last ADF that was enabled. Poses won't be localized from that moment on and will be retrieved depending on the start of the execution.

Some new data structures/classes have been created to support some new functionalities as the underlying Tango platform allows new types of interactions/features. Most of the calls are pretty straightforward and the documentation might provide some idea of how they could be integrated in any web application. The one that might need a bit more explanation is the [VRSeeThroughCamera](http://judax.github.io/webar/doc/webarapi/VRSeeThroughCamera.html) class as it provides some useful information about the camera parameters (what are called the camera intrinsics), but it might not be clear how it could be used to render the camera feed in an application. In the current implementation, the approach that has been selected is to create a new overloaded function in the [WebGL API](https://www.khronos.org/registry/webgl/specs/1.0). The [WebGLRenderingContext](https://www.khronos.org/registry/webgl/specs/1.0/#5.14) now exposes the following function:

```
void texImage2D(GLenum target, GLint level, GLenum internalformat, GLenum format, GLenum type, VRSeeThroughCamera? source);
```

This approach has some benefits:

1. There is no need to retrieve the pixels of the image (it is not very efficient to pass a whole frame from native to JavaScript).
2. There is full control over the camera image in WebGL (in a fragment shader for example).
3. It uses a common way to handle video content (`texImage2D` already has some overloads for using `HTMLVideoElement`, `HTMLCanvasElement` or `HTMLImageElement` among others).

There is one caveat with the current implementation that should be taken into account when doing custom rendering with the camera's image as it is handled inside the `texImage2D` call that requires to use an OpenGL extension that is not available in WebGL at the moment: [OES_EGL_image_external](https://www.khronos.org/registry/gles/extensions/OES/OES_EGL_image_external.txt). The Android Chromium WebAR modification in this repository includes the activation of this extension internally. It is important to note that the fragment shader to render the video feed will also need to use the corresponding extension:

```
#extension GL_OES_EGL_image_external : require
...

uniform samplerExternalOES map;
...
```

The best recommendation to better understand the new WebAR API is to review the examples provided in this repository that try to explain some of the new functionalities from the ground up both using plain WebGL and also [ThreeJS](http://threejs.org), the most widely used 3D engine on the web.

Please also review the [Known issues](#known_issues) section to better understand some drawbacks in the form of log and warning messages for using this approach.

## <a name="using_the_webar_apis_in_threejs">Using the WebAR APIs in ThreeJS</a>

**NOTE:** If you are using ThreeJS r86 and above you can stop reading as there is no need to make any modification to ThreeJS to make the Chromium WebAR prototype to work along with it.

As mentioned in the [previous section](#overview_of_the_webar_apis), in order to use the video feed from the underlying Tango platform, there's the need to use a WebGL extension that is not available in the WebGL standard at the moment. The Chromium implementation in this repository activates it so fragment shaders may use it. In the case of ThreeJS, as it internally maps the uniforms in the shaders, a modification to the engine/library is required. It is a simple modification and we are working on making it available in future releases of ThreeJS directly. 

In the `getSingularSetter` function that is able to identify the set-functions for the different types of uniforms/attributes in a shader, a new type needs to be added as follows:

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

See that the case 36198 is the id that identifies the shader uniforms of type `samplerExternalOES` used in the extension.

There are no additional modifications needed to the ThreeJS engine.

A support library has been developed inside this repository under the `THREE.WebAR` folder that provides some functionalities to ease the use of the underlying WebAR APIs in ThreeJS by creating the basic types of structures needed like the `THREE.Mesh` instance that represents the video camera quad (along with the corresponding `THREE.VideoTexture` instance and the right fragment shader), the `THREE.Camera` instance that represents the orthogonal camera to correctly render the video camera feed, a `VRPointCloud` structure that handles a point mesh with a `THREE.BufferGeometry` internally to render the point cloud, etc.

All the documentation for the THREE.WebAR.js file is available at: [http://judax.github.io/webar/doc/THREE.WebAR](http://judax.github.io/webar/doc/THREE.WebAR)

# <a name="how_to_build_your_own_version_of_chromium_with_webar"></a>How to build your own version of Chromium with WebAR

**IMPORTANT**: This repository includes only the modifications of the Chromium repository that allows to add Tango/WebAR capabilities (check the `chromium` folder in this repo). Chromium is a complex project with gigabytes of source code, resources and third party libraries. At some point in the steps below you will need to copy some of the content of this repo to the actual chromium repo. **This means you will need to have 2 repos: this one *chromium-webar) and the chromium repo itself and copy some content from the former to the latter as stated in the step number 9 below.**

Building the modified version of Chromium is a 2 step process: 

1. Clone the Chromium project (copying the changes in this repository) and prepare it to build it.
2. Build, install and run.

## 1. Clone the Chromium project (copying the changes in this repository) and prepare it to be built

Chromium cloning/building instruction are available online: [https://www.chromium.org/developers/how-tos/android-build-instructions](https://www.chromium.org/developers/how-tos/android-build-instructions)

Anyway, in order to help with the process, we recommend you follow the following steps. 

Tango is only available on the Android platform for the moment so in order to be able to use the modifications present in this project, you need to compile Chromium for Android that can only be done on Linux. Unfortunately, this document does not include instructions on how to setup a linux machine.

Let's assume that the machine is installed along with:

* GIT
* Python

Open a terminal window to be able

1. Install depot_tools. You can follow this [tutorial](https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up) or simply follow these 2 steps:
  * `git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git`
  * `export PATH=$PATH:/path/to/depot_tools`
2. Create a folder to contain `chromium`: `$ mkdir ~/chromium && cd ~/chromium`
3. Checkout the Chromium repo: `~/chromium$ fetch --nohooks android`. **NOTE**: This process may take a long time (an hour?)
4. Enter the `src` folder: `$ cd src`.
5. Checkout a specific tag to a new branch. The tag used for this build is `57.0.2987.5`. You can use whatever name you like for the new branch but we recommend `webar_57.0.2987.5` as it states that is webar and the tag is based on: `~/chromium/src$ git checkout -b webar_57.0.2987.5 57.0.2987.5`. Remember the name of the branch (`webar_57.0.2987.5` in our example) as it will be useful in the next step when the output folder is created.
6. Synchronize the dependencies with this command: `~/chromium/src$ gclient sync --disable-syntax-validation`. **NOTE**: This process may take some time too.
7. Create a folder where to make the final product compilation using the same name as the branch created in the previous step: `~/chromium/src$ mkdir -p out/webar_57.0.2987.5` in our example.
8. Create and edit a new file `out/webar_57.0.2987.5/args.gn` with the command `~/chromium/src$ gedit out/webar_57.0.2987.5/args.gn` (or any other editor). If you chose to use a different branch/folder name, please, use that name in this step. Copy and paste the following content in the `args.gn` file:
  ```
  target_os = "android"
  target_cpu = "arm" 

  is_debug = false
  is_component_build = true

  enable_webvr = true

  proprietary_codecs = false
  ffmpeg_branding = "Chromium"

  enable_nacl = false
  remove_webcore_debug_symbols = true
  ```
9. Copy and paste all the content from the `chromium/src` folder inside this repository to the `~/chromium/src` folder just created in the step number 2 above. Override every possible conflict that may arise if you use the file explorer by merging and replacing. You can use the following command line too: `cp -r PATH_TO_THIS_REPOS_SRC_FOLDER/* ~/chromium/src`.
10. Prepare to build: `~/chromium/src$ gn args out/webar_57.0.2987.5`. **NOTE**: once the command is executed, the vi editor will show you the content of the `args.gn` file just edited a few steps before. Just exit by pressing ESC and typing colon and `q` with an exclamation mark = `:q!`.
11. Install the build dependencies: `~/chromium/src$ build/install-build-deps-android.sh` 
12. Synchronize the resources once again: `~/chromium/src$ gclient sync --disable-syntax-validation`
13. Setup the environment: `~/chromium/src$ . build/android/envsetup.sh`

I know, many steps to be followed, but once you have completed all of them (remember that some will take a loooong time to finish), you won't need to execute them again (except from `gclient sync --disable-syntax-validation` that you may need to execute it occassionally if you rebase from one tag to another for example).

## 2. Build, install and run

**IMPORTANT:** some changes have been done to the Chromium command buffer to enable the camera feed rendering. These changes may require to rebuild the command buffer. The Python script to do so does not execute along with the regular building process so the script needs to be executed with the following command at least once (and everytime any change is made to the command buffer):
```
~/chromium/src/python gpu/command_buffer/build_gles2_cmd_buffer.py
```
This tutorial specified that the name of the out folder created during the setup process above is the same as the branch (`webar_57.0.2987.5`). This is no coincidence, as the `build_install_run.sh` shell script provided along with this documentation allows to build the Chromium project depending on the current checked out git branch. This script not only compiles Chromium but also the Tango native library called `tango_chromium` that handle the Tango SDK calls. Moreover, this script also installs the final APK on to a connected device and runs it, so it is convenient that you to connect the Tango device via USB before executing it. The project that will be built by default is the Chromium WebView project, the only one that has been modified to provide Tango/WebAR capabilities.
```
~/chromium/src/build_install_run.sh
```
You can review the content of the script to see what it does (it is a fairly simple script) but if you would like to compile the final APK on your own you could do it by executing the following command:
```
~/chromium/src$ ninja -C out/webar_57.0.2987.5
```
The final APK will be built in the folder `~/chromium/src/out/webar_57.0.2987.5/out/apks`.

## A brief overview on the Chromium source code modifications to support WebAR

**_WORK IN PROGRESS_**

# <a name="supported_devices">Supported devices</a>

The current version of Chromium that supports WebAR has been built on top of Tango and has been tested on the following devices:

* Tango Development Kit Tablet
  * Android version: 4.4.2
  * Tango Core version: 1.53:2017.04.28-hopak-release-0-gb1545f34:190015395:stable
  * Tango SDK version: Hopak

* Lenovo Phab 2 Pro
  * Android version: 6.0.1
  * Tango Core version: 1.53:2017.04.28-hopak-release-0-gb1545f34:230015395:stable
  * Tango SDK version: Hopak

* Asus Zenfone AR
  * Android version: 7.0
  * Tango Core version: 1.55:2017.06.23-release-m20-release-0-gb571120a1:240016802:stable
  * Tango SDK version: Hopak

# <a name="license">License</a>

Apache License Version 2.0 (see the `LICENSE' file inside this repo).

# <a name="known_issues">Known issues</a>

* **The webview flavor of Chromium.** The current implementation of WebAR is built on top of the Chromium WebView flavor. This has some implementation advantages but some performance and use disadvantages. We are working on making the implementation on a full version of Chromium.

* **Black view when pausing/resuming the app.** This is a consequence of having built the implementation on top of the WebView flavor of Chromium. At some point, the Chromium committers have introduced this bug. A proper implementation on full Chromium or a rebase to a different Chromium WebView version might solve this annoying problem.

* **Granting permissions.** Currently, all the neccessary permissions are requested as soon as the application starts. This is not how the web works and the permissions should be requested when needed by the underlying APIs.

* **Console warning: `RENDER WARNING: there is no texture bound to the unit X`.** Because the VRSeeThroughCamera rendering requires the external image texture extension at the moment, the underlying WebGL implementation has a mismatch of the type of texture that has been bound. An initial solution for this warning message that has been tested (and that worked) is to expose the correct target type for the VRSeeThroughCamera texture: `GL_TEXTURE_EXTERNAL_OES`. The problem with approach is that it requieres serious modifications of both Chromium and specially THREEJS so for now, this warning will be shown in the console :(.

* **`Logcat error: TangoEGL: bindTextureImage: error binding external texture image 0xab874310: 0x502`.** A similar consequence to the previous known issue. Tango, while updating the camera image texture is expecting that the texture that is bound is of the correct target `GL_TEXTURE_EXTERNAL_OES` so this GL error is logged.

# <a name="future_work">Future work</a>

* Adapt the implementation to the WebVR spec proposal version 2.0.

* Implement the prototype on full Chromium (not on the WebView flavor) and to a newer tag verion (>57.0.2987.5).

* Improve the VRSeeThroughCamera rendering pipeline either making it obscure for the developer or by using regular WebGL textures and shader samplers without having to use the external image texture extension.

* Add more Tango SDK "hidden" features: mesh reconstruction, ...

* Develop more examples.
  * 6DOF VR example.
  * Fully operational example that shows possibilities of WebAR for online shopping.
-->
