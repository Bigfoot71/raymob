cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("${APP_LIB_NAME}")

add_subdirectory(${CMAKE_SOURCE_DIR}/deps/raylib)
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/raymob)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX ".*/deps/.*")
list(FILTER SOURCES EXCLUDE REGEX ".*/main\\.c$")

include_directories(${ANDROID_NDK}/sources/android/native_app_glue/)
list(APPEND SOURCES ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

add_library(${APP_LIB_NAME} SHARED ${SOURCES})
target_compile_definitions(${APP_LIB_NAME} PRIVATE PLATFORM_ANDROID)
target_include_directories(${APP_LIB_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/raylib"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/raymob"
)
target_link_libraries(${APP_LIB_NAME} raylib raymoblib)
