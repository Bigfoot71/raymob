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

#include "jni.h"
#include "raylib.h"

#ifndef RMBAPI
#define RMBAPI
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/* Helper functions */

RMBAPI JNIEnv* AttachCurrentThread(void);
RMBAPI void DetachCurrentThread(void);

RMBAPI jobject GetNativeLoaderInstance(void);
RMBAPI jobject GetFeaturesInstance(void);

/* Feature functions */

RMBAPI void Vibrate(float sec);

RMBAPI void StartAccelerometerListening(void);
RMBAPI void StopAccelerometerListening(void);
RMBAPI Vector3 GetAccelerometerAxis(void);
RMBAPI float GetAccelerometerX(void);
RMBAPI float GetAccelerometerY(void);
RMBAPI float GetAccelerometerZ(void);

RMBAPI void ShowSoftKeyboard(void);
RMBAPI void HideSoftKeyboard(void);
RMBAPI bool IsSoftKeyboardActive(void);
RMBAPI int GetLastSoftKeyCode(void);
RMBAPI unsigned short GetLastSoftKeyLabel(void);
RMBAPI int GetLastSoftKeyUnicode(void);
RMBAPI char GetLastSoftKeyChar(void);
RMBAPI void ClearLastSoftKey(void);
RMBAPI void SoftKeyboardEditText(char* text, unsigned int size);

#if defined(__cplusplus)
}
#endif

#endif //RAYMOB_H
