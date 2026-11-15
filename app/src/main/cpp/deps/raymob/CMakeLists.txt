# Native Android support library from the raymob template.
add_library(raymoblib STATIC
    helper.c
    sensor.c
    vibrator.c
    display.c
    soft_keyboard.c
    callback.c
)

target_compile_definitions(raymoblib PRIVATE PLATFORM_ANDROID)

# NATIVE_APP_GLUE_DIR is resolved by the parent CMakeLists.txt from either
# ANDROID_NDK or CMAKE_ANDROID_NDK and is shared with the raymob headers.
target_include_directories(raymoblib
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
    PRIVATE
        "${raylib_SOURCE_DIR}/src"
        "${NATIVE_APP_GLUE_DIR}"
)

target_link_libraries(raymoblib PUBLIC
    raylib
    android
    log
)
