# crypto_pro_flutter

Flutter plugin for working with **CryptoPro CSP SDK** on Android and iOS.

## Overview

The plugin provides functionality for:

- Managing licenses
- Importing PFX containers
- Working with trusted and personal certificate stores
- Accessing external containers (Android, e.g., Rutoken)
- Signing files and messages using CAdES formats (CADES-BES and CAdES-X Long Type 1)

## Supported Platforms

### Android

Full support.

### iOS

External hardware containers are **not supported**.

## Installation

Add the dependency:

```yaml
dependencies:
  crypto_pro_flutter: ^latest_version
```

Then run:

```bash
flutter pub get
```

## Android Setup (Required)

### Step 1

CryptoPro SDK is distributed as `.aar` libraries and must be added
manually. Copy folder with `.aar` files from plugin sources:

    android/libs

into your main application Android `app` folder:

    android/app/libs

### Step 2

In your application `android/app/build.gradle` add:

```gradle
dependencies {
    implementation fileTree(dir: 'libs', include: ['*.aar'])
}
```

### Step 3

In your application `android/app/build.gradle` add:

#### Android Gradle Plugin 7.2 and newer

```gradle
android {
    packagingOptions {
        jniLibs {
            keepDebugSymbols += [
                '*/arm64-v8a/*.so',
                '*/armeabi-v7a/*.so',
                '*/x86_64/*.so'
            ]
        }
    }
}
```

#### Android Gradle Plugin 7.1 and older

```gradle
android {
    packagingOptions {
        doNotStrip "*/arm64-v8a/*.so"
        doNotStrip "*/armeabi-v7a/*.so"
        doNotStrip "*/x86_64/*.so"
    }
}
```

### Step 4

#### Android Gradle Plugin 4.2.0 and newer

In your application `android/app/build.gradle` add:

```gradle
android {
    packagingOptions {
        jniLibs {
            useLegacyPackaging true
        }
    }
}
```

#### Android Gradle Plugin 3.3.0 - 8.0

In your application `android/gradle.properties` add:

```
android.bundle.enableUncompressedNativeLibs = false
```

#### Android Gradle Plugin 3.6.0 - 4.1.0

In your application `AndroidManifest.xml`:

```
<application
    android:extractNativeLibs="true">
</application>
```

### Step 5

It is recommended not to use **resource optimization**, by doing the following:

In your application `android/app/build.gradle` add:

```gradle
android {
    buildTypes {
        release {
            shrinkResources false
        }
        debug {
            shrinkResources false
        }
    }
}
```

In your application `android/gradle.properties` add:

```
android.enableResourceOptimizations=false
```

## API Overview

You can view all available methods and their descriptions in the [dart-doc](https://pub.dev/documentation/crypto_pro_flutter/latest)

## TODO

- Improve error handling (specifically native Crypto Pro errors)
- Add more cases in integration tests
- Support external containers on iOS
