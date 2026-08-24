package ro.alynsampmobile.launcher;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.core.app.NotificationCompat;

import com.downloader.Error;
import com.downloader.OnDownloadListener;
import com.downloader.PRDownloader;
import com.downloader.PRDownloaderConfig;
import com.joom.paranoid.Obfuscate;

import org.json.JSONObject;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Objects;

import kotlin.jvm.internal.Ref;
import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;
import ro.alynsampmobile.launcher.utils.FileData;
import ro.alynsampmobile.launcher.utils.Utils;

@Obfuscate
public class UpdateService extends Service {
    public Messenger mMessenger;
    public IncomingHandler mInHandler;
    public Messenger mActivityMessenger;

    public UpdateActivity.UpdateStatus mUpdateStatus = UpdateActivity.UpdateStatus.Undefined;
    public UpdateActivity.GameStatus mGameStatus = UpdateActivity.GameStatus.Undefined;

    public boolean mDownloadingStatus = false;
    public ArrayList<FileData> mUpdateFiles = new ArrayList<>();
    public long mUpdateFilesSizeTotal = 0;
    public int mDownloadFailedOffset;

    public String mUpdateVersion;

    public void onCreate() {
        HandlerThread thread = new HandlerThread("ServiceStartArguments", 10);
        thread.start();
        PRDownloader.initialize(getApplicationContext(), PRDownloaderConfig.newBuilder().setDatabaseEnabled(true).setReadTimeout(30000).setConnectTimeout(30000).build());
        mInHandler = new IncomingHandler(thread.getLooper());
        mMessenger = new Messenger(mInHandler);
    }

    public int onStartCommand(Intent intent, int flags, int startId) {
        return Service.START_STICKY;
    }

    public IBinder onBind(Intent intent) {
        if (mMessenger != null) {
            return mMessenger.getBinder();
        }
        return null;
    }

    public boolean onUnbind(Intent intent) {
        return false;
    }

    public void onRebind(Intent intent) {
    }

    public void onDestroy() {
    }

    private final class IncomingHandler extends Handler {
        public IncomingHandler(Looper looper) {
            super(looper);
        }

        public void handleMessage(Message msg) {
            mActivityMessenger = msg.replyTo;
            // Log.i("UpdateService", "handleMessage -> " + msg.what);
            if (msg.what == 0) checkUpdate(); // check update
            else if (msg.what == 1) updateGameFiles(); // update files
            else if (msg.what == 2) updateGame(); // update game
            else if (msg.what == 4) { // get update status
                Message outMsg = Message.obtain(mInHandler, 4);
                outMsg.getData().putString(NotificationCompat.CATEGORY_STATUS, mUpdateStatus.name());
                outMsg.replyTo = mMessenger;
                if (mActivityMessenger != null) {
                    try {
                        mActivityMessenger.send(outMsg);
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }
                }
            } else if (msg.what == 5) { // get game status
                Message outMsg = Message.obtain(mInHandler, 5);
                outMsg.getData().putString(NotificationCompat.CATEGORY_STATUS, mGameStatus.name());
                outMsg.replyTo = mMessenger;
                if (mActivityMessenger != null) {
                    try {
                        mActivityMessenger.send(outMsg);
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }
                }
            } else if (msg.what == 7) { // check update
                Log.i("UpdateService", "UPDATE_STATUS_GAME");
                checkUpdate();
            } else if (msg.what == 8) { // check update
                checkUpdate();
            }
        }
    }

    public void checkUpdate() {
        Log.d("UpdateService", "checkUpdate()");
        setUpdateStatus(UpdateActivity.UpdateStatus.CheckUpdate);

        try {
            OkHttpClient client = new OkHttpClient();
            Request request = new Request.Builder()
                    .url(Utils.update)
                    .build();

            client.newCall(request).enqueue(new Callback() {
                @Override
                public void onResponse(@NonNull Call call, @NonNull Response response) {
                    try {
                        assert response.body() != null;
                        String responseBody = response.body().string();
                        JSONObject data = new JSONObject(responseBody);

                        mUpdateVersion = data.getString("game_version");
                        Log.i("UpdateService", "mUpdateVersion = " + mUpdateVersion);

                        mUpdateFiles = new ArrayList<>();

                        mGameStatus = UpdateActivity.GameStatus.Undefined;
                        Message outMsg = Message.obtain(mInHandler, 10);
                        outMsg.replyTo = mMessenger;
                        if (mActivityMessenger != null) {
                            try {
                                mActivityMessenger.send(outMsg);
                            } catch (RemoteException e) {
                                e.printStackTrace();
                            }
                        }

                        String data_url = getSharedPreferences("samp_settings", Context.MODE_PRIVATE).getString("files_type", "none").equals("full") ?
                                data.getString("full_list_url") : data.getString("lite_list_url");
                        String samp_data_url = data.getString("samp_list_url");

                        checkGameFilesUpdate(data_url, samp_data_url);

                        if (!isGamePackageExists()) {
                            mGameStatus = UpdateActivity.GameStatus.GameUpdateRequired;
                        } else if (isGameUpdateExists() && !BuildConfig.DEBUG) {
                            mGameStatus = UpdateActivity.GameStatus.GameUpdateRequired;
                        } else if (isGameFilesUpdateExists()) {
                            mGameStatus = UpdateActivity.GameStatus.GameFilesUpdateRequired;
                        } else {
                            mGameStatus = UpdateActivity.GameStatus.Updated;
                        }

                        setUpdateStatus(UpdateActivity.UpdateStatus.Undefined);
                    } catch (Exception e) {
                        Log.e("UpdateService", Objects.requireNonNull(e.getMessage()));
                        mGameStatus = UpdateActivity.GameStatus.Undefined;
                        Message outMsg = Message.obtain(mInHandler, 5);
                        outMsg.getData().putString(NotificationCompat.CATEGORY_STATUS, mGameStatus.name());
                        outMsg.replyTo = mMessenger;
                        if (mActivityMessenger != null) {
                            try {
                                mActivityMessenger.send(outMsg);
                            } catch (RemoteException ee) {
                                ee.printStackTrace();
                            }
                        }
                    }
                }

                @Override
                public void onFailure(@NonNull Call call, @NonNull IOException e) {
                    mGameStatus = UpdateActivity.GameStatus.Undefined;
                    Message outMsg = Message.obtain(mInHandler, 5);
                    outMsg.getData().putString(NotificationCompat.CATEGORY_STATUS, mGameStatus.name());
                    outMsg.replyTo = mMessenger;
                    if (mActivityMessenger != null) {
                        try {
                            mActivityMessenger.send(outMsg);
                        } catch (RemoteException ex) {
                            ex.printStackTrace();
                        }
                    }
                }
            });
        } catch (Exception exception) {
            exception.printStackTrace();
        }
    }

    public boolean isGameUpdateExists() {
        PackageInfo packageInfo;
        Log.i("UpdateService", "isGameUpdateExists");
        PackageManager packageManager = getPackageManager();
        String currentVersion = null;
        if (packageManager != null) {
            try {
                packageInfo = packageManager.getPackageInfo(getPackageName(), PackageManager.GET_ACTIVITIES);
            } catch (PackageManager.NameNotFoundException e) {
                return true;
            }
        } else {
            packageInfo = null;
        }
        if (packageInfo != null) {
            currentVersion = packageInfo.versionName;
        }
        String sb = "isGameUpdateExists -> currentVersion " + currentVersion + " | mUpdateVersion " + mUpdateVersion;
        Log.d("UpdateService", sb);
        return (currentVersion == null || !currentVersion.equals(mUpdateVersion));
    }

    public boolean isGamePackageExists() {
        try {
            getPackageManager().getPackageInfo(getPackageName(), PackageManager.GET_META_DATA);
        } catch (PackageManager.NameNotFoundException e) {
            return false;
        }
        return true;
    }

    public void updateGame() {
        setUpdateStatus(UpdateActivity.UpdateStatus.Undefined);
        Message finishMsg = Message.obtain(mInHandler, 2);
        finishMsg.getData().putBoolean(NotificationCompat.CATEGORY_STATUS, true);
        finishMsg.replyTo = mMessenger;
        Messenger messenger = mActivityMessenger;
        if (messenger != null) {
            try {
                messenger.send(finishMsg);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
        setUpdateStatus(UpdateActivity.UpdateStatus.Undefined);
    }

    public void checkGameFilesUpdate(String list_url, String samp_list_url) throws Exception {
        Log.d("UpdateService", "checkGameFilesUpdate");

        mUpdateFilesSizeTotal = 0;

        String data_str = Utils.getStringOutputByURL(list_url);
        JSONObject data_json = new JSONObject(data_str);
        ArrayList<FileData> dataList = FileData.getListByJson(data_json);

        if (!samp_list_url.isEmpty()) {
            Log.d("UpdateService", "checkGameFilesUpdate -> samp_list_url");
            ArrayList<FileData> sampDataList = FileData.getListByJson(new JSONObject(Utils.getStringOutputByURL(samp_list_url)));
            dataList.addAll(sampDataList);
        } else {
            Log.d("UpdateService", "checkGameFilesUpdate -> samp_list_url is empty");
        }

        for (FileData fileData : dataList) {
            File forCheck = new File(getExternalFilesDir(null), fileData.getPath());

            boolean modifyFiles = getSharedPreferences("samp_settings", Context.MODE_PRIVATE).getBoolean("modify_files", false);

            // skip checking file size if not using modify_files
            if (modifyFiles ? forCheck.exists() : (forCheck.exists() && forCheck.length() == fileData.getSize())) {
                continue; // The file exists and has the correct size; no need to update.
            }

            if (!fileData.getGpu().equals("all")) {
                if ((fileData.getGpu().equals("dxt") && Utils.GPU_TYPE != Utils.GPUType.DXT) ||
                        (fileData.getGpu().equals("pvr") && Utils.GPU_TYPE != Utils.GPUType.PVR) ||
                        (fileData.getGpu().equals("etc") && Utils.GPU_TYPE != Utils.GPUType.ETC)) {
                    continue; // GPU type doesn't match; skip this file.
                }
            }

            System.out.println("Missing/Corrupted file: " + fileData.getPath() + " | " + fileData.getSize() + " bytes");
            System.out.println("File: " + forCheck.getAbsolutePath() + " | " + (forCheck.exists() ? forCheck.length() + " bytes" : "missing"));

            mUpdateFiles.add(fileData);
            mUpdateFilesSizeTotal += fileData.getSize();
        }
    }

    public void setUpdateStatus(UpdateActivity.UpdateStatus status) {
        if (!(status.name().isEmpty()) && mUpdateStatus != status) {
            mUpdateStatus = status;
            Message outMsg = Message.obtain(mInHandler, 4);
            outMsg.getData().putString(NotificationCompat.CATEGORY_STATUS, mUpdateStatus.name());
            outMsg.replyTo = mMessenger;
            Messenger messenger = mActivityMessenger;
            if (messenger != null) {
                try {
                    messenger.send(outMsg);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public void updateGameFiles() {
        if (isGameFilesUpdateExists()) {
            setUpdateStatus(UpdateActivity.UpdateStatus.DownloadGameFiles);
            downloadGameFiles();
            return;
        }
        Log.d("UpdateService", "updateGameFiles");
        Message outMsg = Message.obtain(mInHandler, 1);
        outMsg.getData().putBoolean(NotificationCompat.CATEGORY_STATUS, true);
        outMsg.replyTo = mMessenger;
        if (mActivityMessenger != null) {
            try {
                mActivityMessenger.send(outMsg);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
    }

    private void downloadGameFiles() {
        Log.i("UpdateService", "Download Game Files");
        mDownloadFailedOffset = 0;
        final ArrayList<FileData> tempUpdateFiles = new ArrayList<>(mUpdateFiles);
        mUpdateFiles.clear();
        final Ref.IntRef i = new Ref.IntRef();
        final Ref.LongRef mUpdateFilesSizeCurrent = new Ref.LongRef();
        mUpdateFilesSizeCurrent.element = 0;

        sendLoadingScreen(false, "", 0, 0);
        for (i.element = 0; i.element < tempUpdateFiles.size(); i.element++) {
            mDownloadingStatus = true;
            final FileData fileData = tempUpdateFiles.get(i.element);

            Log.i("UpdateService", i.element + " | filePath = " + fileData.getPath());
            Log.i("UpdateService", "Request uri = " + fileData.getUrl());

            File dir = new File(getExternalFilesDir(null), fileData.getPath()).getParentFile();
            if (!dir.exists()) dir.mkdirs();

            final File file = new File(getExternalFilesDir(null), fileData.getPath());
            if (file.exists()) file.delete();

            final Ref.LongRef mLastDownloadedBytesTime = new Ref.LongRef();
            mLastDownloadedBytesTime.element = System.currentTimeMillis();
            mDownloadingStatus = true;
            PRDownloader.download(fileData.getUrl(), dir.toString(), fileData.getName()).build().
                    setOnStartOrResumeListener(null).
                    setOnPauseListener(null).
                    setOnCancelListener(null).
                    setOnProgressListener(progress -> {
                        mDownloadingStatus = true;
                        if (System.currentTimeMillis() - mLastDownloadedBytesTime.element > ((long) 100)) {
                            mLastDownloadedBytesTime.element = System.currentTimeMillis();
                            Message outMsg = Message.obtain(mInHandler, 4);
                            outMsg.getData().putString(NotificationCompat.CATEGORY_STATUS, UpdateActivity.UpdateStatus.DownloadGameFiles.name());
                            outMsg.getData().putBoolean("withProgress", true);
                            outMsg.getData().putLong("current", (mUpdateFilesSizeCurrent.element + progress.currentBytes));
                            outMsg.getData().putLong("total", mUpdateFilesSizeTotal);
                            outMsg.getData().putString("filename", fileData.getName());
                            outMsg.getData().putLong("totalfiles", ((long) tempUpdateFiles.size()) - ((long) mDownloadFailedOffset));
                            outMsg.getData().putLong("currentfile", ((long) i.element) - ((long) mDownloadFailedOffset));
                            outMsg.replyTo = mMessenger;
                            if (mActivityMessenger != null) {
                                try {
                                    mActivityMessenger.send(outMsg);
                                } catch (RemoteException e) {
                                    e.printStackTrace();
                                }
                            }
                        }
                    }).
                    start(new OnDownloadListener() {
                        @Override
                        public void onDownloadComplete() {
                            Log.d("UpdateService", "onDownloadComplete");
                            mDownloadingStatus = false;
                            mUpdateFilesSizeCurrent.element += fileData.getSize();
                        }

                        @Override
                        public void onError(Error error) {
                            mDownloadingStatus = false;
                            mUpdateFiles.add(tempUpdateFiles.get(i.element));
                            mDownloadFailedOffset += 1;
                            mUpdateFilesSizeTotal -= fileData.getSize();
                            Log.d("UpdateService", "onError. ServerError = " + (error != null ? error.isServerError() : null) + ". ConnectionError = " + (error != null ? error.isConnectionError() : null));
                        }
                    });
            do {
                try {
                    Thread.sleep(30);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            } while (mDownloadingStatus);
        }
        mDownloadingStatus = false;
        sendLoadingScreen(false, "", 0, 0);

        updateGame();
    }

    private void sendLoadingScreen(final boolean unpacking, final String fileName, final long current, final long total) {
        new Thread(() -> {
            Message outMsg = Message.obtain(mInHandler, 4);
            outMsg.getData().putString(NotificationCompat.CATEGORY_STATUS, UpdateActivity.UpdateStatus.CheckUpdate.name());
            outMsg.getData().putBoolean("withProgress", true);
            outMsg.getData().putString("filename", fileName);
            outMsg.getData().putBoolean("unpacking", unpacking);
            outMsg.getData().putLong("current", current);
            outMsg.getData().putLong("total", total);
            outMsg.replyTo = mMessenger;
            if (mActivityMessenger != null) {
                try {
                    mActivityMessenger.send(outMsg);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    public boolean isGameFilesUpdateExists() {
        Log.i("UpdateService", "isGameFilesUpdateExists");
        return !mUpdateFiles.isEmpty();
    }
}
