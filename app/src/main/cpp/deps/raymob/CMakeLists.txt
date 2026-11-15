# Native Android support library from raymob.
# Gradle preBuild rewrites JNI class descriptors in display.c and soft_keyboard.c;
# keep both original upstream translation units at these exact paths.
add_library(raymoblib STATIC
    helper.c
    sensor.c
    vibrator.c
    display.c
    soft_keyboard.c
    callback.c
)

target_compile_definitions(raymoblib PRIVATE PLATFORM_ANDROID)

target_include_directories(raymoblib
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PRIVATE
        "${raylib_SOURCE_DIR}/src"
        "${ANDROID_NDK}/sources/android/native_app_glue"
)

target_link_libraries(raymoblib PUBLIC raylib android log)
