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

// TODO: Replaced method duplications with
//  interfaces and null object pattern (?)

package com.raylib.raymob;

import android.content.Context;
import com.raylib.features.Vibration;
import com.raylib.features.Accelerometer;
import com.raylib.features.SoftKeyboard;

public class Features {

    public final Vibration vibrator;
    public final Accelerometer accelerometer;
    public final SoftKeyboard softKeyboard;

    public Features(Context context) {
        vibrator = BuildConfig.FEATURE_VIBRATION ? new Vibration(context) : null;
        accelerometer = BuildConfig.FEATURE_ACCELEROMETER ? new Accelerometer(context) : null;
        softKeyboard = BuildConfig.FEATURE_SOFT_KEYBOARD ? new SoftKeyboard(context) : null;
    }

    /* VIBRATION */

    public void vibrate(float seconds) {
        if (vibrator != null) vibrator.vibrate(seconds);
    }

    /* ACCELEROMETER */

    public void startAccelerometerListening() {
        if (accelerometer != null) accelerometer.startListening();
    }

    public void stopAccelerometerListening() {
        if (accelerometer != null) accelerometer.stopListening();
    }

    public float getAccelerometerX() {
        if (accelerometer != null) return accelerometer.getX();
        return 0;
    }

    public float getAccelerometerY() {
        if (accelerometer != null) return accelerometer.getY();
        return 0;
    }

    public float getAccelerometerZ() {
        if (accelerometer != null) return accelerometer.getZ();
        return 0;
    }

    /* SOFT KEYBOARD */

    public void showKeyboard() {
        if (softKeyboard != null) softKeyboard.showKeyboard();
    }

    public void hideKeyboard() {
        if (softKeyboard != null) softKeyboard.hideKeyboard();
    }

    public boolean isKeyboardActive() {
        if (softKeyboard != null) return softKeyboard.isActive();
        return false;
    }

    public int getLastKeyCode() {
        if (softKeyboard != null) return softKeyboard.getLastKeyCode();
        return 0;
    }

    public char getLastKeyLabel() {
        if (softKeyboard != null) return softKeyboard.getLastKeyLabel();
        return 0;
    }

    public int getLastKeyUnicode() {
        if (softKeyboard != null) return softKeyboard.getLastKeyUnicode();
        return 0;
    }

    public void clearLastKeyEvent() {
        if (softKeyboard != null) softKeyboard.clearLastEvent();
    }

}
