#include "raymob.h"
#include "game/app.h"

int main(void)
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(0, 0, "");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    static nbapp::App app;
    app.init();

    while (!WindowShouldClose() && !app.quit)
    {
        app.frame();
    }

    app.shutdown();
    CloseWindow();
    return 0;
}
