// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.android_webview.shell;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.ServiceConnection;
import android.content.ComponentName;
import android.content.DialogInterface;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.os.IBinder;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnFocusChangeListener;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.view.Display;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.webkit.GeolocationPermissions;
import android.webkit.WebChromeClient;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
import android.support.v4.content.ContextCompat;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.ActivityCompat.OnRequestPermissionsResultCallback;
import android.graphics.Point;
import android.hardware.Camera;

import org.chromium.android_webview.AwBrowserContext;
import org.chromium.android_webview.AwBrowserProcess;
import org.chromium.android_webview.AwContents;
import org.chromium.android_webview.AwContentsClient;
import org.chromium.android_webview.AwDevToolsServer;
import org.chromium.android_webview.AwSettings;
import org.chromium.android_webview.test.AwTestContainerView;
import org.chromium.android_webview.test.NullContentsClient;
import org.chromium.android_webview.JsResultReceiver;
import org.chromium.android_webview.JsPromptResultReceiver;
import org.chromium.android_webview.AwWebResourceResponse;
import org.chromium.android_webview.AwContentsClient.AwWebResourceRequest;
import org.chromium.base.BaseSwitches;
import org.chromium.base.CommandLine;
import org.chromium.base.ContextUtils;
import org.chromium.base.Log;
import org.chromium.base.TraceEvent;
import org.chromium.content.app.ContentApplication;
import org.chromium.content_public.browser.NavigationController;
import org.chromium.content_public.browser.WebContents;

import java.net.URI;
import java.net.URL;
import java.net.URISyntaxException;
import java.net.MalformedURLException;
import java.util.Locale;

import com.google.zxing.integration.android.IntentIntegrator;
import com.google.zxing.integration.android.IntentResult;

/**
 * This is a lightweight activity for tests that only require WebView functionality.
 */
public class AwShellActivity extends Activity implements OnRequestPermissionsResultCallback {
    private static final String TAG = "cr.AwShellActivity";
    private static final String PREFERENCES_NAME = "AwShellPrefs";
    private static final String INITIAL_URL = "about:blank";
    private static final String LAST_USED_URL_PREFERENCE_NAME = "url";
    private static final int CAMERA_PERMISSION_ID = 1;
    private static final int CAMERA_ID = 0;
    private Point mScreenSize = new Point();
    private AwBrowserContext mBrowserContext;
    private AwDevToolsServer mDevToolsServer;
    private AwTestContainerView mAwTestContainerView;
    private WebContents mWebContents;
    private NavigationController mNavigationController;
    private EditText mUrlTextView;
    private ImageButton mPrevButton;
    private ImageButton mNextButton;
    private ImageButton mQRCodeButton;

    private static AlertDialog createAlertDialog(Context context, String title,
            String message, DialogInterface.OnClickListener onClickListener,
            int numberOfButtons, String yesButtonText, String noButtonText,
            String cancelButtonText)
    {
        AlertDialog alertDialog = new AlertDialog.Builder(context).create();
        alertDialog.setTitle(title);
        alertDialog.setMessage(message);
        alertDialog.setButton(AlertDialog.BUTTON_POSITIVE, yesButtonText,
                onClickListener);
        if (numberOfButtons > 1)
        {
            alertDialog.setButton(AlertDialog.BUTTON_NEGATIVE, noButtonText,
                    onClickListener);
        }
        if (numberOfButtons > 2)
        {
            alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, cancelButtonText,
                    onClickListener);
        }
        return alertDialog;
    }    

    private static AlertDialog createPromptDialog(Context context, EditText editText, String title,
            String message, DialogInterface.OnClickListener onClickListener,
            int numberOfButtons, String yesButtonText, String noButtonText,
            String cancelButtonText)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        builder.setView(editText);        
        AlertDialog alertDialog = builder.create();
        alertDialog.setTitle(title);
        alertDialog.setMessage(message);
        alertDialog.setButton(AlertDialog.BUTTON_POSITIVE, yesButtonText,
                onClickListener);
        if (numberOfButtons > 1)
        {
            alertDialog.setButton(AlertDialog.BUTTON_NEGATIVE, noButtonText,
                    onClickListener);
        }
        if (numberOfButtons > 2)
        {
            alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, cancelButtonText,
                    onClickListener);
        }
        return alertDialog;
    }    

    // Tango Service connection.
    ServiceConnection mTangoServiceConnection = new ServiceConnection()
    {
        public void onServiceConnected(ComponentName name, IBinder service)
        {
            TangoJniNative.onTangoServiceConnected(service);
        }

        public void onServiceDisconnected(ComponentName name)
        {
            // Handle this if you need to gracefully shutdown/retry
            // in the event that Tango itself crashes/gets upgraded while
            // running.
        }
    };

    private void saveStringToPreferences(String name, String value) 
    {
        Editor editor = getPreferences(Activity.MODE_PRIVATE).edit();
        editor.putString(name, value);
        if (!editor.commit())
        {
            System.err.println("ERROR: Could not save the preference: " + name + " - " + value);
        }
    }       

    private void requestCameraPermission() 
    {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_CONTACTS) != PackageManager.PERMISSION_GRANTED) 
        {
            // Should we show an explanation?
            // if (ActivityCompat.shouldShowRequestPermissionRationale(this, Manifest.permission.READ_CONTACTS)) 
            // {

            //     // Show an expanation to the user *asynchronously* -- don't block
            //     // this thread waiting for the user's response! After the user
            //     // sees the explanation, try again to request the permission.

            // } 
            // else 
            {

                // No explanation needed, we can request the permission.

                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.CAMERA},
                        CAMERA_PERMISSION_ID);

                // MY_PERMISSIONS_REQUEST_READ_CONTACTS is an
                // app-defined int constant. The callback method gets the
                // result of the request.
            }
        }
    }    

   @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {

        if (requestCode == CAMERA_PERMISSION_ID) {

            // Received permission result for camera permission.est.");
            // Check if the only required permission has been granted
            if (grantResults.length == 1 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            } else {
            }
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestCameraPermission();

        WindowManager windowManager = (WindowManager) getSystemService(WINDOW_SERVICE);
        Display display = windowManager.getDefaultDisplay();
        display.getSize(mScreenSize);
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(CAMERA_ID, info);
        TangoJniNative.onCreate(this, display.getRotation(), info.orientation);

        CommandLine.init(new String[] { "chrome", "--ignore-gpu-blacklist", "--enable-webvr" });

        AwShellResourceProvider.registerResources(this);

        ContentApplication.initCommandLine(this);
        waitForDebuggerIfNeeded();

        ContextUtils.initApplicationContext(getApplicationContext());
        AwBrowserProcess.loadLibrary();

        if (CommandLine.getInstance().hasSwitch(AwShellSwitches.ENABLE_ATRACE)) {
            Log.e(TAG, "Enabling Android trace.");
            TraceEvent.setATraceEnabled(true);
        }

        setContentView(R.layout.testshell_activity);

        mAwTestContainerView = createAwTestContainerView();

        mWebContents = mAwTestContainerView.getContentViewCore().getWebContents();
        mNavigationController = mWebContents.getNavigationController();
        LinearLayout contentContainer = (LinearLayout) findViewById(R.id.content_container);
        mAwTestContainerView.setLayoutParams(new LinearLayout.LayoutParams(
                LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT, 1f));
        contentContainer.addView(mAwTestContainerView);
        mAwTestContainerView.requestFocus();

        initializeUrlField();
        initializeNavigationButtons();

        mAwTestContainerView.getAwContents().clearCache(true);        

        String startupUrl = getUrlFromIntent(getIntent());
        if (TextUtils.isEmpty(startupUrl)) {
            startupUrl = getPreferences(Activity.MODE_PRIVATE).getString(LAST_USED_URL_PREFERENCE_NAME, INITIAL_URL);
        }

        mAwTestContainerView.getAwContents().loadUrl(startupUrl);
        AwContents.setShouldDownloadFavicons();
        mUrlTextView.setText(startupUrl);
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        TangoInitializationHelper.bindTangoService(this, mTangoServiceConnection);
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        // Disconnect from Tango Service, release all the resources that the app is holding from Tango Service.
        TangoJniNative.onPause();
        unbindService(mTangoServiceConnection);
    }

    @Override
    public void onDestroy() {

        TangoJniNative.onDestroy();
        
        if (mDevToolsServer != null) {
            mDevToolsServer.destroy();
            mDevToolsServer = null;
        }
        super.onDestroy();
    }

    private AwTestContainerView createAwTestContainerView() {
        AwBrowserProcess.start();
        AwTestContainerView testContainerView = new AwTestContainerView(this, true);
        AwContentsClient awContentsClient = new NullContentsClient() {
            private View mCustomView;

            @Override
            public void handleJsAlert(String url, String message, final JsResultReceiver receiver) {
                createAlertDialog(AwShellActivity.this, url, message, new DialogInterface.OnClickListener()
                {
                    @Override
                    public void onClick(DialogInterface dialog, int which)
                    {
                        receiver.confirm();
                    }
                }, 1, "Ok", null, null).show();                
            }

            @Override
            public void handleJsPrompt(String url, String message, String defaultValue, final JsPromptResultReceiver receiver) {
                final EditText editText = new EditText(AwShellActivity.this);
                createPromptDialog(AwShellActivity.this, editText, url, message, new DialogInterface.OnClickListener()
                {
                    @Override
                    public void onClick(DialogInterface dialog, int which)
                    {
                        if (which == DialogInterface.BUTTON_POSITIVE)
                        {
                            receiver.confirm(editText.getText().toString());
                        }
                        else 
                        {
                            receiver.cancel();
                        }
                    }
                }, 2, "Ok", "Cancel", null).show();                
            }

            @Override
            public void handleJsConfirm(String url, String message, final JsResultReceiver receiver) {
                createAlertDialog(AwShellActivity.this, url, message, new DialogInterface.OnClickListener()
                {
                    @Override
                    public void onClick(DialogInterface dialog, int which)
                    {
                        if (which == DialogInterface.BUTTON_POSITIVE) 
                        {
                            receiver.confirm();
                        }
                        else 
                        {
                            receiver.cancel();
                        }

                    }
                }, 2, "Yes", "No", null).show();                
            }

            @Override
            public void onReceivedError(int errorCode, String description, String failingUrl) { 
                createAlertDialog(AwShellActivity.this, "ERROR: " + errorCode, failingUrl + ": " + description, null, 1, "Ok", null, null).show();                
            }

            // @Override
            // public void onReceivedError2(AwWebResourceRequest request, AwWebResourceError error) {
            //     String failingUrl = request.url;
            //     int errorCode = error.errorCode;
            //     String description = error.description;
            //     createAlertDialog(AwShellActivity.this, "ERROR: " + errorCode, failingUrl + ": " + description, null, 1, "Ok", null, null).show();                
            // }

            @Override
            public void onReceivedHttpError(AwWebResourceRequest request, AwWebResourceResponse response) {
                String failingUrl = request.url;
                // HACK! Do not show the icon loading error.
                if (failingUrl.toLowerCase(Locale.getDefault()).contains("favicon.ico")) return;
                int errorCode = response.getStatusCode();
                String description = response.getReasonPhrase();
                createAlertDialog(AwShellActivity.this, "HTTP ERROR: " + errorCode, failingUrl + ": " + description, null, 1, "Ok", null, null).show();                
            }

            @Override
            public void onPageStarted(String url) {
                if (mUrlTextView != null) {
                    mUrlTextView.setText(url);
                    saveStringToPreferences(LAST_USED_URL_PREFERENCE_NAME, url);
                }
            }

            @Override
            public void onShowCustomView(View view, WebChromeClient.CustomViewCallback callback) {
                getWindow().setFlags(
                        WindowManager.LayoutParams.FLAG_FULLSCREEN,
                        WindowManager.LayoutParams.FLAG_FULLSCREEN);

                getWindow().addContentView(view,
                        new FrameLayout.LayoutParams(
                                ViewGroup.LayoutParams.MATCH_PARENT,
                                ViewGroup.LayoutParams.MATCH_PARENT,
                                Gravity.CENTER));
                mCustomView = view;
            }

            @Override
            public void onHideCustomView() {
                getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
                FrameLayout decorView = (FrameLayout) getWindow().getDecorView();
                decorView.removeView(mCustomView);
                mCustomView = null;
            }

            @Override
            public boolean shouldOverrideKeyEvent(KeyEvent event) {
                if (event.getKeyCode() == KeyEvent.KEYCODE_BACK) {
                    return true;
                }
                return false;
            }

            @Override
            public void onGeolocationPermissionsShowPrompt(String origin,
                    GeolocationPermissions.Callback callback) {
                callback.invoke(origin, false, false);
            }
        };

        SharedPreferences sharedPreferences =
                getSharedPreferences(PREFERENCES_NAME, Context.MODE_PRIVATE);
        if (mBrowserContext == null) {
            mBrowserContext = new AwBrowserContext(sharedPreferences, getApplicationContext());
        }
        final AwSettings awSettings = new AwSettings(this /* context */,
                false /* isAccessFromFileURLsGrantedByDefault */, false /* supportsLegacyQuirks */,
                false /* allowEmptyDocumentPersistence */,
                true /* allowGeolocationOnInsecureOrigins */);
        // Required for WebGL conformance tests.
        awSettings.setMediaPlaybackRequiresUserGesture(false);
        // Allow zoom and fit contents to screen
        awSettings.setBuiltInZoomControls(true);
        awSettings.setDisplayZoomControls(false);
        awSettings.setUseWideViewPort(true);
        awSettings.setLoadWithOverviewMode(true);
        awSettings.setLayoutAlgorithm(android.webkit.WebSettings.LayoutAlgorithm.TEXT_AUTOSIZING);

        testContainerView.initialize(new AwContents(mBrowserContext, testContainerView,
                testContainerView.getContext(), testContainerView.getInternalAccessDelegate(),
                testContainerView.getNativeDrawGLFunctorFactory(), awContentsClient, awSettings));
        testContainerView.getAwContents().getSettings().setJavaScriptEnabled(true);
        if (mDevToolsServer == null) {
            mDevToolsServer = new AwDevToolsServer();
            mDevToolsServer.setRemoteDebuggingEnabled(true);
        }
        return testContainerView;
    }

    private static String getUrlFromIntent(Intent intent) {
        return intent != null ? intent.getDataString() : null;
    }

    private void setKeyboardVisibilityForUrl(boolean visible) {
        InputMethodManager imm = (InputMethodManager) getSystemService(
                Context.INPUT_METHOD_SERVICE);
        if (visible) {
            imm.showSoftInput(mUrlTextView, InputMethodManager.SHOW_IMPLICIT);
        } else {
            imm.hideSoftInputFromWindow(mUrlTextView.getWindowToken(), 0);
        }
    }

    private void initializeUrlField() {
        mUrlTextView = (EditText) findViewById(R.id.url);
        mUrlTextView.setOnEditorActionListener(new OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                if ((actionId != EditorInfo.IME_ACTION_GO) && (event == null
                        || event.getKeyCode() != KeyEvent.KEYCODE_ENTER
                        || event.getAction() != KeyEvent.ACTION_DOWN)) {
                    return false;
                }

                String url = mUrlTextView.getText().toString();
                try {
                    URI uri = new URI(url);
                    if (uri.getScheme() == null) {
                        url = "http://" + uri.toString();
                    } else {
                        url = uri.toString();
                    }
                } catch (URISyntaxException e) {
                    // Ignore syntax errors.
                }
                mAwTestContainerView.getAwContents().loadUrl(url);
                mUrlTextView.clearFocus();
                setKeyboardVisibilityForUrl(false);
                mAwTestContainerView.requestFocus();
                return true;
            }
        });
        mUrlTextView.setOnFocusChangeListener(new OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                setKeyboardVisibilityForUrl(hasFocus);
                mNextButton.setVisibility(hasFocus ? View.GONE : View.VISIBLE);
                mPrevButton.setVisibility(hasFocus ? View.GONE : View.VISIBLE);
                if (!hasFocus) {
                    mUrlTextView.setText(mWebContents.getUrl());
                }
            }
        });
    }

    private void initializeNavigationButtons() {
        mPrevButton = (ImageButton) findViewById(R.id.prev);
        mPrevButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mNavigationController.canGoBack()) {
                    mNavigationController.goBack();
                }
            }
        });

        mNextButton = (ImageButton) findViewById(R.id.next);
        mNextButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mNavigationController.canGoForward()) {
                    mNavigationController.goForward();
                }
            }
        });

        mQRCodeButton = (ImageButton) findViewById(R.id.qrcodeImageButton);
        mQRCodeButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                IntentIntegrator intentIntegrator = new IntentIntegrator(AwShellActivity.this);
                intentIntegrator.initiateScan();
            }
        });
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            if (mNavigationController.canGoBack()) {
                mNavigationController.goBack();
                return true;
            }
        }

        return super.onKeyUp(keyCode, event);
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        WindowManager windowManager = (WindowManager) getSystemService(WINDOW_SERVICE);
        Display display = windowManager.getDefaultDisplay();
        display.getSize(mScreenSize);
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(CAMERA_ID, info);

        TangoJniNative.onConfigurationChanged(display.getRotation(), info.orientation);
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent intent) 
    {
        IntentResult scanResult = IntentIntegrator.parseActivityResult(requestCode, resultCode, intent);
        if (scanResult != null && scanResult.getContents() != null) 
        {
            String url = scanResult.getContents();
            try
            {
                new URL(url);
                mUrlTextView.setText(url);
                mAwTestContainerView.getAwContents().loadUrl(url);
                mUrlTextView.clearFocus();
                setKeyboardVisibilityForUrl(false);
                mAwTestContainerView.requestFocus();
            }
            catch(MalformedURLException e)
            {               
                createAlertDialog(this, "Not an URL", "The read QRCode does not represent a valid URL.", null, 1, "Ok", null, null).show();
            }
        }
    }

/*
    boolean firstTouchDone = false;

    @Override 
    public boolean dispatchTouchEvent( android.view.MotionEvent event )
    {
        android.view.Display display = getWindowManager().getDefaultDisplay(); 
        int width = display.getWidth();  // deprecated
        int height = display.getHeight();  // deprecated
        int action = event.getAction();
        float x = event.getRawX();
        float y = event.getRawY();

        System.out.println("x = " + x  + ", width / 2 = " + width / 2);

        if (!firstTouchDone && x > (width >> 1))
        {
//            mAwTestContainerView.getAwContents().loadUrl("http://judax.github.io/tango/");
            mAwTestContainerView.getAwContents().loadUrl("http://192.168.0.104/judax/tango/video3js/");
            mUrlTextView.clearFocus();
            setKeyboardVisibilityForUrl(false);
            mAwTestContainerView.requestFocus();
            firstTouchDone = true;
            return true;
        }
        else
        {
            return super.dispatchTouchEvent(event);
        }
    }
*/

    private void waitForDebuggerIfNeeded() {
        if (CommandLine.getInstance().hasSwitch(BaseSwitches.WAIT_FOR_JAVA_DEBUGGER)) {
            Log.e(TAG, "Waiting for Java debugger to connect...");
            android.os.Debug.waitForDebugger();
            Log.e(TAG, "Java debugger connected. Resuming execution.");
        }
    }
}
