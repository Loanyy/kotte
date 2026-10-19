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
- Solid bombs. The owner passes only while it still overlaps its bomb, so no extra flag. One bomb per tile.
- A blast-range pickup on the small map, collected once on contact. Range 3 to 4, max 5. The bomb request saves the range, so old bombs keep theirs.
- Up to 3 bombs. The count comes from the live bombs, not a counter.
- HUD shows bombs and blast range.

**Problem**
- In my Windows check, enemies died just outside the drawn blast. The blast was drawn 3 px smaller than the tile it hits. Now it draws the full tile.

**Checks**
- Linux: tests and scripted play checks passed. Capacity cycle ran 5 times. Restart resets everything.

**Commits**
- `151ad83` Add solid bombs, `31635a3` Add blast range pickup, `01242e7` Add multiple bombs, `0c4de33` Draw blast on the full tile.

**Next**
- Remote detonation.
