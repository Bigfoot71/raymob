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

package com.raylib.raymob;

import android.content.Context;
import com.raylib.features.*;
import java.util.Optional;

public class Features {

    /* FEATURES INSTANCES */

    private final Optional<Vibration> vibrator;
    private final Optional<Sensor> sensor;
    private final Optional<SoftKeyboard> softKeyboard;
    private final Optional<DisplayManager> display;

    /* CONSTRUCTOR */

    public Features(Context context) {
        vibrator = BuildConfig.FEATURE_VIBRATION ? Optional.of(new Vibration(context)) : Optional.empty();
        sensor = BuildConfig.FEATURE_ACCELEROMETER ? Optional.of(new Sensor(context)) : Optional.empty();
        softKeyboard = BuildConfig.FEATURE_SOFT_KEYBOARD ? Optional.of(new SoftKeyboard(context)) : Optional.empty();
        display = BuildConfig.FEATURE_DISPLAY ? Optional.of(new DisplayManager(context)) : Optional.empty();
    }

    /* VIBRATION */

    public void vibrate(float seconds) {
        vibrator.ifPresent(v -> v.vibrate(seconds));
    }

    /* SENSOR */

    public void startSensorListening() {
        sensor.ifPresent(Sensor::startListening);
    }

    public void stopSensorListening() {
        sensor.ifPresent(Sensor::stopListening);
    }

    /* ACCELEROMETER */
    public float getAccelerometerX() {
        return sensor.map(Sensor::getAccelerometerX).orElse(0f);
    }

    public float getAccelerometerY() {
        return sensor.map(Sensor::getAccelerometerY).orElse(0f);
    }

    public float getAccelerometerZ() {
        return sensor.map(Sensor::getAccelerometerZ).orElse(0f);
    }

    /* GYROSCOPE */
    public float getGyroscopeX() {
        return sensor.map(Sensor::getGyroscopeX).orElse(0f);
    }

    public float getGyroscopeY() {
        return sensor.map(Sensor::getGyroscopeY).orElse(0f);
    }

    public float getGyroscopeZ() {
        return sensor.map(Sensor::getGyroscopeZ).orElse(0f);
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
