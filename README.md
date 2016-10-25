INTRODUCTION
============

This repository includes modifications on Chromium that allows to add Tango/AR capabilities into JavaScript. Chromium is a complex project with gigabytes of source code, resources and third party libraries. This repository does not include all Chromium but just the files necessary to make the changes to it. This documentation is divided in 2 sections: 1) How to build your own version of Chromium and 2) Using the new VR/AR JavaScript APIs. This way, anyone that wants to build and modify his/her own version of chromium, will be able to do it and those who just want to install a prebuilt version of Chromium and start using it right away.

1.- THE FOLDER STRUCTURE
========================

/android_webview: The modifications to the android webview Chromium project to be able to load the tango handling dynamic library, store the URLs for future executions, etc. This folder, internally, contains the tango handling library and all the build files needed. 

/device: The modifications to the vr device files to add both the tango_vr_device class and the modifications to the vr service to be able to expose some specific data related to AR (point cloud, see through camera data, etc.).

/examples: A set of some basic examples based on straight WebGL or (mostly) using the ThreeJS framework.

/third_party: Both the inclusion of the tango third party libraries (and the tango handling library) and the WebKit vr and webgl IDL classes modifications to be able to expose all the functionalities.

2.- BUILD YOUR OWN VERSION OF CHROMIUM
======================================

Building the modified version of Chromium, is a 3 steps process: 1) Clone the chromium project and prepare it to be built, 2) Copy the chnages in this repository to the Chromium repository, 3) Build, install and run.

2.1) Clone the Chromium project and prepare it to be built
----------------------------------------------------------

Chromium cloning/building instruction are available online. 

https://www.chromium.org/developers/how-tos/android-build-instructions

Anyway, in order to help with the process, we recommend you follow these steps. Remember, Tango is an Android platform for the moment so in order to be able to use the modifications present in this project, you need to compile Chromium for Android. The only way to do this is using a Linux machine. Unfortunately, this document does not include instructions on how to setup a linux machine. Let's assume that the machine is installed along with:

* Java JDK 1.8 and JRE 1.8
* Android SDK
* Android NDK 12b
* GIT
* Setup the PATH variable to point to the tools.

Open a terminal window to be able 
 0.- Install depot_tools: https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up
	0.1.- git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
	0.2.- export PATH=$PATH:/path/to/depot_tools
 1.- $ mkdir ~/chromium && cd ~/chromium
 2.- ~/chromium$ fetch --nohooks android
 * NOTE: This process may take quite some time (an hour?)
 3.- Verify that the .gclient file has "target_os = ['android']" in it
 4.- ~/chromium/src$ gclient sync
 5.- Checkout a specific tag to a new branch. The tag is 54.0.2796.3. The name of the branch to checkout could be "webar_54.0.2796.3" for example.
 	~/chromium/src$ git checkout -b webar_54.0.2796.3 54.0.2796.3
 6.- Create a folder for the final product compilation:
 	~/chromium/src$ mkdir -p out/webar_54.0.2796.3
 7.- Edit the a new file "out/webar_54.0.2796.3/args.gn" file
 	~/chromium/src$ gedit out/webar_54.0.2796.3/args.gn
 Copy and paste this content:

		target_os = "android"
		target_cpu = "arm"  # (default)
		is_debug = true  # (default)

		# Other args you may want to set:
		is_component_build = true
		is_clang = true
		symbol_level = 1  # Faster build with fewer symbols. -g1 rather than -g2
		enable_incremental_javac = true  # Much faster; experimental

 8.- Copy and paste all the content from this folder into the chromium/src folder. Override every possible conflict that may arise if you use the file explorer.
 	cp -r PATH_TO_THIS_FOLDER/* ~/chromium/src
 9.- ~/chromium/src$ gn args out/webar_54.0.2796.3
		NOTE: just exit "q!" in vi when it opens and shows args.gn (modified in the previous step using a proper editor ;))
 10.- ~/chromium$ src/build/install-build-deps-android.sh 
 11.- Execute the following commands and select the right choice on each. Do not worry if some commands do not have any effect.
	sudo update-alternatives --config javac
	sudo update-alternatives --config java
	sudo update-alternatives --config javaws
	sudo update-alternatives --config javap
	sudo update-alternatives --config jar
	sudo update-alternatives --config jarsigner
 12.- ~/chromium/src$ gclient sync
 13.- ~/chromium/src$ . build/android/envsetup.sh

2.2.- Build the project
-----------------------

IMPORTANT: some changes have been done to the Chromium command buffer. These changes may require to rebuild the command buffer. The Python script to do so does not execute along with the regular building process so if further changes are done to the command buffer, this script needs to be executed with the following command:

	~/chromium/src/python gpu/command_buffer/build_gles2_cmd_buffer.py

The out folder that has been created has the same name as the branch. This is no coincidence, as the "build_install_run.sh" shell script provided along with this documentation allows to build the Chromium project depending on the current git branch name that is currently checked out. This script not only compiles Chromium but also the Tango use native library called tango_chromium. This script, apart of building the project it installs it and runs it, so it is convenient that you connect the Tango device via USB. The built project is the Chromium WebView project.
	~/chromium/src/build_install_run.sh

3.- USING THE NEW VR/AR JAVASCRIPT APIs
=======================================

Under construction

4.- DEVELOPING A WEBAR THREEJS APP
==================================

IMPORTANT NOTE: In order to use the external image OES extension, a modification to the ThreeJS engine is required. In the getSingularSetter function that is able to identify the set functions for the different types of uniforms/attributes in a shader, a new type needs to be added as follows:

	function getSingularSetter( type ) {

		switch ( type ) {

			case 0x1406: return setValue1f; // FLOAT
			case 0x8b50: return setValue2fv; // _VEC2
			case 0x8b51: return setValue3fv; // _VEC3
			case 0x8b52: return setValue4fv; // _VEC4

			case 0x8b5a: return setValue2fm; // _MAT2
			case 0x8b5b: return setValue3fm; // _MAT3
			case 0x8b5c: return setValue4fm; // _MAT4

			case 0x8b5e: case 36198: return setValueT1; // SAMPLER_2D  // case 36198: ADDED by WebAR
			case 0x8b60: return setValueT6; // SAMPLER_CUBE

			case 0x1404: case 0x8b56: return setValue1i; // INT, BOOL
			case 0x8b53: case 0x8b57: return setValue2iv; // _VEC2
			case 0x8b54: case 0x8b58: return setValue3iv; // _VEC3
			case 0x8b55: case 0x8b59: return setValue4iv; // _VEC4

		}

	}


