// Headless stand-in for the raylib calls Kotte uses, so the trace scenarios run on Linux
// without a window. Keys come from the scenario file. Not part of the game build.
#include <raylib.h>
#include <string>
#include <vector>
extern "C" bool __wrap_IsKeyDown(int key);
extern "C" bool __wrap_IsKeyPressed(int key);
std::vector<std::string> stub_texts;
std::vector<Color> stub_rect_colors;
static int screen_w = 1280, screen_h = 720;
extern "C" {
void InitWindow(int w, int h, const char*){ screen_w = w; screen_h = h; }
void CloseWindow(void){}
bool IsWindowReady(void){ return true; }
bool WindowShouldClose(void){ return false; }
void SetConfigFlags(unsigned int){}
void SetTargetFPS(int){}
int GetScreenWidth(void){ return screen_w; }
int GetScreenHeight(void){ return screen_h; }
void BeginDrawing(void){ stub_texts.clear(); stub_rect_colors.clear(); }
void EndDrawing(void){}
void ClearBackground(Color){}
void DrawRectangle(int, int, int, int, Color){}
void DrawRectangleRounded(Rectangle, float, int, Color c){ stub_rect_colors.push_back(c); }
void DrawText(const char* t, int, int, int, Color){ stub_texts.emplace_back(t); }
void DrawFPS(int, int){}
float GetFrameTime(void){ return 1.0f / 60.0f; }
bool IsKeyDown(int key){ return __wrap_IsKeyDown(key); }
bool IsKeyPressed(int key){ return __wrap_IsKeyPressed(key); }
void TraceLog(int, const char*, ...){}
void SetTraceLogLevel(int){}
// Same test as raylib 6.0 rshapes.c.
bool CheckCollisionRecs(Rectangle a, Rectangle b){
    return a.x < b.x + b.width && a.x + a.width > b.x && a.y < b.y + b.height && a.y + a.height > b.y;
}
}
