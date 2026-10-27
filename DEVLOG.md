# Devlog

Short notes per working day. Windows checks are my own; Linux checks are scripted test programs.

## Day 1 — 17 Oct

**Added**
- A small 15x11 map with crates and 3 enemies, so the game can be won. The big map opens with `--stress`.
- `EntityStore::clear()` for restart. It bumps the generations, so old handles are rejected.

**Problem**
- A new `EntityStore` would restart generations at 1, so an old handle could point at a new entity. Clearing keeps the slots and avoids this.

**Checks**
- Linux: tests passed. With the generation bump removed, the new test failed as expected.
- Windows: Release x64, no warnings, tests pass. `--stress`: 4570 entities, 294 enemies.

## Day 2 — 18 Oct

**Added**
- Player damage from blasts (exact rectangle test) and enemy contact.
- Playing, Won and Lost. A hit in the winning frame gives Lost. Gameplay stops after the round.
- R calls `start_round()`, which clears the store, grid, lists, queues and effects and rebuilds the world.

**Problem**
- Solid entities never overlap, so a player-enemy move that is blocked counts as contact.

**Checks**
- Linux: tests and scripted checks passed, including restart with an active bomb.
- Windows: Release x64 /W4, no warnings, tests pass. Deaths, win and restart correct.

## Day 3 — 19 Oct

**Added**
- Solid bombs. The owner passes only while still overlapping its bomb. One bomb per tile.
- Range pickup (3 to 4, max 5). Old bombs keep their range.
- Up to 3 bombs, counted from the live bombs.

**Problem**
- Enemies seemed to die outside the blast. The blast was drawn 3 px smaller than the tile it hits, so it now draws the full tile.

**Checks**
- Linux: tests and scripted checks passed.
- Windows: Release x64 /W4, no warnings, tests pass. Bombs, pickup, limit, win and restart correct.

## Day 4 — 21 Oct (Windows check 23 Oct)

**Added**
- E detonates the player's live bombs. Fuse and E share one check, so a bomb detonates once. Capacity returns in the same frame.
- `--trace-bombs`: one line per bomb event with `counted` and `verified`.

**Problem**
- The first trace printed nothing because stdout was buffered when the game was killed. Each line is now flushed.

**Checks**
- Linux: tests and scripted checks passed. A deliberately broken counter showed MISMATCH.
- Windows: Release x64 /W4, no warnings, tests pass. E, capacity and restart correct. Trace: no MISMATCH.

## Day 5 — 27 Oct

**Added**
- Shield: F gives 3 s once per round. Hits are ignored while it runs; collisions are unchanged. R resets it.
- HUD shows `blast range: 3`.
- README with the VG investigation, and `docs/vg/` with the trace scenarios, counter patches and traces.

**Problem**
- The shield lasts 181 frames, not 180, because of float steps. Kept.
- The docs named `kotte.exe`, but the build makes `kotte.release.exe`. Fixed.

**Checks**
- Linux on `aacb59f`: tests passed, 161 scripted checks 0 FAIL. Trace 0 MISMATCH; counters A 10, B 0, C 5.
- Windows on `ec3f970`: Release x64 /W4, no warnings, tests pass. Shield rules, restart and Won/Lost correct.
- Windows final check on `c7cdf48`: clean tree, Release x64 /W4 with 0 warnings, tests pass, HUD shows range 3 and 4, no raylib DLL needed, the extracted ZIP runs, trace with no MISMATCH.

## Docs — 27 Oct

- README, ROADMAP and DEVLOG made shorter. Only documentation changed; the game code and EXE are the same as on `c7cdf48`.
