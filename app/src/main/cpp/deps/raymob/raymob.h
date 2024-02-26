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

// NOTE: This function is defined in 'raylib/platforms/rcore_android.c'.
// The function is not public by default, so we are making its declaration
// public here now (instead of doing it in 'raylib.h'), in order to keep a
// clean version of raylib.

RMBAPI struct android_app *GetAndroidApp(void);

/* Helper functions */

RMBAPI JNIEnv* AttachCurrentThread(void);                               // Attaches the current native thread to the Java VM environment.
RMBAPI void DetachCurrentThread(void);                                  // Detaches the current native thread from the Java VM environment.

RMBAPI jobject GetNativeLoaderInstance(void);                           // Returns a pointer to the class that initiated the native activity.
RMBAPI jobject GetFeaturesInstance(void);                               // Returns a pointer to the raymob features class.

/* Feature functions */

RMBAPI void Vibrate(float sec);                                         // Initiates device vibration for the specified duration in seconds.

RMBAPI void StartAccelerometerListening(void);                          // Starts listening to accelerometer sensor data.
RMBAPI void StopAccelerometerListening(void);                           // Stops listening to accelerometer sensor data.
RMBAPI Vector3 GetAccelerometerAxis(void);                              // Returns the current accelerometer axis vector.
RMBAPI float GetAccelerometerX(void);                                   // Returns the current value of the X-axis accelerometer.
RMBAPI float GetAccelerometerY(void);                                   // Returns the current value of the Y-axis accelerometer.
RMBAPI float GetAccelerometerZ(void);                                   // Returns the current value of the Z-axis accelerometer.

RMBAPI void ShowSoftKeyboard(void);                                     // Displays the soft keyboard on the screen.
RMBAPI void HideSoftKeyboard(void);                                     // Hides the soft keyboard from the screen.
RMBAPI bool IsSoftKeyboardActive(void);                                 // Checks if the soft keyboard is currently active.
RMBAPI int GetLastSoftKeyCode(void);                                    // Returns the code of the last key pressed on the soft keyboard.
RMBAPI unsigned short GetLastSoftKeyLabel(void);                        // Returns the label of the last key pressed on the soft keyboard.
RMBAPI int GetLastSoftKeyUnicode(void);                                 // Returns the Unicode value of the last key pressed on the soft keyboard.
RMBAPI char GetLastSoftKeyChar(void);                                   // Returns the character value of the last key pressed on the soft keyboard.
RMBAPI void ClearLastSoftKey(void);                                     // Clears the record of the last soft key pressed.
RMBAPI void SoftKeyboardEditText(char* text, unsigned int size);        // Allows editing the text displayed in the soft keyboard.

RMBAPI void KeepScreenOn(bool keepOn);                                  // Controls whether the screen should remain on or not.

#if defined(__cplusplus)
}
#endif

#endif //RAYMOB_H
