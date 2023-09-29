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
#include <string.h> // Used for 'SoftKeyboardEditText()'

/* VIBRATION */

void Vibrate(float sec)
{
    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();
            jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);
            jmethodID method = (*env)->GetMethodID(env, featuresClass, "vibrate", "(F)V");
            (*env)->CallVoidMethod(env, featuresInstance, method, (jfloat)sec);
        DetachCurrentThread();
    }
}

/* ACCELEROMETER */

void StartAccelerometerListening(void)
{
    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();
            jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);
            jmethodID method = (*env)->GetMethodID(env, featuresClass, "startAccelerometerListening", "()V");
            (*env)->CallVoidMethod(env, featuresInstance, method);
        DetachCurrentThread();
    }
}

void StopAccelerometerListening(void)
{
    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();
            jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);
            jmethodID method = (*env)->GetMethodID(env, featuresClass, "stopAccelerometerListening", "()V");
            (*env)->CallVoidMethod(env, featuresInstance, method);
        DetachCurrentThread();
    }
}

Vector3 GetAccelerometerAxis(void)
{
    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();

            jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);

            jmethodID methodX = (*env)->GetMethodID(env, featuresClass, "getAccelerometerX", "()F");
            jmethodID methodY = (*env)->GetMethodID(env, featuresClass, "getAccelerometerY", "()F");
            jmethodID methodZ = (*env)->GetMethodID(env, featuresClass, "getAccelerometerZ", "()F");

            Vector3 value = {
                (*env)->CallFloatMethod(env, featuresInstance, methodX),
                (*env)->CallFloatMethod(env, featuresInstance, methodY),
                (*env)->CallFloatMethod(env, featuresInstance, methodZ)
            };

        DetachCurrentThread();

        return value;
    }

    return (Vector3) {0};
}

float GetAccelerometerX(void)
{
    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();
            jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);
            jmethodID method = (*env)->GetMethodID(env, featuresClass, "getAccelerometerX", "()F");
            float value = (*env)->CallFloatMethod(env, featuresInstance, method);
        DetachCurrentThread();

        return value;
    }

    return 0;
}

float GetAccelerometerY(void)
{
    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();
            jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);
            jmethodID method = (*env)->GetMethodID(env, featuresClass, "getAccelerometerY", "()F");
            float value = (*env)->CallFloatMethod(env, featuresInstance, method);
        DetachCurrentThread();

        return value;
    }

    return 0;
}

float GetAccelerometerZ(void)
{
    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();
            jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);
            jmethodID method = (*env)->GetMethodID(env, featuresClass, "getAccelerometerZ", "()F");
            float value = (*env)->CallFloatMethod(env, featuresInstance, method);
        DetachCurrentThread();

        return value;
    }

    return 0;
}

/* SOFT KEYBOARD */

void ShowSoftKeyboard(void)
{
    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();
            jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);
            jmethodID method = (*env)->GetMethodID(env, featuresClass, "showKeyboard", "()V");
            (*env)->CallVoidMethod(env, featuresInstance, method);
        DetachCurrentThread();
    }
}

void HideSoftKeyboard(void)
{
    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();
            jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);
            jmethodID method = (*env)->GetMethodID(env, featuresClass, "hideKeyboard", "()V");
            (*env)->CallVoidMethod(env, featuresInstance, method);
        DetachCurrentThread();
    }
}

bool IsSoftKeyboardActive(void)
{
    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();
            jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);
            jmethodID method = (*env)->GetMethodID(env, featuresClass, "isKeyboardActive", "()Z");
            bool value = (*env)->CallBooleanMethod(env, featuresInstance, method);
        DetachCurrentThread();

        return value;
    }

    return false;
}

int GetLastSoftKeyCode(void)
{
    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();
            jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);
            jmethodID method = (*env)->GetMethodID(env, featuresClass, "getLastKeyCode", "()I");
            int value = (*env)->CallIntMethod(env, featuresInstance, method);
        DetachCurrentThread();

        return value;
    }

    return 0;
}

unsigned short GetLastSoftKeyLabel(void)
{
    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();
            jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);
            jmethodID method = (*env)->GetMethodID(env, featuresClass, "getLastKeyLabel", "()C");
            unsigned short value = (*env)->CallCharMethod(env, featuresInstance, method);
        DetachCurrentThread();

        return value;
    }

    return 0;
}

int GetLastSoftKeyUnicode(void)
{
    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();
            jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);
            jmethodID method = (*env)->GetMethodID(env, featuresClass, "getLastKeyUnicode", "()I");
            int value = (*env)->CallIntMethod(env, featuresInstance, method);
        DetachCurrentThread();

        return value;
    }

    return 0;
}

char GetLastSoftKeyChar(void)
{
#   define KEYCODE_ENTER    66
#   define KEYCODE_DEL      67

    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        char value = '\0';

        JNIEnv* env = AttachCurrentThread();
        jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);

        jmethodID methodKeyCode = (*env)->GetMethodID(env, featuresClass, "getLastKeyCode", "()I");
        int keyCode = (*env)->CallIntMethod(env, featuresInstance, methodKeyCode);

        if (keyCode != 0)
        {
            switch (keyCode)
            {
                case KEYCODE_ENTER: {
                    value = '\n';
                } break;

                case KEYCODE_DEL: {
                    value = '\b';
                } break;

                default: {
                    jmethodID methodKeyUnicode = (*env)->GetMethodID(env, featuresClass, "getLastKeyUnicode", "()I");
                    int u = (*env)->CallIntMethod(env, featuresInstance, methodKeyUnicode);
                    if (u > 0xFF) value = '?';
                    else value = (char) u;
                }
            }
        }

        DetachCurrentThread();

        return value;
    }

    return '\0';
}

void ClearLastSoftKey(void)
{
    jobject featuresInstance = GetFeaturesInstance();

    if (featuresInstance != NULL)
    {
        JNIEnv* env = AttachCurrentThread();
            jclass featuresClass = (*env)->GetObjectClass(env, featuresInstance);
            jmethodID method = (*env)->GetMethodID(env, featuresClass, "clearLastKeyEvent", "()V");
            (*env)->CallVoidMethod(env, featuresInstance, method);
        DetachCurrentThread();
    }
}

void SoftKeyboardEditText(char* text, unsigned int size)
{
    char c = GetLastSoftKeyChar();
    if (c == '\0') return;

    unsigned int len = strlen(text);

    if (c == '\b' && len > 0)
    {
        text[len-1] = '\0';
    }
    else if (c != '\b' && len < size)
    {
        text[len++] = c;
        text[len] = '\0';
    }

    ClearLastSoftKey();
}
