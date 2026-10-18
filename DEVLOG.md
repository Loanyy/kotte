# Devlog

## Day 1 — 17 Oct

**Added**
- A small 15x11 map with 19 crates and 3 enemies, so the game can actually be finished. The old big map still opens with `--stress`.
- Added `clear()` to `EntityStore` for the restart work. It changes the generations so old handles are rejected.

**Commits**
- `fdce9ac` Add assignment roadmap
- `3424c89` Add small gameplay map (merged in `08b4ec6`)
- `d17b3d0` Add clear to EntityStore (merged in `8cbff38`)
- `859f229` Shorten comments
- `d4a6425` Update roadmap status
- `afd8ca7` Add devlog

**Checks**
- Start version `19e2866` on Windows: Release build and tests passed. Bombs break crates, Q quits.
- Merged `final-assignment` on Windows: Release x64 rebuild, 2 succeeded, no warnings. Tests passed.
- Small map: 15x11, 23 entities, 3 enemies. `--stress`: 200x120, 4570 entities, 294 enemies, same as before.
- On Linux I broke `clear()` on purpose and the new test failed, so the test works.

**Problem**
- Making a new `EntityStore` on restart would start the generations at 1 again, so an old handle could point at a new entity. I clear the store instead and keep the slots.
- Walls inside the room only stop blasts, not the player, so I used crates as obstacles.

**Next**
- Player damage from blasts and enemies, Playing/Won/Lost, full restart.

## Day 2 — 18 Oct

**Added**
- Player damage. A blast tile kills the player only after the exact rectangle test, not just because the player was a grid candidate.
- Enemy contact. Player and enemy are both solid, so they never overlap. A move that is blocked by a player-enemy overlap counts as the contact. It works from both sides: player walks in, or enemy walks in.
- Round states Playing, Won and Lost. Killing all three enemies wins. Hits are checked first, so if one blast kills the last enemy and me, I lose.
- The HUD shows the state and enemies left. After Won or Lost, movement, bombs, fuses and enemies stop.
- R restarts from any state. `start_round()` clears the store with `clear()`, empties the grid, the handle lists, all queues and blast effects, and rebuilds the world with the same seed. The constructor uses the same function.

**Commits**
- `166d3c9` Add player damage
- `fbdafee` Add won and lost states
- `8a3576e` Add round restart
- `bc24000` Update devlog for day 2
- `83e2264` Merge day 2 (`feature/day-02` into `final-assignment`)

**Checks**
- Linux, g++ 14 with ASan/UBSan and warnings as errors: `kotte.tests` passed, the game builds.
- A scratch test driver ran the real frame code with fake key input. All checks passed:
  - own bomb kills me (blast), an enemy walking into me and me walking into an enemy kill me (enemy), a crate does not;
  - an enemy one tile away diagonally is a grid candidate but does not kill me, and a blast misses a diagonal tile;
  - death happens once, and after it nothing moves for 3 seconds;
  - blast that kills all enemies gives Won; the same blast also hitting me gives Lost;
  - restart with an active bomb and a blast effect on screen, from Won, from Lost, on the detonation frame, on the death frame, and 100 times in a row. Every time: 23 entities, 3 enemies, no bombs or effects, empty queues, old handles do not resolve, the grid holds only live entities, the store does not grow, and the world is the same as a fresh start.
- Stress layout still gives 4570 entities and 294 enemies, before and after restart. Both layouts give the same world as `final-assignment` before today.
- Ran the game on a virtual screen: Space, wait, "YOU LOSE (blast)", R, back to "PLAYING | enemies left: 3".
- Windows, `feature/day-02` at `bc24000`:
  - Release x64 /W4 rebuild: 2 succeeded, 0 failed, no warnings. `EntityStore tests passed.`, exit code 0.
  - Blast death shows "YOU LOSE (blast)". Enemy contact shows "YOU LOSE (enemy)".
  - Killing all three enemies shows "YOU WIN" with 0 enemies left.
  - In Won and Lost I can't move or place a new bomb.
  - R starts a new round with 23 entities and 3 enemies. Three restarts with an active bomb left no blast from the old bomb.
  - `--stress`: 200x120, 4570 entities, 294 enemies.
- The merge needed no code changes. The merged files are the same as `bc24000`, and the Linux tests pass on the merge.

**Problem**
- Checking for overlap after moving never finds enemy contact, because solid entities never overlap. I record the contact inside the movement check, where the blocked overlap is already found. The limit is that contact distance depends on the frame step (a few pixels).
- Restarting in the middle of a phase could leave pointers to a half-built world, so the restart runs after the mutations and before drawing.

**Next**
- Day 3: solid bombs, range pickup, multiple bombs.
