# Assignment roadmap

Working notes for the final assignment. The submission report is `README.md`.

- Start: `2026-week-06-reference` (`19e2866`), branch `final-assignment`
- Deadline: Friday 30 October 2026, 16:00
- Extensions: #6 multiple bombs, #13 remote detonation, #8 temporary shield

## Plan

| Day | Date | Work | Status |
| --- | --- | --- | --- |
| 1 | 17 Oct | Baseline check, small gameplay map, `EntityStore::clear` | Done |
| 2 | 18 Oct | Player damage, Playing/Won/Lost, safe restart | Done |
| 3 | 19 Oct | Solid bombs, range pickup, #6 multiple bombs | Done |
| 4 | 21 Oct | #13 remote detonation, bomb trace for the VG study | Done |
| 5 | 27 Oct | #8 shield, check all eight features, VG trace and evidence, README | Done |
| 6 | 27–28 Oct | Final check of code, README and VG, Release /W4 build from the final commit, ZIP check | Final check done, build and ZIP pending |
| 7 | 29 Oct | Buffer | |
| — | 30 Oct 16:00 | Submission (Okan) | |

## Status

Only checks that were actually run count as done.

- Day 1: Windows Release x64 build with no warnings, tests pass. Small map: 23 entities, 3 enemies. `--stress`: 4570 entities, 294 enemies.
- Day 2: Windows Release x64 /W4 build with no warnings, tests pass. Blast and enemy deaths, win, freeze after the round and restart work. `--stress` unchanged.
- Day 3: Windows Release x64 /W4 build with no warnings, tests pass. Small map: 24 entities, 3 enemies. Solid bombs, pickup, 3-bomb limit, win and restart work. `--stress` unchanged.
- Day 4 (checked 23 Oct): Windows Release x64 /W4 build with no warnings, tests pass. E works with no bombs, one bomb and three bombs, and capacity returns. Restart with an active bomb and E in Lost work. Trace: counted and verified match on every line. `--stress`: 4570 entities, 294 enemies.
- Day 5 (checked 27 Oct on `ec3f970`): Windows Release x64 /W4 build with no warnings, tests pass. F starts the shield with a visible look and HUD, F again does not extend it, once per round. Blast and enemy contact do not kill during it, collisions still work, enemy contact kills after it. Restart resets it. F does nothing in Won/Lost; movement and bombs stop there. `--stress`: 4570 entities, 294 enemies. HUD blast range fix and ZIP check: pending.
- Final check (27 Oct): no code change after `aacb59f`, only docs. Windows HUD check, Release /W4 build and ZIP check on the final commit: pending.
