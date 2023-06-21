package com.raylib.raymob;  // Don't change the package name (see gradle.properties)

import android.app.NativeActivity;
import android.os.Bundle;
import android.view.View;
//import android.util.Log;

public class NativeLoader extends NativeActivity {

    private static NativeLoader instance;   // Used to obtain the instance from C/C++ to call non-static methods easily

    // Loading method of your native application
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        instance = this;                        // Get this instance of NativeLoader
        setImmersiveMode();                     // Activate immersive mode
        System.loadLibrary("raymob");   // Load your game library (don't change raymob, see gradle.properties)
    }

    // Handling loss and regain of application focus
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        // If the app has focus, re-enable immersive mode
        if (hasFocus) {
            setImmersiveMode();
        }

    }

    // Static method get the instance (from C/C++ for example)
    public static NativeLoader getInstance() {
        return instance;
    }

    // Set immersive mode method
    private void setImmersiveMode() {
        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_FULLSCREEN |
                        View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                        View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY |
                        View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                        View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                        View.SYSTEM_UI_FLAG_LAYOUT_STABLE
        );
    }

}