package com.samp.cef;

import android.app.Activity;
import android.util.Log;

public class CefClientManager {
    private static final String TAG = "SAMP_CEF_CLIENT";
    private final Activity mActivity;
    private CefJavaManager mJavaManager;

    private static volatile CefClientManager sInstance;

    static {
        try {
            System.loadLibrary("SAMPMobileCef");
            Log.i(TAG, "libSAMPMobileCef.so loaded successfully.");
        } catch (UnsatisfiedLinkError e) {
            Log.w(TAG, "libSAMPMobileCef library link pending: " + e.getMessage());
        }
    }

    public CefClientManager(Activity activity) {
        this.mActivity = activity;
        sInstance = this;
    }

    public static CefClientManager getInstance() {
        return sInstance;
    }

    public void release() {
        if (sInstance == this) {
            sInstance = null;
        }
    }

    public void setJavaManager(CefJavaManager javaManager) {
        this.mJavaManager = javaManager;
    }

    public CefJavaManager getJavaManager() {
        return mJavaManager;
    }

    public void openBrowser(String url) {
        Log.i(TAG, "openBrowser: " + url);

        if (url == null || url.isEmpty()) {
            Log.e(TAG, "openBrowser failed: url is null/empty");
            return;
        }

        if (mJavaManager == null) {
            Log.e(TAG, "openBrowser failed: CefJavaManager is not set (call setJavaManager() first)");
            return;
        }

        mJavaManager.openBrowser(url);
    }

    public static void staticOpenBrowser(String url) {
        CefClientManager instance = sInstance;
        if (instance == null) {
            Log.e(TAG, "staticOpenBrowser failed: no CefClientManager instance available yet");
            return;
        }
        instance.openBrowser(url);
    }

    public void closeBrowser() {
        Log.i(TAG, "closeBrowser");

        if (mJavaManager == null) {
            Log.e(TAG, "closeBrowser failed: CefJavaManager is not set (call setJavaManager() first)");
            return;
        }

        mJavaManager.hideBrowserView();
    }

    public static void staticCloseBrowser() {
        CefClientManager instance = sInstance;
        if (instance == null) {
            Log.e(TAG, "staticCloseBrowser failed: no CefClientManager instance available yet");
            return;
        }
        instance.closeBrowser();
    }

    public void showBrowser() {
        Log.i(TAG, "showBrowser");
        if (mJavaManager == null) {
            Log.e(TAG, "showBrowser failed: CefJavaManager is not set");
            return;
        }
        mJavaManager.showBrowserView();
    }

    public static void staticShowBrowser() {
        CefClientManager instance = sInstance;
        if (instance == null) { Log.e(TAG, "staticShowBrowser failed: no instance"); return; }
        instance.showBrowser();
    }

    public void hideBrowser() {
        if (mJavaManager != null) {
            mJavaManager.hideBrowserView();
        }
    }

    public static void staticHideBrowser() {
        if (sInstance != null) {
            sInstance.hideBrowser();
        } else {
            Log.w(TAG, "staticHideBrowser: sInstance null");
        }
    }

    public void setBrowserUrl(String url) {
        Log.i(TAG, "setBrowserUrl: " + url);
        if (url == null || url.isEmpty()) {
            Log.e(TAG, "setBrowserUrl failed: url is null/empty");
            return;
        }
        if (mJavaManager == null) {
            Log.e(TAG, "setBrowserUrl failed: CefJavaManager is not set");
            return;
        }
        mJavaManager.loadUrl(url);
    }

    public static void staticSetBrowserUrl(String url) {
        CefClientManager instance = sInstance;
        if (instance == null) { Log.e(TAG, "staticSetBrowserUrl failed: no instance"); return; }
        instance.setBrowserUrl(url);
    }

    public void setBrowserFocus(boolean isFocused) {
        if (mJavaManager != null) {
            mJavaManager.setBrowserFocus(isFocused);
        }
    }

    public static void staticSetBrowserFocus(boolean isFocused) {
        if (sInstance != null) {
            sInstance.setBrowserFocus(isFocused);
        } else {
            Log.w(TAG, "staticSetBrowserFocus: sInstance null");
        }
    }

    public void dispatchServerEvent(String eventName, String eventData) {
        if (mJavaManager != null) {
            mJavaManager.dispatchServerEvent(eventName, eventData);
        }
    }

    public static void staticDispatchServerEvent(String eventName, String eventData) {
        if (sInstance != null) {
            sInstance.dispatchServerEvent(eventName, eventData);
        } else {
            Log.w(TAG, "staticDispatchServerEvent: sInstance null");
        }
    }

    public void onBrowserInit(boolean isSuccess, int errorCode) {
        try {
            nativeOnBrowserInit(isSuccess, errorCode);
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "nativeOnBrowserInit call failed: " + e.getMessage());
        }
    }

    public void sendClientEvent(String eventName, String eventDataJson) {
        try {
            nativeSendClientEvent(eventName, eventDataJson);
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "nativeSendClientEvent call failed: " + e.getMessage());
        }
    }

    private native void nativeOnBrowserInit(boolean isSuccess, int errorCode);
    private native void nativeSendClientEvent(String eventName, String eventDataJson);
}