# Native Android client. All project-owned native files stay under app/src/main/cpp.
cmake_minimum_required(VERSION 3.22.1)

project("${APP_LIB_NAME}" LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# raylib was missing from the repository's submodule checkout. Fetch a fixed
# upstream release so local builds and CI use the same native dependency.
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

# Restore the raymob Android JNI, display, sensor, and keyboard layer.
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/deps/raymob"
                 "${CMAKE_CURRENT_BINARY_DIR}/raymob")

file(GLOB GAME_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/game/*.cpp")

add_library(${APP_LIB_NAME} SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    ${GAME_SOURCES}
)

target_include_directories(${APP_LIB_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/game"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/raymob"
)

target_compile_definitions(${APP_LIB_NAME} PRIVATE PLATFORM_ANDROID)
target_link_libraries(${APP_LIB_NAME} PRIVATE raylib raymoblib)
