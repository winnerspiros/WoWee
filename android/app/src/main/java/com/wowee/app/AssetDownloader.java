package com.wowee.app;

import android.util.Log;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

/**
 * Downloads and extracts WoW game assets from a CDN/manifest.
 *
 * Protocol:
 *   1. Fetch manifest.json from CDN
 *   2. For each asset bundle: download, verify SHA256, extract to Data/
 *   3. On completion: write local manifest.json with checksums
 */
public class AssetDownloader {

    private static final String TAG = "AssetDownloader";

    // Default CDN manifest URL — override via constructor
    private static final String DEFAULT_MANIFEST_URL =
            "https://wowee-assets.example.com/manifest.json";

    // Buffer size for downloads
    private static final int BUFFER_SIZE = 128 * 1024; // 128 KB

    private final File mDataDir;
    private final String mManifestUrl;
    private final ExecutorService mExecutor = Executors.newSingleThreadExecutor();
    private volatile boolean mCancelled = false;

    private Callback mCallback;

    public interface Callback {
        void onProgress(String status, int percent);
        void onComplete(File dataDir);
        void onError(String message);
    }

    public AssetDownloader(String dataDirPath, String manifestUrl) {
        mDataDir = new File(dataDirPath);
        mManifestUrl = (manifestUrl != null && !manifestUrl.isEmpty()) ? manifestUrl : DEFAULT_MANIFEST_URL;
        if (!mDataDir.exists()) {
            mDataDir.mkdirs();
        }
    }

    public void setCallback(Callback callback) {
        mCallback = callback;
    }

    public void start() {
        mExecutor.execute(this::downloadAssets);
    }

    public void cancel() {
        mCancelled = true;
        mExecutor.shutdownNow();
    }

    private void reportProgress(String status, int percent) {
        if (mCallback != null) {
            mCallback.onProgress(status, percent);
        }
    }

    private void reportError(String message) {
        if (mCallback != null) {
            mCallback.onError(message);
        }
    }

    private void reportComplete() {
        if (mCallback != null) {
            mCallback.onComplete(mDataDir);
        }
    }

    // --- Download logic ---

    private void downloadAssets() {
        try {
            // 1. Fetch manifest
            reportProgress("Fetching manifest...", 5);
            JSONObject manifest = fetchManifest();
            if (manifest == null) {
                reportError("Failed to fetch asset manifest");
                return;
            }

            // 2. Parse bundle list
            JSONArray bundles = manifest.optJSONArray("bundles");
            if (bundles == null || bundles.length() == 0) {
                reportError("Manifest contains no asset bundles");
                return;
            }

            long totalSize = 0;
            List<BundleInfo> bundleList = new ArrayList<>();
            for (int i = 0; i < bundles.length(); i++) {
                JSONObject bundle = bundles.getJSONObject(i);
                BundleInfo info = new BundleInfo(
                        bundle.getString("name"),
                        bundle.getString("url"),
                        bundle.optString("sha256", ""),
                        bundle.optLong("size", 0)
                );
                bundleList.add(info);
                totalSize += info.size;
            }

            Log.i(TAG, "Downloading " + bundleList.size() + " bundles (" +
                    (totalSize / (1024 * 1024)) + " MB)");

            // 3. Download each bundle
            long downloaded = 0;
            for (int i = 0; i < bundleList.size(); i++) {
                if (mCancelled) return;

                BundleInfo bundle = bundleList.get(i);
                File tempFile = new File(mDataDir, bundle.name + ".tmp");
                File zipFile = new File(mDataDir, bundle.name + ".zip");

                int basePercent = 10 + (80 * i / bundleList.size());

                boolean ok = downloadFile(bundle.url, tempFile, bundle.sha256,
                        basePercent, 80 / bundleList.size());

                if (!ok) {
                    reportError("Failed to download: " + bundle.name);
                    return;
                }

                // Rename .tmp to .zip
                tempFile.renameTo(zipFile);

                // Extract
                reportProgress("Extracting " + bundle.name + "...", basePercent + (70 / bundleList.size()));
                if (!extractZip(zipFile, mDataDir)) {
                    reportError("Failed to extract: " + bundle.name);
                    return;
                }

                // Clean up zip
                zipFile.delete();

                downloaded += bundle.size;
            }

            // 4. Write local manifest
            writeLocalManifest(manifest);

            reportProgress("Complete!", 100);
            reportComplete();

        } catch (Exception e) {
            Log.e(TAG, "Download failed", e);
            reportError("Download error: " + e.getMessage());
        }
    }

    private JSONObject fetchManifest() throws IOException {
        URL url = new URL(mManifestUrl);
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setConnectTimeout(15000);
        conn.setReadTimeout(30000);
        conn.setRequestMethod("GET");
        conn.setRequestProperty("User-Agent", "WoWee-Android/1.0");

        try {
            int responseCode = conn.getResponseCode();
            if (responseCode != 200) {
                Log.e(TAG, "Manifest fetch failed: HTTP " + responseCode);
                return null;
            }

            InputStream is = conn.getInputStream();
            byte[] data = readFully(is);
            is.close();

            return new JSONObject(new String(data, StandardCharsets.UTF_8));
        } finally {
            conn.disconnect();
        }
    }

    private boolean downloadFile(String urlStr, File outFile, String expectedSha256,
                                  int basePercent, int percentRange) {
        HttpURLConnection conn = null;
        InputStream is = null;
        FileOutputStream fos = null;

        try {
            URL url = new URL(urlStr);
            conn = (HttpURLConnection) url.openConnection();
            conn.setConnectTimeout(30000);
            conn.setReadTimeout(120000);
            conn.setRequestMethod("GET");
            conn.setRequestProperty("User-Agent", "WoWee-Android/1.0");

            int responseCode = conn.getResponseCode();
            if (responseCode != 200) {
                Log.e(TAG, "Download failed: HTTP " + responseCode + " for " + urlStr);
                return false;
            }

            long contentLength = conn.getContentLengthLong();
            is = new BufferedInputStream(conn.getInputStream(), BUFFER_SIZE);
            fos = new FileOutputStream(outFile);

            byte[] buffer = new byte[BUFFER_SIZE];
            long totalRead = 0;
            int bytesRead;
            long lastProgressReport = 0;

            while ((bytesRead = is.read(buffer)) != -1) {
                if (mCancelled) {
                    return false;
                }
                fos.write(buffer, 0, bytesRead);
                totalRead += bytesRead;

                // Report progress every 5 MB
                if (totalRead - lastProgressReport > 5 * 1024 * 1024 && contentLength > 0) {
                    lastProgressReport = totalRead;
                    int subPercent = (int) (totalRead * percentRange / contentLength);
                    reportProgress("Downloading...", basePercent + subPercent);
                }
            }

            fos.flush();

            // Verify SHA256 if provided
            if (!expectedSha256.isEmpty()) {
                String actual = sha256(outFile);
                if (!expectedSha256.equalsIgnoreCase(actual)) {
                    Log.e(TAG, "SHA256 mismatch for " + outFile.getName());
                    outFile.delete();
                    return false;
                }
            }

            return true;

        } catch (IOException e) {
            Log.e(TAG, "Download error", e);
            outFile.delete();
            return false;
        } finally {
            try { if (fos != null) fos.close(); } catch (IOException ignored) {}
            try { if (is != null) is.close(); } catch (IOException ignored) {}
            if (conn != null) conn.disconnect();
        }
    }

    private boolean extractZip(File zipFile, File destDir) {
        try (ZipInputStream zis = new ZipInputStream(new BufferedInputStream(
                new FileInputStream(zipFile)))) {

            byte[] buffer = new byte[BUFFER_SIZE];
            ZipEntry entry;

            while ((entry = zis.getNextEntry()) != null) {
                if (mCancelled) return false;

                File entryFile = new File(destDir, entry.getName());

                // Security: prevent zip-slip attacks
                if (!entryFile.getCanonicalPath().startsWith(destDir.getCanonicalPath() + File.separator)) {
                    Log.w(TAG, "Skipping zip-slip entry: " + entry.getName());
                    continue;
                }

                if (entry.isDirectory()) {
                    entryFile.mkdirs();
                } else {
                    entryFile.getParentFile().mkdirs();
                    try (FileOutputStream fos = new FileOutputStream(entryFile)) {
                        int len;
                        while ((len = zis.read(buffer)) > 0) {
                            fos.write(buffer, 0, len);
                        }
                    }
                }
                zis.closeEntry();
            }
            return true;

        } catch (IOException e) {
            Log.e(TAG, "Extraction error", e);
            return false;
        }
    }

    private void writeLocalManifest(JSONObject manifest) {
        File manifestFile = new File(mDataDir, "manifest.json");
        try (FileOutputStream fos = new FileOutputStream(manifestFile)) {
            fos.write(manifest.toString(2).getBytes(StandardCharsets.UTF_8));
        } catch (IOException e) {
            Log.e(TAG, "Failed to write local manifest", e);
        }
    }

    // --- Utilities ---

    private static byte[] readFully(InputStream is) throws IOException {
        java.io.ByteArrayOutputStream bos = new java.io.ByteArrayOutputStream();
        byte[] buffer = new byte[BUFFER_SIZE];
        int len;
        while ((len = is.read(buffer)) != -1) {
            bos.write(buffer, 0, len);
        }
        return bos.toByteArray();
    }

    private static String sha256(File file) throws IOException {
        try (FileInputStream fis = new FileInputStream(file)) {
            MessageDigest digest = MessageDigest.getInstance("SHA-256");
            byte[] buffer = new byte[BUFFER_SIZE];
            int len;
            while ((len = fis.read(buffer)) != -1) {
                digest.update(buffer, 0, len);
            }
            byte[] hash = digest.digest();
            StringBuilder sb = new StringBuilder();
            for (byte b : hash) {
                sb.append(String.format("%02x", b));
            }
            return sb.toString();
        } catch (java.security.NoSuchAlgorithmException e) {
            throw new IOException("SHA-256 not available", e);
        }
    }

    // --- Data classes ---

    private static class BundleInfo {
        final String name;
        final String url;
        final String sha256;
        final long size;

        BundleInfo(String name, String url, String sha256, long size) {
            this.name = name;
            this.url = url;
            this.sha256 = sha256;
            this.size = size;
        }
    }
}