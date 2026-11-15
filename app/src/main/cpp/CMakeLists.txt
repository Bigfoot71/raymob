# Native Android application using the raylib + raymob template layout.
cmake_minimum_required(VERSION 3.22.1)

project("${APP_LIB_NAME}" LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# raylib is pinned to the template-compatible 5.5 release. FetchContent is
# used because the raylib submodule directory is not checked into this repo.
include(FetchContent)
set(PLATFORM "Android" CACHE STRING "raylib target platform" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "Do not build raylib examples" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build raylib as a static library" FORCE)
set(WITH_PIC ON CACHE BOOL "Build static dependencies as PIC" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

# The glue source must be compiled directly into lib${APP_LIB_NAME}.so.
# Merely linking raylib's static archive does not guarantee that the object
# exporting ANativeActivity_onCreate is extracted from that archive.
if(DEFINED ANDROID_NDK AND
   EXISTS "${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c")
    set(NATIVE_APP_GLUE_DIR "${ANDROID_NDK}/sources/android/native_app_glue")
elseif(DEFINED CMAKE_ANDROID_NDK AND
       EXISTS "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c")
    set(NATIVE_APP_GLUE_DIR "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue")
else()
    message(FATAL_ERROR "Android NDK native_app_glue sources were not found.")
endif()

# Restore and build the Android-specific raymob support library.
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/raymob"
                 "${CMAKE_CURRENT_BINARY_DIR}/raymob")

file(GLOB GAME_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/game/*.cpp")

add_library(${APP_LIB_NAME} SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    ${GAME_SOURCES}
    "${NATIVE_APP_GLUE_DIR}/android_native_app_glue.c"
)

target_include_directories(${APP_LIB_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/game"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/raymob"
    "${NATIVE_APP_GLUE_DIR}"
)

target_compile_definitions(${APP_LIB_NAME} PRIVATE PLATFORM_ANDROID)
target_link_libraries(${APP_LIB_NAME} PRIVATE
    raylib
    raymoblib
    android
    log
    EGL
    GLESv2
    OpenSLES
    m
)

# Keep the NativeActivity entry point exported even with section garbage
# collection or static-library extraction.
target_link_options(${APP_LIB_NAME} PRIVATE
    "-Wl,-u,ANativeActivity_onCreate"
)
