// Bomb trace scenarios T1-T6 for the VG investigation (docs/vg/README.md).
// Not part of the game build. Build and run: bash docs/vg/run_trace.sh
// Real run_frame with fake keys at a fixed 1/60 s step, seed 0x4b4f545445, gameplay layout.
#include <cstdio>
#include <set>
#include <utility>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#define private public
#include "kotte/camera.hpp"
#include "kotte/application.hpp"
#undef private
static std::set<int> keys_down, keys_pressed;
extern "C" bool __wrap_IsKeyDown(int key){ return keys_down.contains(key); }
extern "C" bool __wrap_IsKeyPressed(int key){ return keys_pressed.contains(key); }
using namespace kotte; using App = Application;
static constexpr float dt = 1.0f / 60.0f;
static void teleport(App& app, EntityHandle h, Vector2 pos){
    Entity& e = app.entity(h); const Rectangle o = app.entity_world_bounds(e);
    e.world_position = pos; app.spatial_grid_.update(h, o, app.entity_world_bounds(e)); }
// Enemies are held at (13, 1..3) every frame, away from the test row 9.
static std::vector<std::pair<EntityHandle, Vector2>> parked;
static void park_enemies(App& app){ parked.clear(); int i = 0;
    for(EntityHandle h : app.enemy_handles_){ parked.push_back({h, App::tile_centre(13, 1 + i)}); ++i; } }
static void step(App& app, std::initializer_list<int> pressed = {}){
    for(auto [h, pos] : parked){ if(app.entities_.contains(h)) teleport(app, h, pos); }
    keys_pressed = pressed; app.run_frame(dt); keys_pressed.clear(); }
// Teleport the player to the tile, press Space, then move the player to (1, 1).
static void bomb_at(App& app, int c, int r){ teleport(app, app.player_handle_, App::tile_centre(c, r)); step(app, {KEY_SPACE}); teleport(app, app.player_handle_, App::tile_centre(1, 1)); }
static void wait(App& app, int frames){ for(int i = 0; i < frames; ++i) step(app); }
static void title(const char* t){ std::printf("\n## %s\n", t); }
static App make(){ return App{1280, 720, "t", 0x4b4f545445ULL, WorldLayout::gameplay, 60, true}; }
int main(){
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    title("T1 capacity: three bombs, a fourth is rejected, a fuse frees a slot, place again");
    { App app = make(); park_enemies(app);
      // Four tiles apart, so range 3 blasts do not reach the next bomb.
      bomb_at(app, 1, 9); wait(app, 10); bomb_at(app, 5, 9); wait(app, 10); bomb_at(app, 9, 9); wait(app, 10);
      bomb_at(app, 13, 9); wait(app, 120 - 33); bomb_at(app, 13, 9); wait(app, 140); }
    title("T2 remote: three bombs, E, place again");
    { App app = make(); park_enemies(app);
      bomb_at(app, 2, 9); bomb_at(app, 5, 9); bomb_at(app, 8, 9); bomb_at(app, 11, 9);
      step(app, {KEY_E}); bomb_at(app, 5, 9); step(app, {KEY_E}); wait(app, 2); }
    title("T3 one blast removes another own bomb");
    { App app = make(); park_enemies(app);
      bomb_at(app, 4, 9); wait(app, 60); bomb_at(app, 6, 9); wait(app, 70); bomb_at(app, 6, 9); step(app, {KEY_E}); }
    title("T4 restart with three active bombs, then fill to capacity again");
    { App app = make(); park_enemies(app);
      bomb_at(app, 2, 9); bomb_at(app, 5, 9); bomb_at(app, 8, 9); wait(app, 30); step(app, {KEY_R}); park_enemies(app);
      bomb_at(app, 2, 9); bomb_at(app, 5, 9); bomb_at(app, 8, 9); bomb_at(app, 11, 9); step(app, {KEY_E}); }
    title("T5 E in the frame the fuse ends");
    { App app = make(); park_enemies(app);
      bomb_at(app, 5, 9); wait(app, 30);
      app.entity(app.active_bomb_handles_[0]).bomb_state->fuse_remaining = dt * 0.5f; step(app, {KEY_E}); wait(app, 2); }
    title("T6 shield on, standing on own bombs: fuse and remote");
    { App app = make(); park_enemies(app);
      step(app, {KEY_F}); teleport(app, app.player_handle_, App::tile_centre(3, 9)); step(app, {KEY_SPACE});
      teleport(app, app.player_handle_, App::tile_centre(6, 9)); step(app, {KEY_SPACE}); step(app, {KEY_E});
      teleport(app, app.player_handle_, App::tile_centre(3, 9)); step(app, {KEY_SPACE}); wait(app, 125);
      std::printf("state=%d (0 = playing)\n", static_cast<int>(app.round_state_)); }
}
