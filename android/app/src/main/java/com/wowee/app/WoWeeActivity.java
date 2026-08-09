package com.wowee.app;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import org.libsdl.app.SDL;

import java.io.File;

/**
 * WoWee Native Client — Android Activity.
 *
 * Flow:
 *   1. Check assets → if missing, launch AssetDownloadService
 *   2. Once assets ready: SDL.setupJNI() → SDL.initialize() → SDL.setSurface() → nativeInit()
 *   3. Game loop runs on dedicated native thread; UI thread dispatches SDL events
 */
public class WoWeeActivity extends AppCompatActivity implements SurfaceHolder.Callback {

    private static final String TAG = "WoWeeActivity";
    private static final int PERMISSION_REQUEST_CODE = 1001;
    private static final String BROADCAST_ASSETS_READY = "com.wowee.app.ASSETS_READY";
    private static final String WOW_DATA_DIR = "Data";

    private enum State { CHECKING, DOWNLOADING, INIT_SDL, RUNNING }

    private SurfaceView mSurfaceView;
    private Surface mSurface;
    private State mState = State.CHECKING;
    private boolean mSDLReady = false;
    private boolean mSurfaceReady = false;

    // Overlay
    private View mDownloadOverlay;
    private ProgressBar mDownloadProgress;
    private TextView mDownloadStatus;

    // Broadcast receiver for asset download completion
    private final BroadcastReceiver mAssetReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.i(TAG, "Assets ready broadcast received");
            onAssetsReady();
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG, "onCreate");

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        if (getSupportActionBar() != null) getSupportActionBar().hide();

        setContentView(R.layout.activity_main);

        mSurfaceView = findViewById(R.id.surface_view);
        mSurfaceView.getHolder().addCallback(this);

        mDownloadOverlay = findViewById(R.id.download_overlay);
        mDownloadProgress = findViewById(R.id.download_progress);
        mDownloadStatus = findViewById(R.id.download_status);

        // Register for asset download complete broadcasts
        registerReceiver(mAssetReceiver, new IntentFilter(BROADCAST_ASSETS_READY),
                Context.RECEIVER_NOT_EXPORTED);

        if (hasRequiredPermissions()) {
            checkAssets();
        } else {
            requestPermissions();
        }
    }

    // --- Permissions ---

    private boolean hasRequiredPermissions() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
            return ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
                    == PackageManager.PERMISSION_GRANTED;
        }
        return true;
    }

    private void requestPermissions() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
                    PERMISSION_REQUEST_CODE);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == PERMISSION_REQUEST_CODE) checkAssets();
    }

    // --- Asset management ---

    private void checkAssets() {
        File dataDir = getDataDir();
        File manifestFile = new File(dataDir, "manifest.json");

        if (manifestFile.exists() && manifestFile.length() > 0) {
            Log.i(TAG, "Assets found at " + dataDir);
            mState = State.INIT_SDL;
            initSDL();
        } else {
            Log.i(TAG, "Assets missing, starting download...");
            mState = State.DOWNLOADING;
            mDownloadOverlay.setVisibility(View.VISIBLE);
            mSurfaceView.setVisibility(View.GONE);

            Intent intent = new Intent(this, AssetDownloadService.class);
            intent.putExtra("data_dir", dataDir.getAbsolutePath());
            intent.putExtra("external_dir", getExternalDir().getAbsolutePath());
            startService(intent);
        }
    }

    private void onAssetsReady() {
        mState = State.INIT_SDL;
        runOnUiThread(() -> {
            mDownloadOverlay.setVisibility(View.GONE);
            mSurfaceView.setVisibility(View.VISIBLE);
            initSDL();
        });
    }

    private File getDataDir() {
        File ext = getExternalDir();
        return new File(ext, WOW_DATA_DIR);
    }

    private File getExternalDir() {
        File ext = getExternalFilesDir(null);
        return ext != null ? ext : getFilesDir();
    }

    // --- SDL + Native ---

    private void initSDL() {
        if (mSDLReady) return;
        if (mState != State.INIT_SDL) return;

        Log.i(TAG, "SDL.setupJNI() + SDL.initialize()");
        SDL.setupJNI();
        SDL.initialize();
        mSDLReady = true;

        // If surface is already ready, set it and init native now
        if (mSurfaceReady && mSurface != null) {
            SDL.setSurface(mSurface);
            startGame();
        }
    }

    private void startGame() {
        if (mSurface == null || !mSDLReady) return;
        if (mState == State.RUNNING) return;

        Log.i(TAG, "nativeInit → starting game loop on native thread");
        String dataPath = getDataDir().getAbsolutePath();
        String externalPath = getExternalDir().getAbsolutePath();

        mState = State.RUNNING;
        nativeInit(dataPath, externalPath);
    }

    // --- SurfaceHolder.Callback ---

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder) {
        Log.i(TAG, "surfaceCreated");
        mSurface = holder.getSurface();
        mSurfaceReady = true;

        if (mSDLReady && mState == State.INIT_SDL) {
            SDL.setSurface(mSurface);
            startGame();
        }
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {
        Log.i(TAG, "surfaceChanged " + width + "x" + height);
        if (mSurface != null) {
            SDL.onNativeResize(width, height, mSurface);
        }
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {
        Log.i(TAG, "surfaceDestroyed");
        mSurface = null;
        mSurfaceReady = false;
    }

    // --- Lifecycle ---

    @Override
    protected void onResume() {
        super.onResume();
        SDL.onNativeResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        SDL.onNativePause();
    }

    @Override
    protected void onDestroy() {
        Log.i(TAG, "onDestroy");
        unregisterReceiver(mAssetReceiver);

        if (mState == State.RUNNING) {
            nativeShutdown();
        }
        super.onDestroy();
    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            getWindow().getDecorView().setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_FULLSCREEN |
                    View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                    View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY |
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE |
                    View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                    View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            );
        }
    }

    // --- JNI ---

    private static native void nativeInit(String dataPath, String externalPath);
    private static native void nativeShutdown();

    static {
        try {
            System.loadLibrary("wowee");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to load libwowee.so: " + e.getMessage());
        }
    }
}