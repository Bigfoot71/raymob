#include "raylib.h"
#include "game/app.h"

int main() {
    SetConfigFlags(FLAG_VSYNC_HINT);
#ifdef PLATFORM_ANDROID
    InitWindow(0, 0, "");
#else
    InitWindow(540, 960, "");
#endif
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    static nbapp::App app;
    app.init();
    while (!WindowShouldClose() && !app.quit) app.frame();
    app.shutdown();
    CloseWindow();
    return 0;
}
