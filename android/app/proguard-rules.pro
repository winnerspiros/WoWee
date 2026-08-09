# ProGuard rules for WoWee Android

# Keep SDL2 native methods
-keep class org.libsdl.app.** { *; }

# Keep JNI native methods in our activity
-keep class com.wowee.app.WoWeeActivity {
    native <methods>;
}

# Keep OkHttp
-dontwarn okhttp3.**
-keep class okhttp3.** { *; }
-dontwarn okio.**

# General
-keepattributes *Annotation*
-keepattributes SourceFile,LineNumberTable