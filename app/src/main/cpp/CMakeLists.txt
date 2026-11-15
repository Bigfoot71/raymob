# Native Android client. All project-owned native files stay under app/src/main/cpp.
cmake_minimum_required(VERSION 3.22.1)

project("${APP_LIB_NAME}" LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# raylib remains the only required native dependency. The game does not call
# raymob-only helpers, so do not add deps/raymob or compile its display.c.
set(RAYLIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/raylib")
if(NOT EXISTS "${RAYLIB_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "Missing raylib source at app/src/main/cpp/deps/raylib. "
        "Keep the repository's existing raylib submodule in place; this update "
        "does not require or reference deps/raymob.")
endif()

# Keep raylib's own Android platform selection and target configuration.
set(BUILD_EXAMPLES OFF CACHE BOOL "Do not build raylib examples" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "Do not build raylib games" FORCE)
add_subdirectory("${RAYLIB_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/raylib")

file(GLOB GAME_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/game/*.cpp")

add_library(${APP_LIB_NAME} SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    ${GAME_SOURCES})

target_include_directories(${APP_LIB_NAME} PRIVATE
    "${RAYLIB_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/game")

target_compile_definitions(${APP_LIB_NAME} PRIVATE PLATFORM_ANDROID)
target_link_libraries(${APP_LIB_NAME} PRIVATE raylib)

if(ANDROID)
    # Android NativeActivity glue is part of the NDK. Compile it directly
    # into the app library, as required by NativeActivity.
    if(DEFINED ANDROID_NDK AND EXISTS "${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c")
        set(NATIVE_APP_GLUE_DIR "${ANDROID_NDK}/sources/android/native_app_glue")
    elseif(DEFINED CMAKE_ANDROID_NDK AND EXISTS "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c")
        set(NATIVE_APP_GLUE_DIR "${CMAKE_ANDROID_NDK}/sources/android/native_app_glue")
    else()
        message(FATAL_ERROR "Android NDK native_app_glue sources were not found.")
    endif()

    target_sources(${APP_LIB_NAME} PRIVATE
        "${NATIVE_APP_GLUE_DIR}/android_native_app_glue.c")
    target_include_directories(${APP_LIB_NAME} PRIVATE "${NATIVE_APP_GLUE_DIR}")
    target_link_libraries(${APP_LIB_NAME} PRIVATE
        android log EGL GLESv2 OpenSLES m)
    target_link_options(${APP_LIB_NAME} PRIVATE
        "-Wl,-u,ANativeActivity_onCreate")
endif()
