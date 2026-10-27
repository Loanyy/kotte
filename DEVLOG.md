# Devlog

## Day 1 — 17 Oct

**Added**
- A small 15x11 map with 19 crates and 3 enemies, so the game can be won. The big map opens with `--stress`.
- `EntityStore::clear()` for restart. It bumps the generations, so old handles are rejected.

**Problem**
- A new `EntityStore` would restart the generations at 1, so an old handle could point at a new entity. Clearing keeps the slots and avoids this.

**Checks**
- Linux: tests passed. With the generation bump removed, the new test failed as expected.
- Windows, my check: Release x64, no warnings, tests pass. Small map: 23 entities, 3 enemies. `--stress`: 4570 entities, 294 enemies.

**Commits**
- `fdce9ac` Add assignment roadmap, `3424c89` Add small gameplay map (merge `08b4ec6`), `d17b3d0` Add clear to EntityStore (merge `8cbff38`), `859f229` Shorten comments, `d4a6425` Update roadmap status, `afd8ca7` Add devlog.

**Next**
- Player damage, Playing/Won/Lost, restart.

## Day 2 — 18 Oct

**Added**
- Blast damage uses the exact rectangle test, not the grid candidate list.
- Playing, Won and Lost in the HUD, with enemies left. Killing all enemies wins. A hit in the winning frame gives Lost. Gameplay stops.
- R calls `start_round()`. It clears the store, grid, lists, queues and effects and rebuilds the same world.

**Problem**
- Solid entities never overlap, so a move blocked by a player-enemy overlap counts as contact.

**Checks**
- Linux: tests and scripted play checks passed, including restarts with an active bomb.
- Windows, my check: Release x64 /W4, no warnings, tests pass. Deaths, win, restart and `--stress` correct.

**Commits**
- `166d3c9` Add player damage, `fbdafee` Add won and lost states, `8a3576e` Add round restart, `bc24000` Update devlog for day 2, `83e2264` Merge day 2, `5e6d975` Record day 2 checks.

**Next**
- Solid bombs, range pickup, multiple bombs.

## Day 3 — 19 Oct

**Added**
- Solid bombs. The owner passes only while still overlapping its bomb. One bomb per tile.
- A range pickup, collected once on contact (3 to 4, max 5). Old bombs keep their range.
- Up to 3 bombs, counted from the live bombs.

**Problem**
- Enemies seemed to die outside the blast. It was drawn 3 px smaller than the tile it hits, so now it draws the full tile. Partly entering a blast tile kills, as expected.

**Checks**
- Linux: tests and scripted play checks passed.
- Windows, my check: Release x64 /W4, no warnings, tests pass. 24 entities, 3 enemies. Bombs, pickup, limit, win and restart correct. `--stress` unchanged.

**Commits**
- `151ad83` Add solid bombs, `31635a3` Add blast range pickup, `01242e7` Add multiple bombs, `0c4de33` Draw blast on the full tile, `d105c50` Merge day 3, Record day 3 checks.

**Next**
- Remote detonation.

## Day 4 — 21 Oct

**Added**
- E detonates the player's live bombs. The fuse still runs. Fuse and E share one check, so a bomb detonates once. The normal blast and destroy requests remove it, so capacity returns that frame. E does nothing in Won/Lost.
- `--trace-bombs` prints one line per bomb event: `counted` is used by the capacity check, `verified` comes from scanning the whole store.

**Problem**
- The first trace printed nothing: stdout was buffered when the game was killed. Each line is now flushed.

**Checks**
- Linux: tests and scripted play checks passed. A deliberately broken counter showed MISMATCH.
- Windows, my check on 23 Oct: Release x64 /W4, no warnings, tests pass. E, capacity, restart and Lost correct. Trace of fuse, remote and restart: no MISMATCH. `--stress` unchanged.

**Commits**
- `aa5a5ff` Add remote detonation, `93ccb62` Add count_if to EntityStore, `610aef4` Add bomb trace, `a1683f6` Merge day 4, Record day 4 checks.

**Next**
- The shield.

## Day 5 — 27 Oct

**Added**
- #8 shield: F gives 3 s once per round. Hits still happen, but `evaluate_round_outcome` ignores them while it runs. Collisions are unchanged. HUD shows the time, then `used`. R resets it.
- HUD shows `blast range: 3` instead of `3/5`.
- README with the VG investigation.

**Problem**
- The timer counts down before blasts resolve, so a blast in its last frame hurts. It lasts 181 frames, not 180 (float steps); kept.

**Checks**
- Linux: tests and scripted checks passed. Trace: 0 MISMATCH; counter variants without the blast-removed bomb or the restart reset gave MISMATCH.
- Windows, my check on `ec3f970`: Release x64 /W4, no warnings, tests pass. Shield rules, restart and Won/Lost correct. `--stress`: 4570 entities, 294 enemies. HUD fix: pending.

**Commits**
- `0c360ca` Add player shield, `7081dfb` Complete README, `ec3f970` Update devlog, `f5f5391` Show blast range without the max, Merge day 5.

**Next**
- Release ZIP from the final commit.

## Final check — 27 Oct

**Added**
- `docs/vg/`: the trace scenarios, the three counter patches and their traces, so the VG evidence can be rerun.

**Problem**
- README and package steps named `kotte.exe`, but the build makes `kotte.release.exe`. Fixed. The VG text also used timings that could not be rerun, so they are removed.

**Checks**
- Linux on `aacb59f`: tests passed, 161 scripted checks 0 FAIL. Trace 0 MISMATCH; counters A 10, B 0, C 5, the same as the day 5 records.
- No code changed. Windows HUD check, final build and ZIP: pending.

**Commits**
- Add VG trace files, Update README for submission, Update devlog and roadmap for final check, Merge final check.

**Next**
- Release /W4 build and ZIP from the final commit (Okan).
