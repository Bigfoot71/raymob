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

package com.raylib.raymob;

import android.content.Context;
import com.raylib.features.*;
import java.util.Optional;

public class Features {

    /* FEATURES INSTANCES */

    private final Optional<Vibration> vibrator;
    private final Optional<Accelerometer> accelerometer;
    private final Optional<SoftKeyboard> softKeyboard;
    private final Optional<DisplayManager> display;

    /* CONSTRUCTOR */

    public Features(Context context) {
        vibrator = BuildConfig.FEATURE_VIBRATION ? Optional.of(new Vibration(context)) : Optional.empty();
        accelerometer = BuildConfig.FEATURE_ACCELEROMETER ? Optional.of(new Accelerometer(context)) : Optional.empty();
        softKeyboard = BuildConfig.FEATURE_SOFT_KEYBOARD ? Optional.of(new SoftKeyboard(context)) : Optional.empty();
        display = BuildConfig.FEATURE_DISPLAY ? Optional.of(new DisplayManager(context)) : Optional.empty();
    }

    /* VIBRATION */

    public void vibrate(float seconds) {
        vibrator.ifPresent(v -> v.vibrate(seconds));
    }

    /* ACCELEROMETER */

    public void startAccelerometerListening() {
        accelerometer.ifPresent(Accelerometer::startListening);
    }

    public void stopAccelerometerListening() {
        accelerometer.ifPresent(Accelerometer::stopListening);
    }

    public float getAccelerometerX() {
        return accelerometer.map(Accelerometer::getX).orElse(0f);
    }

    public float getAccelerometerY() {
        return accelerometer.map(Accelerometer::getY).orElse(0f);
    }

    public float getAccelerometerZ() {
        return accelerometer.map(Accelerometer::getZ).orElse(0f);
    }

    /* SOFT KEYBOARD */

    public void showKeyboard() {
        softKeyboard.ifPresent(SoftKeyboard::showKeyboard);
    }

    public void hideKeyboard() {
        softKeyboard.ifPresent(SoftKeyboard::hideKeyboard);
    }

    public boolean isKeyboardActive() {
        return softKeyboard.map(SoftKeyboard::isActive).orElse(false);
    }

    public int getLastKeyCode() {
        return softKeyboard.map(SoftKeyboard::getLastKeyCode).orElse(0);
    }

    public char getLastKeyLabel() {
        return softKeyboard.map(SoftKeyboard::getLastKeyLabel).orElse('\0');
    }

    public int getLastKeyUnicode() {
        return softKeyboard.map(SoftKeyboard::getLastKeyUnicode).orElse(0);
    }

    public void clearLastKeyEvent() {
        softKeyboard.ifPresent(SoftKeyboard::clearLastEvent);
    }

    public void onKeyUpEvent(android.view.KeyEvent keyEvent) {
        softKeyboard.ifPresent(sk -> sk.onKeyUpEvent(keyEvent));
    }

    /* DISPLAY MANAGER */

    public void keepScreenOn(boolean keepOn) {
        display.ifPresent(d -> d.keepScreenOn(keepOn));
    }

    public void setImmersiveMode() {
        display.ifPresent(DisplayManager::setImmersiveMode);
    }
}
