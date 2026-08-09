plugins {
    id("com.android.application")
}

android {
    namespace = "com.wowee.app"
    compileSdk = 35

    ndkVersion = "27.2.12479018"

    defaultConfig {
        applicationId = "com.wowee.app"
        minSdk = 26
        targetSdk = 35
        versionCode = 1
        versionName = "1.0.0"

        externalNativeBuild {
            cmake {
                arguments(
                    "-DANDROID_STL=c++_shared",
                    "-DWOWEE_ENABLE_AMD_FSR2=OFF",
                    "-DWOWEE_ENABLE_AMD_FSR3_FRAMEGEN=OFF",
                    "-DWOWEE_BUILD_AMD_FSR3_RUNTIME=OFF",
                    "-DWOWEE_BUILD_TESTS=OFF",
                    "-DWOWEE_ANDROID=ON"
                )
                abiFilters("arm64-v8a")
            }
        }

        ndk {
            abiFilters("arm64-v8a")
        }
    }

    signingConfigs {
        create("release") {
            // CI signs with its own keystore via environment variables
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = true
            isShrinkResources = true
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
            signingConfig = signingConfigs.getByName("release")
        }
        debug {
            isDebuggable = true
            isJniDebuggable = true
        }
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }

    sourceSets {
        getByName("main") {
            assets.srcDirs("../../../assets")
        }
    }

    buildFeatures {
        prefab = true
    }
}

dependencies {
    implementation("org.libsdl.app:SDL2:2.30.9")
    implementation("androidx.core:core:1.13.1")
    implementation("androidx.appcompat:appcompat:1.7.0")
    implementation("androidx.lifecycle:lifecycle-process:2.8.4")
    implementation("com.squareup.okhttp3:okhttp:4.12.0")
}