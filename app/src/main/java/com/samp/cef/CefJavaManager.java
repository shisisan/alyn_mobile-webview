package com.samp.cef;

import android.app.Activity;
import android.graphics.Color;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.ConsoleMessage;
import android.webkit.CookieManager;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.FrameLayout;
import org.json.JSONArray;
import org.json.JSONException;

public class CefJavaManager {
    private static final String TAG = "SAMP_CEF_JAVA";

    private final Activity mActivity;
    private final FrameLayout mRootLayout;
    private final Handler mMainHandler = new Handler(Looper.getMainLooper());
    private final Object mLock = new Object();

    private WebView mWebView;
    private CefClientManager mClientManager;

    private boolean mWebViewReady = false;
    private boolean mBridgeAttached = false;
    private String mPendingUrl = null;

    private boolean mIsShown = false;
    private volatile boolean mBrowserFocused = false;

    private WebView mNotifyWebView;
    private boolean mNotifyWebViewReady = false;

    public CefJavaManager(FrameLayout rootLayout, Activity activity) {
        this.mRootLayout = rootLayout;
        this.mActivity = activity;

        mMainHandler.post(this::initWebView);
        mMainHandler.post(this::initNotifyWebView);
    }

    private void initWebView() {
        if (mWebView != null) {
            return;
        }

        mWebView = new WebView(mActivity);
        mWebView.setBackgroundColor(Color.TRANSPARENT);
        mWebView.setLayerType(View.LAYER_TYPE_HARDWARE, null);

        WebSettings settings = mWebView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setDatabaseEnabled(true);
        settings.setAllowFileAccess(true);
        settings.setAllowContentAccess(true);
        settings.setAllowFileAccessFromFileURLs(true);
        settings.setAllowUniversalAccessFromFileURLs(true);
        settings.setMediaPlaybackRequiresUserGesture(false);

        CookieManager.getInstance().setAcceptCookie(true);
        CookieManager.getInstance().setAcceptThirdPartyCookies(mWebView, true);

        mWebView.setWebChromeClient(new WebChromeClient() {
            @Override
            public boolean onConsoleMessage(ConsoleMessage consoleMessage) {
                Log.d(TAG, "console says " + consoleMessage.message() +
                        " -- From line " + consoleMessage.lineNumber() + " of " + consoleMessage.sourceId());
                return true;
            }
        });

        mWebView.setWebViewClient(new WebViewClient() {
            @Override
            public void onPageFinished(WebView view, String url) {
                super.onPageFinished(view, url);
                Log.d(TAG, "Page loaded: " + url);
                if (mClientManager != null) {
                    mClientManager.onBrowserInit(true, -1);
                }
            }

            @Override
            public void onReceivedError(WebView view, int errorCode, String description, String failingUrl) {
                super.onReceivedError(view, errorCode, description, failingUrl);
                Log.e(TAG, "WebView error (" + errorCode + "): " + description);
                if (mClientManager != null) {
                    mClientManager.onBrowserInit(false, errorCode);
                }
            }
        });

        mWebView.setVisibility(View.GONE);
        mRootLayout.addView(mWebView, new FrameLayout.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT
        ));

        attachJsBridgeIfNeeded();

        String pending;
        synchronized (mLock) {
            mWebViewReady = true;
            pending = mPendingUrl;
            mPendingUrl = null;
        }

        Log.i(TAG, "WebView initialized");

        if (pending != null) {
            Log.i(TAG, "Loading pending URL: " + pending);
            loadUrlInternal(pending);
            showBrowserViewInternal();
        }
    }

    private void initNotifyWebView() {
        if (mNotifyWebView != null) return;

        mNotifyWebView = new WebView(mActivity);
        mNotifyWebView.setBackgroundColor(Color.TRANSPARENT);
        mNotifyWebView.setLayerType(View.LAYER_TYPE_SOFTWARE, null);

        WebSettings settings = mNotifyWebView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);

        mNotifyWebView.setWebViewClient(new WebViewClient() {
            @Override
            public void onPageFinished(WebView view, String url) {
                super.onPageFinished(view, url);
                mNotifyWebViewReady = true;
                Log.i(TAG, "Notify overlay WebView ready");
            }
        });

        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(
                dpToPx(300), dpToPx(500)
        );
        params.gravity = android.view.Gravity.TOP | android.view.Gravity.START;
        mRootLayout.addView(mNotifyWebView, params);

        mNotifyWebView.loadUrl("file:///android_asset/cef/notify.html");
    }

    private int dpToPx(int dp) {
        float density = mActivity.getResources().getDisplayMetrics().density;
        return Math.round(dp * density);
    }

    public void setClientManager(CefClientManager clientManager) {
        this.mClientManager = clientManager;
        mMainHandler.post(this::attachJsBridgeIfNeeded);
    }

    private void attachJsBridgeIfNeeded() {
        if (mBridgeAttached) return;
        if (mWebView == null || mClientManager == null) return;

        mWebView.addJavascriptInterface(new CefWebInterface(mClientManager), "AndroidCefBridge");
        mBridgeAttached = true;
    }

    public void openBrowser(String url) {
        Log.i(TAG, "openBrowser requested: " + url);

        boolean ready;
        synchronized (mLock) {
            ready = mWebViewReady;
            if (!ready) {
                mPendingUrl = url;
                Log.i(TAG, "WebView not ready, storing pending URL");
            }
        }

        if (ready) {
            loadUrlInternal(url);
            showBrowserViewInternal();
        }
    }

    public void loadUrl(String url) {
        if (url == null || url.isEmpty()) {
            Log.e(TAG, "loadUrl failed: url is null/empty");
            return;
        }
        loadUrlInternal(url);
    }

    private void loadUrlInternal(String url) {
        mMainHandler.post(() -> {
            if (mWebView == null) {
                Log.e(TAG, "loadUrl failed: WebView is not initialized yet, url=" + url);
                return;
            }
            Log.d(TAG, "Loading URL: " + url);
            mWebView.loadUrl(url);
        });
    }

    public void showBrowserView() {
        showBrowserViewInternal();
    }

    private void showBrowserViewInternal() {
        mMainHandler.post(() -> {
            if (mWebView == null) {
                Log.e(TAG, "showBrowserView failed: WebView is not initialized yet");
                return;
            }
            Log.i(TAG, "Showing WebView");
            mWebView.setLayerType(View.LAYER_TYPE_HARDWARE, null);
            mWebView.setAlpha(0f);
            mWebView.setVisibility(View.VISIBLE);
            mWebView.animate().alpha(1f).setDuration(200).start();
            mIsShown = true;
        });
    }

    public void hideBrowserView() {
        mMainHandler.post(() -> {
            if (mWebView == null) {
                Log.e(TAG, "hideBrowserView failed: WebView is not initialized yet");
                return;
            }
            Log.i(TAG, "Hiding WebView");
            mWebView.animate().alpha(0f).setDuration(150).withEndAction(() -> {
                mWebView.setVisibility(View.GONE);
                mWebView.setLayerType(View.LAYER_TYPE_NONE, null);
                mIsShown = false;
                mRootLayout.invalidate();
            }).start();
        });
    }

    public boolean isShow() {
        return mIsShown;
    }

    public WebView getWebView() {
        return mWebView;
    }

    public boolean isBrowserFocused() {
        return mBrowserFocused;
    }

    public void setBrowserFocus(final boolean isFocused) {
        mBrowserFocused = isFocused;
        mMainHandler.post(() -> {
            if (mWebView == null) return;
            if (isFocused) {
                mWebView.requestFocus();
            } else {
                mWebView.clearFocus();
            }
            Log.d(TAG, "Browser focus set to: " + isFocused);
        });
    }

    public void dispatchServerEvent(final String eventName, final String eventData) {
        if ("cef_ui_notification".equals(eventName)) {
            dispatchToNotifyWebView(eventName, eventData);
            return;
        }

        mMainHandler.post(() -> {
            if (mWebView == null) {
                Log.w(TAG, "dispatchServerEvent: WebView was not ready, event '" + eventName + "' dropped");
                return;
            }

            if ("_cef_eval".equals(eventName)) {
                try {
                    JSONArray arr = new JSONArray(eventData);
                    String code = arr.getString(0);
                    mWebView.evaluateJavascript(code, null);
                } catch (JSONException e) {
                    Log.e(TAG, "Failed to parse _cef_eval payload: " + eventData, e);
                }
                return;
            }

            String safeName = escapeJsString(eventName);
            String safeData = escapeJsString(eventData);
            String js = "window.dispatchEvent(new CustomEvent(\"" + safeName +
                    "\", { detail: \"" + safeData + "\" }));";
            mWebView.evaluateJavascript(js, null);
        });
    }

    private void dispatchToNotifyWebView(final String eventName, final String eventData) {
        mMainHandler.post(() -> {
            if (mNotifyWebView == null || !mNotifyWebViewReady) {
                Log.w(TAG, "dispatchToNotifyWebView: notify overlay is not ready, event was droped.");
                return;
            }
            String safeName = escapeJsString(eventName);
            String safeData = escapeJsString(eventData);
            String js = "window.dispatchEvent(new CustomEvent(\"" + safeName +
                    "\", { detail: \"" + safeData + "\" }));";
            mNotifyWebView.evaluateJavascript(js, null);
        });
    }

    // Escape for bypassing escaoe string in the payload
    private static String escapeJsString(String s) {
        if (s == null) return "";
        return s.replace("\\", "\\\\")
                .replace("\"", "\\\"")
                .replace("\n", "\\n")
                .replace("\r", "\\r")
                .replace("\u2028", "\\u2028")
                .replace("\u2029", "\\u2029");
    }

    public static class CefWebInterface {
        private final CefClientManager mClientManager;

        public CefWebInterface(CefClientManager clientManager) {
            this.mClientManager = clientManager;
        }

        @android.webkit.JavascriptInterface
        public void sendEvent(String eventName, String eventDataJson) {
            if (mClientManager != null) {
                mClientManager.sendClientEvent(eventName, eventDataJson);
            }
        }
    }
}