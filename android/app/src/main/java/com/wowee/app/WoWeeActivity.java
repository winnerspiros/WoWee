package com.wowee.app;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import org.libsdl.app.SDLActivity;

import java.io.File;

/**
 * WoWee Android Activity — extends SDLActivity for proper SDL2 lifecycle.
 *
 * Flow:
 *   1. onCreate → check assets
 *   2. If missing → show download overlay, start AssetDownloadService
 *   3. On assets ready → call super.onCreate() → SDL loads libwowee.so → SDL_main()
 */
public class WoWeeActivity extends SDLActivity {

    private static final String TAG = "WoWeeActivity";
    private static final String BROADCAST_ASSETS_READY = "com.wowee.app.ASSETS_READY";
    private static final String WOW_DATA_DIR = "Data";

    private boolean mAssetsChecked = false;
    private boolean mAssetsReady = false;

    private final BroadcastReceiver mAssetReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            mAssetsReady = true;
            if (mDownloadOverlay != null) mDownloadOverlay.setVisibility(View.GONE);
            initSDL();
        }
    };

    private View mDownloadOverlay;

    @Override
    protected String[] getLibraries() {
        return new String[] { "wowee" };
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(TAG, "onCreate");

        super.setContentView(R.layout.activity_main);
        mDownloadOverlay = findViewById(R.id.download_overlay);

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
            return ContextCompat.checkSelfPermission(this,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE)
                    == PackageManager.PERMISSION_GRANTED;
        }
        return true;
    }

    private void requestPermissions() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1001);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        if (requestCode == 1001) checkAssets();
    }

    private void checkAssets() {
        if (mAssetsChecked) return;
        mAssetsChecked = true;

        File dataDir = getWowDataDir();
        File manifestFile = new File(dataDir, "manifest.json");

        if (manifestFile.exists() && manifestFile.length() > 0) {
            mAssetsReady = true;
            if (mDownloadOverlay != null) mDownloadOverlay.setVisibility(View.GONE);
            initSDL();
        } else {
            Log.i(TAG, "Assets missing, starting download...");
            if (mDownloadOverlay != null) mDownloadOverlay.setVisibility(View.VISIBLE);

            Intent intent = new Intent(this, AssetDownloadService.class);
            intent.putExtra("data_dir", dataDir.getAbsolutePath());
            intent.putExtra("external_dir", getWowExternalDir().getAbsolutePath());
            startService(intent);
        }
    }

    private void initSDL() {
        // SDL already started by super.onCreate() — env vars set before it.
        // This is called when assets are confirmed ready to hide the overlay.
        if (mDownloadOverlay != null) mDownloadOverlay.setVisibility(View.GONE);
    }

    private File getWowDataDir() {
        return new File(getWowExternalDir(), WOW_DATA_DIR);
    }

    private File getWowExternalDir() {
        File ext = getExternalFilesDir(null);
        return ext != null ? ext : getFilesDir();
    }

    @Override
    protected void onDestroy() {
        try { unregisterReceiver(mAssetReceiver); } catch (Exception ignored) {}
        super.onDestroy();
    }
}