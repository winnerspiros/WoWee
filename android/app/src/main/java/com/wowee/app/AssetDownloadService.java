package com.wowee.app;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;

import java.io.File;

/**
 * Foreground service that downloads WoW game assets on first launch.
 *
 * Downloads from a CDN/manifest URL. The URL can be configured via intent extra, or
 * falls back to a hardcoded CDN. After download, extract assets and verify checksums.
 *
 * Communicates progress back to WoWeeActivity via a broadcast or bound interface.
 */
public class AssetDownloadService extends Service {

    private static final String TAG = "AssetDownloadService";
    private static final int NOTIFICATION_ID = 1;
    private static final String CHANNEL_ID = "asset_download";

    private AssetDownloader mDownloader;
    private Handler mMainHandler;

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "Service created");
        mMainHandler = new Handler(Looper.getMainLooper());
        createNotificationChannel();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(TAG, "Service started");

        String dataDir = intent != null ? intent.getStringExtra("data_dir") : null;
        String externalDir = intent != null ? intent.getStringExtra("external_dir") : null;
        String manifestUrl = intent != null ? intent.getStringExtra("manifest_url") : null;

        if (dataDir == null) {
            Log.e(TAG, "No data_dir provided");
            stopSelf();
            return START_NOT_STICKY;
        }

        // Start foreground with progress notification
        startForeground(NOTIFICATION_ID, buildNotification("Preparing download...", 0));

        // Start download
        mDownloader = new AssetDownloader(dataDir, manifestUrl);
        mDownloader.setCallback(new AssetDownloader.Callback() {
            @Override
            public void onProgress(String status, int percent) {
                updateNotification(status, percent);
            }

            @Override
            public void onComplete(File dataDir) {
                Log.i(TAG, "Asset download complete: " + dataDir.getAbsolutePath());
                stopForeground(STOP_FOREGROUND_REMOVE);

                // Notify the Activity
                Intent broadcast = new Intent("com.wowee.app.ASSETS_READY");
                sendBroadcast(broadcast);

                stopSelf();
            }

            @Override
            public void onError(String message) {
                Log.e(TAG, "Asset download failed: " + message);
                Notification errorNotif = new NotificationCompat.Builder(AssetDownloadService.this, CHANNEL_ID)
                        .setContentTitle("Asset Download Failed")
                        .setContentText(message)
                        .setSmallIcon(android.R.drawable.stat_sys_warning)
                        .setPriority(NotificationCompat.PRIORITY_HIGH)
                        .build();
                startForeground(NOTIFICATION_ID, errorNotif);
                stopSelf();
            }
        });

        mDownloader.start();
        return START_NOT_STICKY;
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null; // Not a bound service
    }

    @Override
    public void onDestroy() {
        Log.i(TAG, "Service destroyed");
        if (mDownloader != null) {
            mDownloader.cancel();
        }
        super.onDestroy();
    }

    // --- Notification ---

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(
                    CHANNEL_ID,
                    "Asset Download",
                    NotificationManager.IMPORTANCE_LOW
            );
            channel.setDescription("Downloading game assets");
            NotificationManager nm = getSystemService(NotificationManager.class);
            if (nm != null) {
                nm.createNotificationChannel(channel);
            }
        }
    }

    private Notification buildNotification(String status, int progress) {
        Intent intent = new Intent(this, WoWeeActivity.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(
                this, 0, intent, PendingIntent.FLAG_IMMUTABLE);

        return new NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle("WoWee Asset Download")
                .setContentText(status)
                .setSmallIcon(android.R.drawable.stat_sys_download)
                .setProgress(100, progress, progress < 0)
                .setOngoing(true)
                .setContentIntent(pendingIntent)
                .setPriority(NotificationCompat.PRIORITY_LOW)
                .build();
    }

    private void updateNotification(String status, int progress) {
        NotificationManager nm = getSystemService(NotificationManager.class);
        if (nm != null) {
            nm.notify(NOTIFICATION_ID, buildNotification(status, progress));
        }
    }
}