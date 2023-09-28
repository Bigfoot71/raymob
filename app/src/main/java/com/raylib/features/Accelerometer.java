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
