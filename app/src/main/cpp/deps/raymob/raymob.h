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

#ifndef RAYMOB_H
#define RAYMOB_H

#include "android_native_app_glue.h"
#include "raylib.h"
#include "jni.h"

#ifndef RMBAPI
#define RMBAPI
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief Retrieves the Android application object.
 *
 * This function is defined in 'raylib/platforms/rcore_android.c'.
 * The function is not public by default, so we are making its declaration
 * public here now (instead of doing it in 'raylib.h'), in order to keep a
 * clean version of raylib.
 *
 * @return Pointer to the Android application object.
 */
RMBAPI struct android_app *GetAndroidApp(void);

/* Helper functions */

/**
 * @brief Attaches the current native thread to the Java VM environment.
 *
 * @return Pointer to the JNIEnv structure.
 */
RMBAPI JNIEnv* AttachCurrentThread(void);

/**
 * @brief Detaches the current native thread from the Java VM environment.
 */
RMBAPI void DetachCurrentThread(void);

/**
 * @brief Returns a pointer to the class that initiated the native activity.
 *
 * @return Pointer to the native loader instance.
 */
RMBAPI jobject GetNativeLoaderInstance(void);

/**
 * @brief Returns a pointer to the raymob features class.
 *
 * @return Pointer to the features instance.
 */
RMBAPI jobject GetFeaturesInstance(void);

/**
 * @brief Gets the cache directory path of the Android application.
 *
 * @warning This function returns a string allocated on the heap.
 * The responsibility for releasing the memory lies with the user.
 *
 * @return Pointer to the cache directory path string.
 */
RMBAPI char* GetCacheDir(void);

/* Feature functions */

/**
 * @brief Initiates device vibration for the specified duration in seconds.
 *
 * @param sec Duration of vibration in seconds.
 */
RMBAPI void Vibrate(float sec);

/**
 * @brief Starts listening to accelerometer sensor data.
 */
RMBAPI void StartAccelerometerListening(void);

/**
 * @brief Stops listening to accelerometer sensor data.
 */
RMBAPI void StopAccelerometerListening(void);

/**
 * @brief Returns the current accelerometer axis vector.
 *
 * @return Current accelerometer axis vector.
 */
RMBAPI Vector3 GetAccelerometerAxis(void);

/**
 * @brief Returns the current value of the X-axis accelerometer.
 *
 * @return Current value of the X-axis accelerometer.
 */
RMBAPI float GetAccelerometerX(void);

/**
 * @brief Returns the current value of the Y-axis accelerometer.
 *
 * @return Current value of the Y-axis accelerometer.
 */
RMBAPI float GetAccelerometerY(void);

/**
 * @brief Returns the current value of the Z-axis accelerometer.
 *
 * @return Current value of the Z-axis accelerometer.
 */
RMBAPI float GetAccelerometerZ(void);

/**
 * @brief Displays the soft keyboard on the screen.
 */
RMBAPI void ShowSoftKeyboard(void);

/**
 * @brief Hides the soft keyboard from the screen.
 */
RMBAPI void HideSoftKeyboard(void);

/**
 * @brief Checks if the soft keyboard is currently active.
 *
 * @return True if the soft keyboard is active, false otherwise.
 */
RMBAPI bool IsSoftKeyboardActive(void);

/**
 * @brief Returns the code of the last key pressed on the soft keyboard.
 *
 * @return Code of the last key pressed on the soft keyboard.
 */
RMBAPI int GetLastSoftKeyCode(void);

/**
 * @brief Returns the label of the last key pressed on the soft keyboard.
 *
 * @return Label of the last key pressed on the soft keyboard.
 */
RMBAPI unsigned short GetLastSoftKeyLabel(void);

/**
 * @brief Returns the Unicode value of the last key pressed on the soft keyboard.
 *
 * @return Unicode value of the last key pressed on the soft keyboard.
 */
RMBAPI int GetLastSoftKeyUnicode(void);

/**
 * @brief Returns the character value of the last key pressed on the soft keyboard.
 *
 * @return Character value of the last key pressed on the soft keyboard.
 */
RMBAPI char GetLastSoftKeyChar(void);

/**
 * @brief Clears the record of the last soft key pressed.
 */
RMBAPI void ClearLastSoftKey(void);

/**
 * @brief Allows editing the text displayed in the soft keyboard.
 *
 * @param text Pointer to the text to be edited.
 * @param size Size of the text buffer.
 */
RMBAPI void SoftKeyboardEditText(char* text, unsigned int size);

/**
 * @brief Controls whether the screen should remain on or not.
 *
 * @param keepOn If true, the screen should remain on; otherwise, it should not.
 */
RMBAPI void KeepScreenOn(bool keepOn);

#if defined(__cplusplus)
}
#endif

#endif //RAYMOB_H
