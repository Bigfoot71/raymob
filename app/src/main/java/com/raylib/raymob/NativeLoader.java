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

package com.raylib.raymob;  // Don't change the package name (see gradle.properties)

import android.app.NativeActivity;
import android.view.KeyEvent;
import android.view.View;
import android.os.Bundle;

public class NativeLoader extends NativeActivity {

    private Features features;

    // Loading method of your native application
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setImmersiveMode();                     // Activate immersive mode
        features = new Features(this);  // Instantiates the class allowing access to specific Android features
        System.loadLibrary("raymob");   // Load your game library (don't change raymob, see gradle.properties)
    }

    // Handling loss and regain of application focus
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) setImmersiveMode();       // If the app has focus, re-enable immersive mode
    }

    // Set immersive mode method
    // NOTE: The method is deprecated, you can use androidX instead
    //       but this will greatly increase the size of the application
    private void setImmersiveMode() {
        getWindow().getDecorView().setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_FULLSCREEN              |
            View.SYSTEM_UI_FLAG_HIDE_NAVIGATION         |
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY        |
            View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN       |
            View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION  |
            View.SYSTEM_UI_FLAG_LAYOUT_STABLE
        );
    }

    // Static method to get the instance of features (from C/C++)
    public Features getFeatures() {
        return features;
    }

    // Callback methods for managing the Android software keyboard
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if (features.softKeyboard != null) {
            features.softKeyboard.onKeyUpEvent(event);
        }
        return super.onKeyDown(keyCode, event);
    }

}