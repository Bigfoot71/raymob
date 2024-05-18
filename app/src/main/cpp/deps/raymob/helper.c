/*
 *  raymob License (MIT)
 *
 *  Copyright (c) 2023-2024 Le Juez Victor
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include "raymob.h"
#include <stdlib.h>
#include <string.h>

/* Static variables */

static jobject featuresInstance = NULL;

/* Functions definition */

JNIEnv* AttachCurrentThread(void)
{
    JavaVM *vm = GetAndroidApp()->activity->vm;
    JNIEnv *env = NULL;

    (*vm)->AttachCurrentThread(vm, &env, NULL);
    return env;
}

void DetachCurrentThread(void)
{
    JavaVM *vm = GetAndroidApp()->activity->vm;
    (*vm)->DetachCurrentThread(vm);
}

jobject GetNativeLoaderInstance(void)
{
    return GetAndroidApp()->activity->clazz;
}

jobject GetFeaturesInstance(void)
{
    if (featuresInstance == NULL)
    {
        JNIEnv *env = AttachCurrentThread();
        jobject nativeLoaderInstance = GetNativeLoaderInstance();

        jclass nativeLoaderClass = (*env)->GetObjectClass(env, nativeLoaderInstance);
        jmethodID getFeaturesMethod = (*env)->GetMethodID(env, nativeLoaderClass, "getFeatures", "()Lcom/raylib/raymob/Features;");

        if (getFeaturesMethod == NULL) return NULL; // Handle the case where the method is not found

        jobject localFeaturesInstance = (*env)->CallObjectMethod(env, nativeLoaderInstance, getFeaturesMethod);
        featuresInstance = (*env)->NewGlobalRef(env, localFeaturesInstance);

        DetachCurrentThread();
    }

    return featuresInstance;
}

char* GetCacheDir(void)
{
    struct android_app *app = GetAndroidApp();

    JavaVM* vm = app->activity->vm;
    JNIEnv* env = NULL;
    (*vm)->AttachCurrentThread(vm, &env, NULL);

    // Get the activity object and its class
    jobject activity = app->activity->clazz;
    jclass activityClass = (*env)->GetObjectClass(env, activity);

    // Get the method ID for the getCacheDir() method of the activity
    jmethodID getCacheDirMethod = (*env)->GetMethodID(env, activityClass, "getCacheDir", "()Ljava/io/File;");

    // Call the getCacheDir() method to get the cache directory
    jobject cacheDir = (*env)->CallObjectMethod(env, activity, getCacheDirMethod);

    // Get the class object for java.io.File
    jclass fileClass = (*env)->GetObjectClass(env, cacheDir);

    // Get the method ID for the getPath() method of java.io.File
    jmethodID getPathMethod = (*env)->GetMethodID(env, fileClass, "getPath", "()Ljava/lang/String;");

    // Call the getPath() method to get the path of the cache directory
    jstring pathString = (jstring)(*env)->CallObjectMethod(env, cacheDir, getPathMethod);

    // Get the UTF-8 encoded string from the Java string
    const char *pathChars = (*env)->GetStringUTFChars(env, pathString, NULL);

    // Allocate memory for the cache path
    size_t len = strlen(pathChars) + 1; // NOTE: +1 for the null terminator
    char* cachePath = malloc(len);

    // Copy the string to the allocated memory
    if (cachePath)
    {
        strncpy(cachePath, pathChars, len);
        cachePath[len - 1] = '\0'; // NOTE: just for security
    }

    // Release the UTF-8 encoded string
    (*env)->ReleaseStringUTFChars(env, pathString, pathChars);

    // Clean up local references
    (*env)->DeleteLocalRef(env, pathString);
    (*env)->DeleteLocalRef(env, fileClass);
    (*env)->DeleteLocalRef(env, cacheDir);
    (*env)->DeleteLocalRef(env, activityClass);

    // Detach the current thread from the JavaVM
    (*vm)->DetachCurrentThread(vm);

    // Return the cache path
    return cachePath;
}