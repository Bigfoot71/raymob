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

package com.raylib.features;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorManager;

public class Accelerometer implements android.hardware.SensorEventListener {

    private SensorManager sensorManager;
    private Sensor accelerometer;
    private float[] accelerometerValues = new float[3]; // To store the axis values (x, y, z)

    public Accelerometer(Context context) {
        sensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
        if (sensorManager != null) {
            accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        }
    }

    // Method to start listening to the accelerometer
    public void startListening() {
        if (accelerometer != null) {
            sensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);
        }
    }

    // Method to stop listening to the accelerometer
    public void stopListening() {
        sensorManager.unregisterListener(this);
    }

    // Method to get the value of the X-axis
    public float getX() {
        return accelerometerValues[0];
    }

    // Method to get the value of the Y-axis
    public float getY() {
        return accelerometerValues[1];
    }

    // Method to get the value of the Z-axis
    public float getZ() {
        return accelerometerValues[2];
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            accelerometerValues[0] = event.values[0]; // X-axis
            accelerometerValues[1] = event.values[1]; // Y-axis
            accelerometerValues[2] = event.values[2]; // Z-axis
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // Do nothing in case of sensor accuracy change
    }
}
