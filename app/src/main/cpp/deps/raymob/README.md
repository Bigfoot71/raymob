# raymob Android native support

This directory contains the Android native support sources from
https://github.com/Bigfoot71/raymob (MIT license; upstream copyright notices
are retained in the source files).

The Gradle preBuild step in app/build.gradle edits JNI class descriptors in
display.c and soft_keyboard.c. Both files must remain present at these paths.
CMake builds the support sources as raymoblib and links it to the application.
