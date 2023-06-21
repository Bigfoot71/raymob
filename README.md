# Raymob

Simple raylib implementation for Android Studio.

## How to use ?

Open the `gradle.properties` file and fill in the values for your application, such as the app name, package name, version, etc.

Make sure not to modify the occurrences of `com.bigfoot.raymob` and `raymob` as they will be automatically replaced during compilation with the information provided in `gradle.properties`.

And that's it, all you have to do is code your app!

## About

This project directly integrates the slightly modified source of raylib 4.5 (which will be updated by me with each new release of raylib), containing, among others, these new functions:
```
RLAPI JNIEnv *AttachCurrentThread(void);
RLAPI void DetachCurrentThread(void);
RLAPI const jobject GetNativeLoaderInstance(void);
RLAPI struct android_app *GetAndroidApp(void);
```

These new functions will allow you to call easily static and non-static public methods of your NativeLoader class from your C/C++ code, and also access the `struct android_app` of your application.

As mentioned above, this project is compatible with C/C++ for the logic of your native application.

### Enjoy!