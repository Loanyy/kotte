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
