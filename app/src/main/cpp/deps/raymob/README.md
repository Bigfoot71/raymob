# Gradle compatibility file

`display.c` is kept because the existing `app/build.gradle` preBuild hook opens
that exact path. The current native client uses raylib directly; its CMake
configuration deliberately does not compile this legacy compatibility file.
Do not remove `display.c` unless the Gradle hook outside this directory is
changed as well.
