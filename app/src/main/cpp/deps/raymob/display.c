/*
 * Compatibility source for the existing Android Gradle preBuild hook.
 *
 * app/build.gradle opens app/src/main/cpp/deps/raymob/display.c before CMake
 * configuration. The rewritten client uses raylib's Android platform layer
 * directly and does not compile the legacy raymob display backend, therefore
 * this translation unit intentionally contains no implementation.
 *
 * Keep this file in the repository: removing it causes :app:preBuild to fail
 * with FileNotFoundException before the native build starts.
 */
