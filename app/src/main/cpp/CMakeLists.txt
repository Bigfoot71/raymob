# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Specifies the minimum required version of CMake
cmake_minimum_required(VERSION 3.22.1)

# Set C99 standard for C and C++11 standard for C++
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

# Set the project name using the APP_LIB_NAME variable given by `build.gradle:app`
project("${APP_LIB_NAME}")

# Fetch all source files of your project (recursively)
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/*.c" "${CMAKE_SOURCE_DIR}/*.cpp")

# Exclude raylib from the list using regular expressions
list(FILTER SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/raylib/external/.*")
list(FILTER SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/raylib/.*")

# Properly add the necessary files to the compilation of raylib
list(APPEND SOURCES
        "${CMAKE_SOURCE_DIR}/raylib/rcore.c"
        "${CMAKE_SOURCE_DIR}/raylib/rmodels.c"
        "${CMAKE_SOURCE_DIR}/raylib/rshapes.c"
        "${CMAKE_SOURCE_DIR}/raylib/rtext.c"
        "${CMAKE_SOURCE_DIR}/raylib/rtextures.c"
        "${CMAKE_SOURCE_DIR}/raylib/raudio.c"
        "${CMAKE_SOURCE_DIR}/raylib/utils.c"
        )

# Add headers directory for android_native_app_glue.c
include_directories(${ANDROID_NDK}/sources/android/native_app_glue/)

# Add android_native_app_glue.c to list of source files
list(APPEND SOURCES ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# Creates a shared library with game source files
add_library(${APP_LIB_NAME} SHARED ${SOURCES})

# Defines compiler macros for the library
target_compile_definitions(${APP_LIB_NAME} PRIVATE
        GRAPHICS_API_OPENGL_ES2
        PLATFORM_ANDROID
        __ANDROID__
        MAL_NO_OSS
        )

# We apply flags depending of the build type
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(${APP_LIB_NAME} PRIVATE _DEBUG DEBUG)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-limit-debug-info")
    set(CMAKE_C_FLAGS_DEBUG "-g -fno-limit-debug-info")
endif()
if(CMAKE_BUILD_TYPE MATCHES "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

# Includes Raylib header files
target_include_directories(${APP_LIB_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/raylib")

# Link required libraries to native application
target_link_libraries(${APP_LIB_NAME}
        log
        android
        EGL
        GLESv2
        OpenSLES
        dl
        m
        c)

# Adds library-specific binding option
target_link_options(${APP_LIB_NAME} PRIVATE "-u ANativeActivity_onCreate")

# Search the log library
find_library(log-lib log)

# Links the log library to the application
target_link_libraries(${APP_LIB_NAME} ${log-lib})
