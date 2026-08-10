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
"-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON",
                    "-DWOWEE_ANDROID=ON",
                    "-DANDROID_ALLOW_UNDEFINED_SYMBOLS=TRUE"
                )
                abiFilters("arm64-v8a")
            }
        }

        }

    buildTypes {
        release {
            signingConfig = signingConfigs.getByName("debug")  // debug keystore for CI
            isMinifyEnabled = false
            isShrinkResources = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
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
    // SDL2: Java source copied from release tarball in CI workflow
    // Native lib built via CMake (extern/SDL)
    implementation("androidx.core:core:1.13.1")
    implementation("androidx.appcompat:appcompat:1.7.0")
    implementation("androidx.lifecycle:lifecycle-process:2.8.4")
    implementation("com.squareup.okhttp3:okhttp:4.12.0")
    implementation("com.google.oboe:oboe:1.9.0")
}