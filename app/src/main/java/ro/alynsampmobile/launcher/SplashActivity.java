package ro.alynsampmobile.launcher;

import android.Manifest;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.net.Uri;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Messenger;
import android.os.RemoteException;
import android.os.StrictMode;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import com.applovin.sdk.AppLovinSdk;
import com.google.android.material.button.MaterialButton;
import com.google.android.material.textview.MaterialTextView;
import com.joom.paranoid.Obfuscate;

import java.util.List;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import pub.devrel.easypermissions.EasyPermissions;
import ro.alynsampmobile.launcher.utils.Logcat;
import ro.alynsampmobile.launcher.utils.Utils;

@Obfuscate
public class SplashActivity extends AppCompatActivity implements GLSurfaceView.Renderer, EasyPermissions.PermissionCallbacks {
    private static final int PERMISSION_REQUEST_CODE = 1337;
    private static String[] permissions = {
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.RECORD_AUDIO
    };

    public IncomingHandler mInHandler;
    public Messenger mMessenger;
    public Messenger mService;
    GLSurfaceView glSurfaceView;

    Logcat logcat;

    private final ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(@NonNull ComponentName componentName, @NonNull IBinder service) {
            mService = new Messenger(service);
            checkUpdate();
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mService = null;
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        StrictMode.setThreadPolicy(new StrictMode.ThreadPolicy.Builder().permitAll().build());

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);

        getExternalFilesDir(null);
        getObbDir();
        getExternalMediaDirs();

        logcat = new Logcat(this);
        logcat.start();

        try {
            if (!Utils.isTester(this)) {
                AppLovinSdk.getInstance(this).setMediationProvider("max");
                AppLovinSdk.initializeSdk(this, configuration -> Log.i("AppLovinSdk", "initialized!"));
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

/*        ((MaterialTextView) findViewById(R.id.ahahaha)).setText(Utils.copyright);*/

        mInHandler = new IncomingHandler();
        mMessenger = new Messenger(mInHandler);

        glSurfaceView = findViewById(R.id.surface);
        glSurfaceView.setRenderer(this);

        Toast.makeText(this, "Alyn SA-MP Mobile v" + BuildConfig.VERSION_NAME, Toast.LENGTH_SHORT).show();

        if (EasyPermissions.hasPermissions(this, permissions)) {
            startApp();
        } else {
            EasyPermissions.requestPermissions(this, "Allow app permisions", PERMISSION_REQUEST_CODE, permissions);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mService != null) {
            unbindService(mConnection);
        }
        dismissDialog();
        //logcat.stop();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        EasyPermissions.onRequestPermissionsResult(requestCode, permissions, grantResults, this);
    }

    @Override
    public void onPermissionsGranted(int requestCode, @NonNull List<String> list) {
        if (requestCode == PERMISSION_REQUEST_CODE) {
            startApp();
        }
    }

    @Override
    public void onPermissionsDenied(int requestCode, @NonNull List<String> list) {
        if (requestCode == PERMISSION_REQUEST_CODE) {
            // Toast.makeText(this, "App permissions not granted! (try to give from app settings)", Toast.LENGTH_LONG).show();
            startApp();
        }
    }

    private AlertDialog filesSelectionDialog;

    private void startApp() {
        if (!Utils.isOnline(this)) {
            // dialog
            new AlertDialog.Builder(this)
                    .setTitle("No internet connection!")
                    .setMessage("Please connect to the internet and try again.")
                    .setPositiveButton("Retry", (dialog, which) -> {
                        dialog.dismiss();
                        startApp();
                    }).setNegativeButton("Offline Mode", (dialog, which) -> {
                        AlertDialog alertDialog = new AlertDialog.Builder(this)
                                .setTitle(R.string.offline_mode)
                                .setMessage("Are you sure you want to start the game in offline mode?")
                                .setPositiveButton("Yes", (dialog2, which2) -> {
                                    getSharedPreferences("samp_settings", Context.MODE_PRIVATE).edit().putBoolean("offline_mode", true).apply();
                                    // start game
                                    try {
                                        Intent intent = new Intent(this, ro.alynsampmobile.game.SAMP.class);
                                        intent.putExtra("extra_check", "alynsampmobile1337");
                                        startActivity(intent);
                                    } catch (Exception e) {
                                        e.printStackTrace();
                                    }
                                })
                                .setNegativeButton("No", null)
                                .create();
                        alertDialog.show();
                    }).setCancelable(false).show();
            return;
        }

        if (!getSharedPreferences("samp_settings", Context.MODE_PRIVATE).getString("files_type", "none").equals("lite") &&
                !getSharedPreferences("samp_settings", Context.MODE_PRIVATE).getString("files_type", "none").equals("full")) {
            View view = getLayoutInflater().inflate(R.layout.layout_files_selection, null);

            AlertDialog.Builder dialogBuilder = new AlertDialog.Builder(this).setView(view);
            filesSelectionDialog = dialogBuilder.create();

            MaterialButton lite = view.findViewById(R.id.lite_button);
            MaterialButton full = view.findViewById(R.id.full_button);

            lite.setOnClickListener(v -> {
                getSharedPreferences("samp_settings", Context.MODE_PRIVATE).edit().putString("files_type", "lite").apply();
                dismissDialog();
                new Handler().postDelayed(this::startMain, 2500);
            });

            full.setOnClickListener(v -> {
                getSharedPreferences("samp_settings", Context.MODE_PRIVATE).edit().putString("files_type", "full").apply();
                dismissDialog();
                new Handler().postDelayed(this::startMain, 2500);
            });

            filesSelectionDialog.show();
        } else {
            new Handler().postDelayed(this::startMain, 2500);
        }
    }

    private void dismissDialog() {
        if (filesSelectionDialog != null && filesSelectionDialog.isShowing()) {
            filesSelectionDialog.dismiss();
            filesSelectionDialog = null;
        }
    }

    private void startMain() {
        if (!isFinishing()) {
            try {
                Intent intent = new Intent(SplashActivity.this, MainActivity.class);
                intent.putExtra("extra_check", "alynsampmobile1337");
                startActivity(intent);
                finish();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
        String renderer = gl10.glGetString(gl10.GL_RENDERER);
        Log.i("SplashActivity", "Renderer: " + renderer);

        String extensions = gl10.glGetString(gl10.GL_EXTENSIONS);
        Log.i("SplashActivity", "Extensions: " + extensions);

        Utils.GPUType type;

        if (extensions.contains("GL_IMG_texture_compression_pvrtc")) {
            type = Utils.GPUType.PVR;
        } else if (extensions.contains("GL_EXT_texture_compression_dxt1") || extensions.contains("GL_EXT_texture_compression_s3tc") || extensions.contains("GL_AMD_compressed_ATC_texture")) {
            type = Utils.GPUType.DXT;
        } else {
            type = Utils.GPUType.ETC;
        }

        /*if (extensions.contains("pvr")) {
            type = Utils.GPUType.PVR;
        } else if (extensions.contains("dxt")) {
            type = Utils.GPUType.DXT;
        } else {
            type = Utils.GPUType.ETC;
        }*/

        Utils.GPU_TYPE = type;
        Log.i("SplashActivity", "GPU_TYPE: " + Utils.GPU_TYPE.name());

        int error = gl10.glGetError();
        if (error != gl10.GL_NO_ERROR) {
            Log.e("SplashActivity", "OpenGL error: " + error);
        }

        runOnUiThread(() -> glSurfaceView.setVisibility(View.GONE));
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int i, int i1) {

    }

    @Override
    public void onDrawFrame(GL10 gl10) {

    }

    public void checkUpdate() {
        // Log.i("SplashActivity", "checkUpdate");

        Message obtain = Message.obtain(null, 0);
        obtain.replyTo = mMessenger;

        try {
            mService.send(obtain);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    private class IncomingHandler extends Handler {
        @Override
        public void handleMessage(Message message) {
            int i = message.what;
            if (i == 4) {
                UpdateActivity.UpdateStatus updateStatus = UpdateActivity.UpdateStatus.valueOf(message.getData().getString("status", ""));
                if (updateStatus == UpdateActivity.UpdateStatus.Undefined) {
                    Message obtain = Message.obtain(null, 5);
                    obtain.replyTo = mMessenger;
                    Messenger messenger = mService;
                    try {
                        messenger.send(obtain);
                    } catch (RemoteException e) {
                        e.printStackTrace();
                    }
                } else if (updateStatus == UpdateActivity.UpdateStatus.CheckUpdate) {
                    Message obtain2 = Message.obtain(null, 4);
                    obtain2.replyTo = mMessenger;
                    Messenger messenger = mService;
                    try {
                        messenger.send(obtain2);
                    } catch (RemoteException e2) {
                        e2.printStackTrace();
                    }
                }
            } else if (i == 5) {
                UpdateActivity.GameStatus gameStatus = UpdateActivity.GameStatus.valueOf(message.getData().getString("status", ""));
                Log.i("SplashActivity", "gameStatus = " + gameStatus);

                if (!Utils.appStatus()) {
                    Utils.showAppStatusWarning(SplashActivity.this);
                } else {
                    if (gameStatus == UpdateActivity.GameStatus.GameUpdateRequired && !Utils.isTester(SplashActivity.this)) {
                        try {
                            new AlertDialog.Builder(SplashActivity.this)
                                    .setTitle("Update:").setMessage("Game update required! Press 'Update' and download the latest version from the website.")
                                    .setPositiveButton("Update", (dialog, which) -> {
                                        /*Intent intent = new Intent(SplashActivity.this, UpdateActivity.class);
                                        intent.putExtra("mode", UpdateActivity.UpdateMode.GameUpdate.name());
                                        startActivity(intent);
                                        finish();*/
                                        Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(Utils.web));
                                        startActivity(intent);
                                    }).setCancelable(false).show();
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    } else if (gameStatus == UpdateActivity.GameStatus.GameFilesUpdateRequired) {
                        try {
                            new AlertDialog.Builder(SplashActivity.this)
                                    .setTitle("Update:").setMessage("Game files update required!")
                                    .setPositiveButton("Update", (dialog, which) -> {
                                        Intent intent = new Intent(SplashActivity.this, UpdateActivity.class);
                                        intent.putExtra("mode", UpdateActivity.UpdateMode.GameUpdate.name());
                                        startActivity(intent);
                                        finish();
                                    }).setNegativeButton("No", (dialog, which) -> {
                                        startMain();
                                    }).setCancelable(false).show();
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    } else {
                        startMain();
                    }
                }
            }
        }
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        quitApp();
    }

    private long quit_time = 0;

    public void quitApp() {
        if ((System.currentTimeMillis() - quit_time) > 2000) {
            Toast.makeText(this, "Press again to quit.", Toast.LENGTH_LONG).show();
            quit_time = System.currentTimeMillis();
        } else {
            finish();
        }
    }
}
