# Raymob

Raymob is a simple implementation of [raylib](https://www.raylib.com/) for Android Studio.

## Prerequisites

**You will need SDK API 33.0.0 and NDK r21 (or higher, compatible with CMake 3.22.1).**

If you already have this version of SDK and NDK without having Android Studio, you should still be able to compile the project using `gradlew.bat` for Windows or `gradlew` for Linux or MacOS.

## How to Use?

1. Open the `gradle.properties` file and fill in the necessary values for your application, such as the app name, package name, version, etc.
2. That's it! You can start coding your raylib app in the `app/src/main/cpp` directory!

**Note**: Do not modify the instances of `com.raylib.raymob` and `raymob` as they will be automatically replaced during compilation with the information provided in `gradle.properties`.

## Compatibility

By default, the Raymob template targets APIs 24 to 33, which corresponds from Android 7.0 to Android 13 (_so 96.2% of devices according to [apilevels.com](https://apilevels.com/)_). You can also adapt it to target other API versions if necessary.

This project is fully C/C++ compatible for the logical part with raylib and you can also customize the Java part to your wishes.

## Customization

This template can be customized to meet your specific needs. You can add dependencies to the project via Gradle or additional native libraries by adding them to the `app/libs` directory and making the necessary modifications in the `CMakeLists.txt` file.

## Additional Information

This project directly integrates a slightly modified version of raylib 4.5. For each new release of raylib, I will update this integration. The modified version includes new functions, such as:

```c
RLAPI JNIEnv *AttachCurrentThread(void);
RLAPI void DetachCurrentThread(void);
RLAPI const jobject GetNativeLoaderInstance(void);
RLAPI struct android_app *GetAndroidApp(void);
```

These new functions enable easy calling of static and non-static public methods of your NativeLoader class from your C/C++ code, as well as accessing the `struct android_app` of your application.

## Support

If you have any questions or encounter issues while using this implementation, you can seek help by submitting an issue on this repository.

## Useful Links

- [AdMob Integration in Raymob](https://gist.github.com/Bigfoot71/b3a658458ece93ddcb06f4c78f85076a): Gist demonstrating the integration of AdMob in Raymob.
- [Using the vibration feature in Raymob](https://gist.github.com/Bigfoot71/2c89864354a9254ce555f2eca1961590): Gist explaining how to use the vibration of your device if possible.

_Feel free to reach out to me or open a pull request to add your repositories or gists related to Raymob if you believe they can be beneficial to others. I welcome contributions and collaborations to enhance the Raymob project and make it more valuable for the community._
