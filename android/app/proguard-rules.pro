# ProGuard rules for WoWee Android

# Keep SDL2 native methods
-keep class org.libsdl.app.** { *; }

# Keep JNI native methods
-keepclasseswithmembernames class * {
    native <methods>;
}

# Keep our Activity and Service (reflection in manifest)
-keep class com.wowee.app.WoWeeActivity { *; }
-keep class com.wowee.app.AssetDownloadService { *; }
-keep class com.wowee.app.AssetDownloader { *; }

# OkHttp (used for asset download)
-dontwarn okhttp3.**
-dontwarn okio.**
-keep class okhttp3.** { *; }
-keep class okio.** { *; }

# AndroidX
-keep class androidx.** { *; }
-dontwarn androidx.**

# General
-keepattributes *Annotation*
-keepattributes SourceFile,LineNumberTable
-keepattributes InnerClasses
-keepattributes EnclosingMethod

# Remove logging in release
-assumenosideeffects class android.util.Log {
    public static int v(...);
    public static int d(...);
}