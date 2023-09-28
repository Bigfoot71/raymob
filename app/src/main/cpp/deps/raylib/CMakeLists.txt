# Define a library for raylib
add_library(raylib STATIC
    rcore.c
    rmodels.c
    rshapes.c
    rtext.c
    rtextures.c
    raudio.c
    utils.c
    )

# Include headers directory for android_native_app_glue.c
include_directories(${ANDROID_NDK}/sources/android/native_app_glue/)

# Add android_native_app_glue.c to the source files
list(APPEND SOURCES ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# Define compiler macros for raylib
target_compile_definitions(raylib PRIVATE
    GRAPHICS_API_OPENGL_ES2
    PLATFORM_ANDROID
    __ANDROID__
    MAL_NO_OSS
    )

# Link required libraries to raylib
target_link_libraries(raylib log android EGL GLESv2 OpenSLES dl m c)

# Add library-specific binding option
target_link_options(raylib PRIVATE "-u ANativeActivity_onCreate")
