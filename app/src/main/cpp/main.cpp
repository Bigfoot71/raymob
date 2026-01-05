#include "raylib.h"
#include "raymath.h"

int main() {
    InitWindow(1280, 720, "AETHEL - MOBILE");
    SetTargetFPS(60);

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    float velocityY = 0.0f;
    bool isGrounded = true;
    Vector2 lastMousePos = { 0 };

    while (!WindowShouldClose()) {
        Vector2 mousePos = GetMousePosition();
        Vector2 delta = { mousePos.x - lastMousePos.x, mousePos.y - lastMousePos.y };
        lastMousePos = mousePos;

        camera.position.y += velocityY;
        camera.target.y += velocityY;
        if (camera.position.y > 2.0f) { velocityY -= 0.01f; isGrounded = false; }
        else { camera.position.y = 2.0f; velocityY = 0; isGrounded = true; }

        if (IsMouseButtonDown(0)) {
            Vector3 forward = Vector3Subtract(camera.target, camera.position);
            forward.y = 0; forward = Vector3Normalize(forward);
            Vector3 right = Vector3CrossProduct(forward, camera.up);

            if (CheckCollisionPointRec(mousePos, (Rectangle){120, 420, 100, 100})) { // UP
                camera.position = Vector3Add(camera.position, Vector3Scale(forward, 0.15f));
                camera.target = Vector3Add(camera.target, Vector3Scale(forward, 0.15f));
            }
            else if (CheckCollisionPointRec(mousePos, (Rectangle){950, 500, 250, 150})) { // JUMP
                if (isGrounded) velocityY = 0.2f;
            }
            else if (mousePos.x > 600) { // Geser nengok
                float sensitivity = 0.005f;
                Vector3 view = Vector3Subtract(camera.target, camera.position);
                float angleH = -delta.x * sensitivity;
                float nx = view.x * cos(angleH) - view.z * sin(angleH);
                float nz = view.x * sin(angleH) + view.z * cos(angleH);
                camera.target = (Vector3){ camera.position.x + nx, camera.target.y + (-delta.y * sensitivity * 10), camera.position.z + nz };
            }
        }

        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode3D(camera);
                DrawPlane((Vector3){ 0, 0, 0 }, (Vector2){ 100, 100 }, DARKGRAY);
                DrawGrid(100, 1.0f);
            EndMode3D();
            DrawRectangle(120, 420, 100, 100, Fade(GRAY, 0.5f)); DrawText("UP", 155, 455, 30, WHITE);
            DrawRectangle(950, 500, 250, 150, Fade(SKYBLUE, 0.6f)); DrawText("JUMP", 1060, 565, 30, WHITE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
