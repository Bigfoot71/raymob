/*
 *  raymob License (MIT)
 *
 *  Copyright (c) 2023 Le Juez Victor
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

/* Static variables */

static jobject nativeLoaderInstance = NULL;

/* JNI loading (called automatically when the app is first started) */

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv *env;

    if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_6) != JNI_OK)
    {
        return JNI_ERR;
    }

    jclass nativeLoaderClass = (*env)->FindClass(env, "com/raylib/raymob/NativeLoader");
    jmethodID getInstanceMethod = (*env)->GetStaticMethodID(env, nativeLoaderClass, "getInstance", "()Lcom/raylib/raymob/NativeLoader;");
    jobject instance = (*env)->CallStaticObjectMethod(env, nativeLoaderClass, getInstanceMethod);

    nativeLoaderInstance = (*env)->NewGlobalRef(env, instance);

    return JNI_VERSION_1_6;
}

// NOTE 1: Never called (?)
// Found an alternative or let the system
// handle the cleanup when potentially unloading the library?

// NOTE 2: Unloading this reference in onDestroy or APP_DESTROY
// is problematic because there is no guarantee that the library will be unloaded
// and therefore that JNI_OnLoad will be called again when the application is reopened.

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
    JNIEnv *env;

    if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_6) == JNI_OK) // not tested, not called (?)
    {
        if (nativeLoaderInstance != NULL)
        {
            (*env)->DeleteGlobalRef(env, nativeLoaderInstance);
            nativeLoaderInstance = NULL;
        }
    }
}

/* Functions definition */

JNIEnv* AttachCurrentThread(void)
{
    JavaVM *vm = GetAndroidApp()->activity->vm;
    JNIEnv *env;

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
    return nativeLoaderInstance;
}
