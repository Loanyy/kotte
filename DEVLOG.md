# Devlog

## Day 1 — 17 Oct

**Added**
- Small gameplay map: 15x11 room, 19 crates, 3 enemies. The layout is a character table in `application.cpp`. The old 200x120 world opens with `--stress`.
- `EntityStore::clear()`: frees every slot and moves its generation on, so old handles can't match new entities. Prepares restart.

**Commits**
- `fdce9ac` Add assignment roadmap
- `3424c89` Add small gameplay map, merged in `08b4ec6`
- `d17b3d0` Add clear to EntityStore, merged in `8cbff38`
- `859f229` Shorten comments
- `d4a6425` Update roadmap status

**Checks**
- Windows, baseline `19e2866`: Release x64 build and `kotte.tests` passed. Bombs destroy crates, Q quits.
- Windows, gameplay map branch: 15x11, 23 entities, 3 enemies. `--stress`: 200x120, 4570 entities, 294 enemies, same as baseline.
- Windows, store-clear branch: Release rebuild with no warnings, tests passed.
- Linux: `kotte.tests` passed, also with a broken `clear()` the new test fails as it should. Merged version opens in both layouts.
- Windows, merged `final-assignment` (`afd8ca7`): Release x64 rebuild 2 succeeded, 0 failed, no warnings. Tests passed (exit code 0). Gameplay map 15x11, 23 entities, 3 enemies. `--stress` 200x120, 4570 entities, 294 enemies.

**Problem and decision**
- Replacing the store on restart (`entities_ = EntityStore{}`) would start generations at 1 again, so an old handle could find a new entity. Decided to clear the store instead. The test `a_new_store_can_repeat_old_handles` shows the problem.
- Interior walls only stop blasts, not movement, so the map uses crates as obstacles.

**Next**
- Day 2: player damage (blast and enemy), Playing/Won/Lost, full restart.
