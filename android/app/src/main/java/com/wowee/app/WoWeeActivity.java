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
import android.view.View;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import org.libsdl.app.SDL;
import org.libsdl.app.SDLSurface;

import java.io.File;

/**
 * WoWee Native Client — Android Activity.
 *
 * Uses SDLSurface (from copied SDL2 Java source) for proper SDL lifecycle.
 * Flow: check assets → download if missing → SDL.setupJNI → SDLSurface → game runs.
 */
public class WoWeeActivity extends AppCompatActivity {

    private static final String TAG = "WoWeeActivity";
    private static final String BROADCAST_ASSETS_READY = "com.wowee.app.ASSETS_READY";
    private static final String WOW_DATA_DIR = "Data";

    private SDLSurface mSDLSurface;
    private boolean mAssetsReady = false;

    // Download overlay
    private View mDownloadOverlay;
    private ProgressBar mDownloadProgress;
    private TextView mDownloadStatus;

    private final BroadcastReceiver mAssetReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
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

        // Inflate layout with download overlay
        setContentView(R.layout.activity_main);

        mDownloadOverlay = findViewById(R.id.download_overlay);
        mDownloadProgress = findViewById(R.id.download_progress);
        mDownloadStatus = findViewById(R.id.download_status);

        registerReceiver(mAssetReceiver, new IntentFilter(BROADCAST_ASSETS_READY),
                Context.RECEIVER_NOT_EXPORTED);

        if (hasRequiredPermissions()) {
            checkAssets();
        } else {
            requestPermissions();
        }
    }

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
                    1001);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 1001) checkAssets();
    }

    // --- Asset management ---

    private void checkAssets() {
        File dataDir = getWowDataDir();
        File manifestFile = new File(dataDir, "manifest.json");

        if (manifestFile.exists() && manifestFile.length() > 0) {
            Log.i(TAG, "Assets found at " + dataDir);
            mAssetsReady = true;
            initSDL();
        } else {
            Log.i(TAG, "Assets missing, starting download...");
            mDownloadOverlay.setVisibility(View.VISIBLE);

            Intent intent = new Intent(this, AssetDownloadService.class);
            intent.putExtra("data_dir", dataDir.getAbsolutePath());
            intent.putExtra("external_dir", getWowExternalDir().getAbsolutePath());
            startService(intent);
        }
    }

    private void onAssetsReady() {
        runOnUiThread(() -> {
            mAssetsReady = true;
            mDownloadOverlay.setVisibility(View.GONE);
            initSDL();
        });
    }

    private File getWowDataDir() {
        return new File(getWowExternalDir(), WOW_DATA_DIR);
    }

    private File getWowExternalDir() {
        File ext = getExternalFilesDir(null);
        return ext != null ? ext : getFilesDir();
    }

    // --- SDL2 + Game ---

    private void initSDL() {
        if (!mAssetsReady) return;
        if (mSDLSurface != null) return;

        Log.i(TAG, "Initializing SDL2...");
        SDL.setupJNI();
        SDL.initialize();

        // Set data path env for native code
        String dataPath = getWowDataDir().getAbsolutePath();
        String externalPath = getWowExternalDir().getAbsolutePath();
        SDL.setenv("WOW_DATA_PATH", dataPath);
        SDL.setenv("WOWEE_EXTERNAL_PATH", externalPath);
        SDL.setenv("WOWEE_ANDROID", "1");
        SDL.setenv("HOME", externalPath);

        // Create SDLSurface — SDL handles Surface + lifecycle + events
        mSDLSurface = new SDLSurface(this);
        mSDLSurface.setLayoutParams(new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT));

        // Replace download overlay with SDLSurface
        FrameLayout root = findViewById(R.id.surface_view);
        if (root != null) {
            root.addView(mSDLSurface);
        } else {
            setContentView(mSDLSurface);
        }
    }

    // --- Lifecycle ---

    @Override
    protected void onDestroy() {
        Log.i(TAG, "onDestroy");
        unregisterReceiver(mAssetReceiver);
        if (mSDLSurface != null) {
            mSDLSurface = null;
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

    // --- JNI: native methods loaded by SDL ---

    static {
        try {
            System.loadLibrary("wowee");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Failed to load libwowee.so: " + e.getMessage());
        }
    }
}