#ifndef TEST_RAYLIB_H
#define TEST_RAYLIB_H
#include <cstdint>
struct Color { unsigned char r,g,b,a; };
struct Rectangle { float x,y,width,height; };
struct Vector2 { float x,y; };
enum { KEY_NULL=0, KEY_A=65, KEY_D=68, KEY_E=69, KEY_I=73, KEY_Q=81, KEY_S=83, KEY_W=87,
       KEY_BACKSPACE=259, KEY_DOWN=264, KEY_LEFT=263, KEY_RIGHT=262, KEY_UP=265, KEY_ESCAPE=256, KEY_SPACE=32 };
enum { MOUSE_BUTTON_LEFT=0 };
enum { FLAG_VSYNC_HINT=0x40 };
inline bool CheckCollisionPointRec(Vector2 p, Rectangle r){return p.x >= r.x && p.y >= r.y && p.x <= r.x+r.width && p.y <= r.y+r.height;}
inline int GetScreenWidth(){return 540;} inline int GetScreenHeight(){return 960;}
inline float GetFrameTime(){return 1.0f/60.0f;}
inline Vector2 GetMousePosition(){return Vector2{0,0};}
inline int GetTouchPointCount(){return 0;} inline Vector2 GetTouchPosition(int){return Vector2{0,0};}
inline int GetCharPressed(){return 0;}
inline bool IsKeyDown(int){return false;} inline bool IsKeyPressed(int){return false;}
inline bool IsMouseButtonDown(int){return false;} inline bool IsMouseButtonPressed(int){return false;}
inline void DrawRectangle(int,int,int,int,Color){} inline void DrawRectangleRec(Rectangle,Color){}
inline void DrawRectangleLines(int,int,int,int,Color){} inline void DrawRectangleLinesEx(Rectangle,float,Color){}
inline void DrawLine(int,int,int,int,Color){}
inline void BeginDrawing(){} inline void EndDrawing(){} inline void ClearBackground(Color){}
inline void SetConfigFlags(unsigned int){} inline void InitWindow(int,int,const char*){}
inline void SetTargetFPS(int){} inline void SetExitKey(int){} inline bool WindowShouldClose(){return true;}
inline void CloseWindow(){}
#endif
