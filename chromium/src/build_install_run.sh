clear; reset;
# A crappy way to know if GOMA is being used to build chromium.
# TODO: Parse the args.gn file to know if goma is actually being used or not.
if [ -n "$GOMA_DIR" ]; then
	echo "Making sure that goma is started..."
	python "$GOMA_DIR/goma_ctl.py" ensure_start
	echo "Ensured that goma has started!"
fi
echo "Rebuilding libtango.so..."
cd android_webview/test/shell/tango/jni
./ndkbuild.sh
if [ $? -eq 0 ]; then
	cd ../../../../..
	BRANCH_NAME=$(git symbolic-ref -q HEAD)
	BRANCH_NAME=${BRANCH_NAME##refs/heads/}
	BRANCH_NAME=${BRANCH_NAME:-HEAD}
	APK_TYPE="android_webview_apk"
	if [ $# -eq 1 ]; then
		APK_TYPE="$1"
	fi

	if [ "$APK_TYPE" == "chrome_public_apk" ]; then
		APK_FILE_NAME="ChromePublic"
		APK_RUN_SCRIPT_NAME="adb_run_chrome_public"
		COMMAND_LINE="adb_chrome_public_command_line"
	elif [ "$APK_TYPE" == "android_webview_apk" ]; then
		APK_FILE_NAME="AndroidWebView"
		APK_RUN_SCRIPT_NAME="adb_run_android_webview_shell"
		COMMAND_LINE="adb_android_webview_command_line"
	elif [ "$APK_TYPE" == "content_shell_apk" ]; then
		APK_FILE_NAME="ContentShell"
		APK_RUN_SCRIPT_NAME="adb_run_content_shell"
		COMMAND_LINE="adb_content_shell_command_line"
	else
		echo "ERROR: The provided apk target name to be built '$APK_TYPE' is not one of the 3 possibilities: chrome_public_apk, android_webview_apk, content_shell_apk"
		exit 1
	fi
	echo "Regenerating ninja files from GN..."
	gn gen "out/$BRANCH_NAME"
	if [ $? -eq 0 ]; then
		echo "Regenerated!"
		echo "Building $APK_TYPE for Android at 'out/$BRANCH_NAME' folder..."
		NINJA_RESULT=0
		# A crappy way to know if GOMA is being used to build chromium.
		# TODO: Parse the args.gn file to know if goma is actually being used or not.
		if [ -z "$GOMA_DIR" ]; then
			ninja -C "out/$BRANCH_NAME $APK_TYPE"
			NINJA_RESULT=$?
		else
			ninja -C "out/$BRANCH_NAME" -j 320 "$APK_TYPE"
			NINJA_RESULT=$?
		fi
		if [ $NINJA_RESULT -eq 0 ]; then
			echo "Built!"
			echo "Installing $APK_TYPE on Android device..."
			adb install -r "out/$BRANCH_NAME/apks/$APK_FILE_NAME.apk"
			if [ $? -eq 0 ]; then
				echo "Installed!"
				# echo "Setting up Chrome flags..."
				# build/android/$COMMAND_LINE --ignore-gpu-blacklist --enable-webvr
				# if [ $? -eq 0 ]; then
				# 	echo "Chrome flags set!"
					build/android/"$APK_RUN_SCRIPT_NAME"
				# fi
			fi
		fi
	fi
fi
